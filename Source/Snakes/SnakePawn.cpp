// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakePawn.h"
#include "SnakePawnMovementComponent.h"

// Sets default values
ASnakePawn::ASnakePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitBoxExtent(FVector(50.0f));
	RootComponent = CollisionComponent;

	VisibleMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleMeshComponent"));
	VisibleMeshComponent->SetupAttachment(RootComponent);
	VisibleMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (DefaultMeshAsset.Succeeded())
	{
		VisibleMeshComponent->SetStaticMesh(DefaultMeshAsset.Object);
	}

	CameraSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArmComponent"));
	CameraSpringArmComponent->SetupAttachment(VisibleMeshComponent);
	CameraSpringArmComponent->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-80.0f, 0.0f, 0.0f));
	CameraSpringArmComponent->TargetArmLength = 800.0f;
	CameraSpringArmComponent->bEnableCameraLag = true;
	CameraSpringArmComponent->CameraLagSpeed = 3.0f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraSpringArmComponent, USpringArmComponent::SocketName);

	SnakePawnMovementComponent = CreateDefaultSubobject<USnakePawnMovementComponent>(TEXT("SnakePawnMovementComponent"));
	SnakePawnMovementComponent->SetUpdatedComponent(RootComponent);
}

// Called when the game starts or when spawned
void ASnakePawn::BeginPlay()
{
	Super::BeginPlay();
	
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

void ASnakePawn::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector(), AxisValue);
	//if (SnakePawnMovementComponent != nullptr && SnakePawnMovementComponent->UpdatedComponent == RootComponent)
	//{
	//	SnakePawnMovementComponent->AddInputVector(GetActorForwardVector() * AxisValue);
	//}
}

void ASnakePawn::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector(), AxisValue);
	//if (SnakePawnMovementComponent != nullptr && SnakePawnMovementComponent->UpdatedComponent == RootComponent)
	//{
	//	SnakePawnMovementComponent->AddInputVector(GetActorRightVector() * AxisValue);
	//}
}
