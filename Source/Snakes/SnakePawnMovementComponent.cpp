// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakePawnMovementComponent.h"

USnakePawnMovementComponent::USnakePawnMovementComponent(const FObjectInitializer& ObjectInitializer)
{
	// Move 2.5 units per second.
	NormalSpeed = 100.0f;
	Velocity = FVector::ForwardVector;
}

void USnakePawnMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Treat Velocity as a direction.
	Velocity = Velocity.GetSafeNormal() * NormalSpeed;
}

void USnakePawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (!HasValidData() || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PawnOwner->Role > ROLE_SimulatedProxy)
	{
		if (PawnOwner->Role == ROLE_Authority)
		{
			//PerformMovement(DeltaTime);
		}
		else if (PawnOwner->Role == ROLE_AutonomousProxy)
		{
			//if (GetPendingInputVector() != GetLastInputVector())
			{
				ReplicateMoveToServer(DeltaTime, ConsumeInputVector());
			}
		}
	}

	UpdateComponentVelocity();
}

bool USnakePawnMovementComponent::HasValidData() const
{
	bool bIsValid = UpdatedComponent && IsValid(PawnOwner);
	return bIsValid;
}

void USnakePawnMovementComponent::ReplicateMoveToServer(float DeltaTime, FVector InputVector)
{
	PerformMovement(DeltaTime, InputVector);
	ReplicateInputVector(DeltaTime, InputVector);
}

void USnakePawnMovementComponent::PerformMovement(float DeltaTime, FVector InputVector)
{
	if (!HasValidData())
	{
		return;
	}

	if (!InputVector.IsZero())
	{
		float DotValue = FVector::DotProduct(Velocity, InputVector);
		if (DotValue >= 0)
		{
			Velocity = InputVector.GetSafeNormal() * NormalSpeed;
		}
	}

	FVector MovementDelta = Velocity * DeltaTime;
	MoveUpdatedComponent(MovementDelta, UpdatedComponent->GetComponentRotation(), false);

	//if (GEngine)
	//{
	//	const UEnum* EnumNetRole = FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetRole"), true);
	//	FString Message = FString::Printf(TEXT("%s %s PerformMovement InputVector=%s ActorLocation=%s"), *PawnOwner->GetName(), *EnumNetRole->GetEnumName((int)PawnOwner->Role), *InputVector.ToString(), *GetActorLocation().ToString());
	//	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.0f, FColor::Yellow, Message);
	//}
}

void USnakePawnMovementComponent::ReplicateInputVector_Implementation(float DeltaTime, FVector_NetQuantize100 InputVector)
{
	if (!HasValidData() || !IsActive())
	{
		return;
	}

	PerformMovement(DeltaTime, InputVector);
}

bool USnakePawnMovementComponent::ReplicateInputVector_Validate(float DeltaTime, FVector_NetQuantize100 InputVector)
{
	return true;
}
