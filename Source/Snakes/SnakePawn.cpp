// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakePawn.h"
#include "SnakePawnMovementComponent.h"
#include "SnakesGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Templates/Casts.h"

// Sets default values
ASnakePawn::ASnakePawn() :
	Super()
{
	bReplicateMovement = false;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitBoxExtent(FVector(50.0f));
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASnakePawn::OnBeginOverlap);

	RootComponent = CollisionComponent;

	VisibleMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleMeshComponent"));
	VisibleMeshComponent->SetupAttachment(RootComponent);
	VisibleMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f));
	VisibleMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (DefaultMeshAsset.Succeeded())
	{
		VisibleMeshComponent->SetStaticMesh(DefaultMeshAsset.Object);
	}

	CameraSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArmComponent"));
	CameraSpringArmComponent->SetupAttachment(VisibleMeshComponent);
	CameraSpringArmComponent->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-80.0f, 0.0f, 0.0f));
	CameraSpringArmComponent->TargetArmLength = 800.0f;
	CameraSpringArmComponent->bDoCollisionTest = false;
	CameraSpringArmComponent->bEnableCameraLag = true;
	CameraSpringArmComponent->CameraLagSpeed = 3.0f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraSpringArmComponent, USpringArmComponent::SocketName);

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	ParticleSystemComponent->SetupAttachment(RootComponent);
	ParticleSystemComponent->bAutoActivate = false;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	if (ParticleAsset.Succeeded())
	{
		ParticleSystemComponent->SetTemplate(ParticleAsset.Object);
	}

	SnakePawnMovementComponent = CreateDefaultSubobject<USnakePawnMovementComponent>(TEXT("SnakePawnMovementComponent"));
	SnakePawnMovementComponent->UpdatedComponent = RootComponent;
}

// Called when the game starts or when spawned
void ASnakePawn::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine)
	{
		const UEnum* EnumNetRole = FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetRole"), true);
		FString Message = FString::Printf(TEXT("%s %s BeginPlay"), *GetName(), *EnumNetRole->GetEnumName((int32)Role));
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.0f, FColor::Yellow, Message);
	}

	if (HasAuthority())
	{
		RebornOnRandomLocationAndRotation();
	}
}

// Called every frame
void ASnakePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASnakePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis(TEXT("MoveForward"), this, &ASnakePawn::MoveForward);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &ASnakePawn::MoveRight);
}

UPawnMovementComponent* ASnakePawn::GetMovementComponent() const
{
	return SnakePawnMovementComponent;
}

void ASnakePawn::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GEngine)
	{
		const UEnum* EnumNetRole = FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetRole"), true);
		FString Message = FString::Printf(TEXT("%s %s OnBeginOverlap OtherActor=%s"), *GetName(), *EnumNetRole->GetEnumName((int32)Role), *OtherActor->GetName());
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.0f, FColor::Yellow, Message);
	}

	if (HasAuthority())
	{
		RebornOnRandomLocationAndRotation();
	}
	else
	{
		if (ParticleSystemComponent && ParticleSystemComponent->Template)
		{
			ParticleSystemComponent->SetWorldLocation(SweepResult.ImpactPoint);
			ParticleSystemComponent->ToggleActive();
		}
	}
}

void ASnakePawn::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector(), AxisValue);
}

void ASnakePawn::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector(), AxisValue);
}

void ASnakePawn::RebornOnRandomLocationAndRotation()
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

	FBox WorldBox = SnakeGameMode->GetWorldBox();
	FVector NewLocation = FMath::RandPointInBox(WorldBox);
	SetActorLocation(NewLocation);

	UPawnMovementComponent* MyMovement = GetMovementComponent();
	if (MyMovement && SnakePawnMovementComponent)
	{
		FVector NewDirection = FVector(FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f), 0.0f);
		SnakePawnMovementComponent->SetMovementDirection(NewDirection);
	}
}
