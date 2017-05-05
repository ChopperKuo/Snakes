// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakePawnMovementComponent.h"

USnakePawnMovementComponent::USnakePawnMovementComponent(const FObjectInitializer& ObjectInitializer)
{
	// Move 2.5 units per second.
	NormalSpeed = 250.0f;
	Velocity = FVector::ForwardVector;
}

void USnakePawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (!HasValidData() || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PawnOwner->HasAuthority())
	{
		PerformMovement(DeltaTime);
	}
	else
	{
		FVector InputVector = ConsumeInputVector();
		ReplicateMoveToServer(DeltaTime, InputVector);
	}

	UpdateComponentVelocity();
}

void USnakePawnMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	// Treat Velocity as a direction.
	Velocity = Velocity.SafeNormal() * NormalSpeed;
}

bool USnakePawnMovementComponent::HasValidData() const
{
	bool bIsValid = UpdatedComponent && IsValid(PawnOwner);
	return bIsValid;
}

void USnakePawnMovementComponent::ReplicateMoveToServer(float DeltaTime, FVector InputVector)
{
	PerformMovement(DeltaTime);
	ReplicateInputVector(InputVector);
}

void USnakePawnMovementComponent::PerformMovement(float DeltaTime)
{
	if (!HasValidData())
	{
		return;
	}

	FVector MovementDelta = Velocity * DeltaTime;
	MoveUpdatedComponent(MovementDelta, UpdatedComponent->GetComponentRotation(), false);
}

void USnakePawnMovementComponent::ReplicateInputVector_Implementation(FVector_NetQuantize100 InputVector)
{
	if (!HasValidData() || !IsActive())
	{
		return;
	}

	//PerformMovement(DeltaTime, InputVector);
}

bool USnakePawnMovementComponent::ReplicateInputVector_Validate(FVector_NetQuantize100 InputVector)
{
	return true;
}
