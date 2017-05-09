// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakePawn.h"
#include "SnakePawnMovementComponent.h"
#include "SnakeBody.h"
#include "SnakesGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Templates/Casts.h"

ASnakePawn::ASnakePawn()
{
	// �����۰ʦ�m�P�B�ASnakePawnMovementComponent�t�dClient�����ʡC
	bReplicateMovement = false;

	// ��l��UBoxComponent�C
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitBoxExtent(FVector(50.0f));
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASnakePawn::OnBeginOverlap);

	// ���wRootComponent��UBoxComponent�C
	RootComponent = CollisionComponent;

	// ��l��UStaticMeshComponent�A�@���D�Y�C
	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleMeshComponent"));
	HeadMesh->SetupAttachment(RootComponent);
	HeadMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f));
	HeadMesh->SetCollisionProfileName(TEXT("NoCollision"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (DefaultMeshAsset.Succeeded())
	{
		HeadMesh->SetStaticMesh(DefaultMeshAsset.Object);
	}

	// ��l��USpringArmComponent�C
	CameraSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArmComponent"));
	CameraSpringArmComponent->SetupAttachment(HeadMesh);
	CameraSpringArmComponent->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-80.0f, 0.0f, 0.0f));
	CameraSpringArmComponent->TargetArmLength = 1200.0f;
	CameraSpringArmComponent->bDoCollisionTest = false;
	CameraSpringArmComponent->bEnableCameraLag = true;
	CameraSpringArmComponent->CameraLagSpeed = 3.0f;

	// ��l��UCameraComponent�C
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraSpringArmComponent, USpringArmComponent::SocketName);

	// ��l��UParticleSystemComponent�A�I���ɲ��ͯS�ġC
	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	ParticleSystemComponent->SetupAttachment(RootComponent);
	ParticleSystemComponent->bAutoActivate = false;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	if (ParticleAsset.Succeeded())
	{
		ParticleSystemComponent->SetTemplate(ParticleAsset.Object);
	}

	// ��l��USnakePawnMovementComponent�C
	SnakePawnMovementComponent = CreateDefaultSubobject<USnakePawnMovementComponent>(TEXT("SnakePawnMovementComponent"));
	SnakePawnMovementComponent->UpdatedComponent = RootComponent;
}

UPawnMovementComponent* ASnakePawn::GetMovementComponent() const
{
	return SnakePawnMovementComponent;
}

void ASnakePawn::BeginPlay()
{
	Super::BeginPlay();

	ASnakeBody* SnakeBody = GetWorld()->SpawnActor<ASnakeBody>(GetActorLocation(), GetActorRotation());
	SnakeBody->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	SnakeBody->SetFollowTarget(this);

	if (HasAuthority())
	{
		// Server Logic�A��l�H����m�M���ʤ�V�C
		RebornOnRandomLocationAndDirection();
	}
}

void ASnakePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// ���UWASD����ƥ�C
	InputComponent->BindAxis(TEXT("MoveForward"), this, &ASnakePawn::MoveForward);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &ASnakePawn::MoveRight);
}

void ASnakePawn::MoveForward(float AxisValue)
{
	// �N�T���൹USnakePawnMovementComponent�C
	AddMovementInput(FVector::ForwardVector, AxisValue);
}

void ASnakePawn::MoveRight(float AxisValue)
{
	// �N�T���൹USnakePawnMovementComponent�C
	AddMovementInput(FVector::RightVector, AxisValue);
}

void ASnakePawn::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		// Server Logic�A�b�H����m�M�H����V���͡C
		RebornOnRandomLocationAndDirection();
	}
	else
	{
		// Client Logic�A����I���S�ġC
		if (ParticleSystemComponent && ParticleSystemComponent->Template)
		{
			ParticleSystemComponent->SetWorldLocation(SweepResult.ImpactPoint);
			ParticleSystemComponent->ToggleActive();
		}
	}
}

void ASnakePawn::RebornOnRandomLocationAndDirection()
{
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(GetWorld());
	if (!GameModeBase)
	{
		return;
	}

	ASnakesGameModeBase* SnakeGameMode = Cast<ASnakesGameModeBase>(GameModeBase);
	if (!SnakeGameMode)
	{
		return;
	}

	// ��Actor�h���H����m�M�H����V�C
	FBox WorldBox = SnakeGameMode->GetWorldBox();
	FVector NewLocation = FMath::RandPointInBox(WorldBox);
	FRotator NewRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f);
	SetActorLocationAndRotation(NewLocation, NewRotator);

	//if (SnakePawnMovementComponent)
	//{
	//	FVector NewDirection = FVector::ForwardVector;
	//
	//	int32 DirectionIndex = FMath::Rand() % 4;
	//	switch (DirectionIndex)
	//	{
	//	case 0:
	//		NewDirection = FVector::ForwardVector;
	//		break;
	//	case 1:
	//		NewDirection = FVector::RightVector;
	//		break;
	//	case 2:
	//		NewDirection = -FVector::ForwardVector;
	//		break;
	//	case 3:
	//		NewDirection = -FVector::RightVector;
	//		break;
	//	default:
	//		break;
	//	}
	//
	//	SnakePawnMovementComponent->SetDirection(NewDirection);
	//}
}
