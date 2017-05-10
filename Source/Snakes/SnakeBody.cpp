// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakeBody.h"

ASnakeBody::ASnakeBody()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = false;

	// ��l��UBoxComponent�C
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitBoxExtent(FVector(50.0f));
	CollisionComponent->SetCollisionProfileName(TEXT("NoCollision"));

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

	FollowDistance = 150.0f;
}

void ASnakeBody::BeginPlay()
{
	Super::BeginPlay();
}

void ASnakeBody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasValidData())
	{
		return;
	}

	FSnakeMove NewMove;
	if (CaptureTargetMove(NewMove))
	{
		AddMoveToHead(NewMove);
	}

	Follow(DeltaTime);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (bDrawDebugFollowPath)
	{
		for (auto* Node = FollowPath.GetHead(); Node != nullptr; Node = Node->GetNextNode())
		{
			const FSnakeMove SnakeMove = Node->GetValue();
			DrawDebugSphere(GetWorld(), SnakeMove.Location, 5.0f, 8, FColor::Yellow);
			if (Node->GetNextNode())
			{
				const FSnakeMove NextSnakeMove = Node->GetNextNode()->GetValue();
				FColor DrawColor = FVector::Dist(SnakeMove.Location, NextSnakeMove.Location) == SnakeMove.DistanceToNextMove ? FColor::Green : FColor::Red;
				DrawDebugLine(GetWorld(), SnakeMove.Location, NextSnakeMove.Location, DrawColor);
			}
		}
	}
#endif
}

void ASnakeBody::Die_Implementation()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	FollowPath.Empty();
}

void ASnakeBody::Reborn_Implementation()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
}

void ASnakeBody::SetFollowTarget(AActor* Target)
{
	FollowTarget = Target;
}

FORCEINLINE AActor* ASnakeBody::GetFollowTarget() const
{
	return FollowTarget;
}

bool ASnakeBody::HasValidData() const
{
	return FollowTarget && FollowTarget != this;
}

bool ASnakeBody::CaptureTargetMove(FSnakeMove& OutNewMove)
{
	if (!HasValidData())
	{
		return false;
	}

	OutNewMove.Location = FollowTarget->GetActorLocation();
	OutNewMove.Rotation = FollowTarget->GetActorRotation();
	OutNewMove.DistanceToNextMove = 0.0f;
	return true;
}

void ASnakeBody::AddMoveToHead(FSnakeMove& NewMove)
{
	auto* HeadNode = FollowPath.GetHead();
	FSnakeMove& HeadMove = HeadNode->GetValue();

	if (FollowPath.Num() != 0)
	{
		// ���է�s�������I�MHead�X�֡A���\����N���ª�Head�í��s�p��Z���C
		if (HeadNode->GetNextNode())
		{
			const FSnakeMove& SecondMove = HeadNode->GetNextNode()->GetValue();

			if (CanCombineMoves(HeadMove, SecondMove))
			{
				HeadMove = NewMove;
				HeadMove.DistanceToNextMove = FVector::Dist(HeadMove.Location, SecondMove.Location);
				return;
			}
		}

		// �p��s�������I��U�@�Ӳ����I���Z��(�쥻��Head)�C
		NewMove.DistanceToNextMove = FVector::Dist(HeadMove.Location, NewMove.Location);
	}

	// �s�������I�û���Head�A���F��Head����l�ܥؼЪ���m�C
	FollowPath.AddHead(NewMove);
}

bool ASnakeBody::CanCombineMoves(const FSnakeMove& BaseMove, const FSnakeMove& NewMove)
{
	if (BaseMove.Rotation.Equals(NewMove.Rotation, ERROR_TOLERANCE))
	{
		float closestDistance = FMath::PointDistToLine(NewMove.Location, BaseMove.Rotation.Vector(), BaseMove.Location);
		if (FMath::IsNearlyZero(closestDistance, ERROR_TOLERANCE))
		{
			return true;
		}
	}
	return false;
}

void ASnakeBody::Follow(float DeltaTime)
{
	if (FollowPath.Num() == 0)
	{
		return;
	}

	auto* ClosestFollowNode = FollowPath.GetHead();
	float PathDistance = 0.0f;

	for (; ClosestFollowNode != FollowPath.GetTail(); ClosestFollowNode = ClosestFollowNode->GetNextNode())
	{
		const FSnakeMove& SnakeMove = ClosestFollowNode->GetValue();
		PathDistance += SnakeMove.DistanceToNextMove;

		if (PathDistance >= FollowDistance)
		{
			break;
		}
	}

	if (ClosestFollowNode == FollowPath.GetTail())
	{
		const FSnakeMove& SnakeMove = ClosestFollowNode->GetValue();
		SetActorLocationAndRotation(SnakeMove.Location, SnakeMove.Rotation);
	}
	else
	{
		const FSnakeMove& SnakeMove = ClosestFollowNode->GetValue();
		const FSnakeMove& NextSnakeMove = ClosestFollowNode->GetNextNode()->GetValue();
		float Factor = (PathDistance - FollowDistance) / SnakeMove.DistanceToNextMove;
		FVector NewLocation = FMath::Lerp(NextSnakeMove.Location, SnakeMove.Location, Factor);
		SetActorLocationAndRotation(NewLocation, NextSnakeMove.Rotation);

		while (FollowPath.GetTail() != ClosestFollowNode->GetNextNode())
		{
			FollowPath.RemoveNode(FollowPath.GetTail());
		}
	}
}
