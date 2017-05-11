// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakeBodyMovementComponent.h"


// Sets default values for this component's properties
USnakeBodyMovementComponent::USnakeBodyMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	FollowDistance = 150.0f;
	bDrawDebugFollowPath = false;
}

// Called every frame
void USnakeBodyMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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

void USnakeBodyMovementComponent::SetFollowActor(AActor* Target)
{
	FollowActor = Target;
}

AActor* USnakeBodyMovementComponent::GetFollowActor() const
{
	return FollowActor;
}

void USnakeBodyMovementComponent::Stop()
{
	FollowPath.Empty();
}

bool USnakeBodyMovementComponent::HasValidData() const
{
	return GetOwner() && FollowActor;
}

bool USnakeBodyMovementComponent::CaptureTargetMove(FSnakeMove& OutNewMove)
{
	if (!HasValidData())
	{
		return false;
	}

	OutNewMove.Location = FollowActor->GetActorLocation();
	OutNewMove.Rotation = FollowActor->GetActorRotation();
	OutNewMove.DistanceToNextMove = 0.0f;
	return true;
}

void USnakeBodyMovementComponent::AddMoveToHead(FSnakeMove& NewMove)
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

bool USnakeBodyMovementComponent::CanCombineMoves(const FSnakeMove& BaseMove, const FSnakeMove& NewMove)
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

void USnakeBodyMovementComponent::Follow(float DeltaTime)
{
	if (!HasValidData())
	{
		return;
	}

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
		GetOwner()->SetActorLocationAndRotation(SnakeMove.Location, SnakeMove.Rotation);
	}
	else
	{
		const FSnakeMove& SnakeMove = ClosestFollowNode->GetValue();
		const FSnakeMove& NextSnakeMove = ClosestFollowNode->GetNextNode()->GetValue();
		float Factor = (PathDistance - FollowDistance) / SnakeMove.DistanceToNextMove;
		FVector NewLocation = FMath::Lerp(NextSnakeMove.Location, SnakeMove.Location, Factor);
		GetOwner()->SetActorLocationAndRotation(NewLocation, NextSnakeMove.Rotation);

		while (FollowPath.GetTail() != ClosestFollowNode->GetNextNode())
		{
			FollowPath.RemoveNode(FollowPath.GetTail());
		}
	}
}

