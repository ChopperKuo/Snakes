// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakeBody.h"

ASnakeBody::ASnakeBody() : Super()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ��l��UBoxComponent�C
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitBoxExtent(FVector(50.0f));
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// ���wRootComponent��UBoxComponent�C
	RootComponent = CollisionComponent;

	// ��l��UStaticMeshComponent�A�@���D�Y�C
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleMeshComponent"));
	BodyMesh->SetupAttachment(RootComponent);
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f));
	BodyMesh->SetCollisionProfileName(TEXT("NoCollision"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (DefaultMeshAsset.Succeeded())
	{
		BodyMesh->SetStaticMesh(DefaultMeshAsset.Object);
	}
}

void ASnakeBody::BeginPlay()
{
	Super::BeginPlay();
}

void ASnakeBody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CaptureTargetMove(DeltaTime);
}

void ASnakeBody::SetFollowTarget(AActor* Target)
{
	FollowTarget = Target;
}

FORCEINLINE AActor* ASnakeBody::GetFollowTarget() const
{
	return FollowTarget;
}

void ASnakeBody::CaptureTargetMove(float DeltaTime)
{
	// ���o�ؼЪ���m�M����q�C
	FSnakeMove NewMove;
	NewMove.Location = FollowTarget->GetActorLocation();
	NewMove.Rotation = FollowTarget->GetActorRotation();

	if (FollowPath.Num() == 0)
	{
		FollowPath.Add(NewMove);
		return;
	}

	FSnakeMove LastMove = FollowPath.Last();
	if (NewMove.Rotation.Equals(LastMove.Rotation))
	{
	}
}

bool ASnakeBody::TryCombineTwoMoves(const FSnakeMove& FirstMove, const FSnakeMove& SecondMove, FSnakeMove& NewMove)
{
	if (FirstMove.Rotation.Equals(SecondMove.Rotation))
	{
	}
	return false;
}
