// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakeBody.h"

ASnakeBody::ASnakeBody()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 初始化UBoxComponent。
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitBoxExtent(FVector(50.0f));
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// 指定RootComponent為UBoxComponent。
	RootComponent = CollisionComponent;

	// 初始化UStaticMeshComponent，作為蛇頭。
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
	if (!FollowTarget)
	{
		return;
	}

	// 取得目標的位置和旋轉量。
	FSnakeMove NewMove = CreateMove();
	NewMove.DeltaTime = DeltaTime;

	// 嘗試把新的移動合併到最後一筆紀錄；否則新增新的移動。
	if (FollowPath.Num() != 0)
	{
		FSnakeMove LastMove = FollowPath.Last();
		if (TryCombineTwoMoves(LastMove, NewMove, NewMove))
		{
			FollowPath.Last() = NewMove;
			return;
		}
	}

	FollowPath.Add(NewMove);
}

FSnakeMove ASnakeBody::CreateMove()
{
	FSnakeMove NewMove;
	if (FollowTarget)
	{
		NewMove.Location = FollowTarget->GetActorLocation();
		NewMove.Rotation = FollowTarget->GetActorRotation();
	}
	return NewMove;
}

bool ASnakeBody::TryCombineTwoMoves(const FSnakeMove& BaseMove, const FSnakeMove& IncomingMove, FSnakeMove& NewMove)
{
	if (BaseMove.Rotation.Equals(IncomingMove.Rotation))
	{
		float closestDistance = FMath::PointDistToLine(IncomingMove.Location, BaseMove.Rotation.Vector(), BaseMove.Location);
		if (closestDistance == 0.0f)
		{
			NewMove = BaseMove;
			NewMove.DeltaTime += IncomingMove.DeltaTime;
			return true;
		}
	}
	return false;
}
