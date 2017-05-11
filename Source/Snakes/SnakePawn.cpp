// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakePawn.h"
#include "SnakePawnMovementComponent.h"
#include "SnakeBody.h"
#include "SnakeBodyMovementComponent.h"
#include "SnakesGameModeBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

ASnakePawn::ASnakePawn()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
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

	InitialBodyNum = 2;
	SnakeBodyClass = ASnakeBody::StaticClass();
}

void ASnakePawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASnakePawn, BodyNum);
	DOREPLIFETIME(ASnakePawn, bIsDead);
}

void ASnakePawn::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetBodyNum(InitialBodyNum);

		// Server Logic�A��l�H����m�M���ʤ�V�C
		RebornOnRandomLocationAndDirection();
	}
}

UPawnMovementComponent* ASnakePawn::GetMovementComponent() const
{
	return SnakePawnMovementComponent;
}

void ASnakePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// ���UWASD����ƥ�C
	InputComponent->BindAxis(TEXT("MoveForward"), this, &ASnakePawn::MoveForward);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &ASnakePawn::MoveRight);
}

int32 ASnakePawn::GetBodyNum() const
{
	return BodyNum;
}

void ASnakePawn::SetBodyNum(int32 NewNum)
{
	if (BodyNum != NewNum)
	{
		BodyNum = NewNum;
		if (Role == ROLE_Authority || Role == ROLE_None)
		{
			OnRep_BodyNum();
		}
	}
}

bool ASnakePawn::GetIsDead() const
{
	return bIsDead;
}

void ASnakePawn::SetIsDead(bool bNewIsDead)
{
	if (bIsDead != bNewIsDead)
	{
		bIsDead = bNewIsDead;
		if (Role == ROLE_Authority || Role == ROLE_None)
		{
			OnRep_bIsDead();
		}
	}
}

void ASnakePawn::OnRep_BodyNum()
{
	if (BodyNum > BodyNodes.Num())
	{
		if (SnakeBodyClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;

			AActor* FollowTarget = BodyNodes.Num() == 0 ? this : Cast<AActor>(BodyNodes.Last());

			int32 Count = BodyNum - BodyNodes.Num();
			for (int Index = 0; Index < Count; ++Index)
			{
				ASnakeBody* SnakeBody = GetWorld()->SpawnActor<ASnakeBody>(SnakeBodyClass, GetActorLocation(), GetActorRotation(), SpawnParams);
				if (SnakeBody)
				{
					SnakeBody->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
					SnakeBody->GetMovementComponent()->SetFollowActor(FollowTarget);

					BodyNodes.Add(SnakeBody);

					FollowTarget = SnakeBody;
				}
			}
		}
	}
	else if (BodyNum < BodyNodes.Num())
	{
		for (int Index = BodyNum; Index < BodyNodes.Num(); ++Index)
		{
			BodyNodes[Index]->Destroy();
		}
		BodyNodes.RemoveAt(BodyNum, BodyNodes.Num() - BodyNum);
	}
}

void ASnakePawn::OnRep_bIsDead()
{
	// Client Logic�A����I���S�ġC
	if (!HasAuthority())
	{
		if (ParticleSystemComponent && ParticleSystemComponent->Template)
		{
			ParticleSystemComponent->ToggleActive();
		}
	}

	SnakePawnMovementComponent->SetComponentTickEnabled(!bIsDead);

	if (bIsDead)
	{
		for (auto* SnakeBody : BodyNodes)
		{
			SnakeBody->Die();
		}
	}
	else
	{
		for (auto* SnakeBody : BodyNodes)
		{
			SnakeBody->Reborn();
		}
	}

	ReceivebIsDead();
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
	// Server Logic�A�b�H����m�M�H����V���͡C
	if (HasAuthority())
	{
		SetIsDead(true);
		GetWorld()->GetTimerManager().SetTimer(TimeHandle_DeferredReborn, this, &ASnakePawn::RebornOnRandomLocationAndDirection, 2.0f);
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

	SetIsDead(false);
}
