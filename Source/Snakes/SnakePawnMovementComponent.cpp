// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakePawnMovementComponent.h"
#include "Net/UnrealNetwork.h"

USnakePawnMovementComponent::USnakePawnMovementComponent()
{
	bReplicates = true;
	Speed = 800.0f;
}

void USnakePawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!HasValidData() || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	if (PawnOwner->Role == ROLE_Authority)
	{
		ServerMove(DeltaTime);
	}
	else if (PawnOwner->Role <= ROLE_AutonomousProxy)
	{
		ClientMove(DeltaTime);

		if (PawnOwner->Role == ROLE_AutonomousProxy)
		{
			//if (GetPendingInputVector() != GetLastInputVector())
			{
				ReplicateInputVector(ConsumeInputVector());
			}
		}
	}

	UpdateComponentVelocity();
}

void USnakePawnMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USnakePawnMovementComponent, Speed);
	DOREPLIFETIME(USnakePawnMovementComponent, Location);
	DOREPLIFETIME(USnakePawnMovementComponent, Rotation);
	DOREPLIFETIME(USnakePawnMovementComponent, RushFactor);
}

void USnakePawnMovementComponent::Teleport_Implementation(FVector NewLocation, FRotator NewRotation)
{
	Location = NewLocation;
	Rotation = NewRotation;
	UpdatedComponent->SetWorldLocationAndRotation(NewLocation, NewRotation);
}

bool USnakePawnMovementComponent::HasValidData() const
{
	bool bIsValid = UpdatedComponent && IsValid(PawnOwner);
	return bIsValid;
}

void USnakePawnMovementComponent::ServerMove(float DeltaTime)
{
	if (!HasValidData())
	{
		return;
	}

	float SpeedFactor = RushFactor + 1.0f;

	Rotation = UpdatedComponent->GetComponentRotation();
	Rotation.Yaw += TurnFactor * SpeedFactor * 2;
	UpdatedComponent->MoveComponent(FVector::ZeroVector, Rotation, true);

	Velocity = UpdatedComponent->GetForwardVector() * Speed * SpeedFactor;
	FVector DeltaMovement = Velocity * DeltaTime;
	UpdatedComponent->MoveComponent(DeltaMovement, UpdatedComponent->GetComponentQuat(), true);

	Location = UpdatedComponent->GetComponentLocation();
}

void USnakePawnMovementComponent::ClientMove(float DeltaTime)
{
	if (!HasValidData())
	{
		return;
	}

	FVector CurrentLocation = UpdatedComponent->GetComponentLocation();
	if (CurrentLocation == Location)
	{
		return;
	}

	FVector DeltaMovement = Location - CurrentLocation;
	float Factor = FMath::Clamp((Speed * (RushFactor + 1.0f) * DeltaTime) / DeltaMovement.Size(), 0.0f, 1.0f);
	FVector NewLocation = FMath::LerpStable(CurrentLocation, Location, Factor);

	UpdatedComponent->SetWorldLocationAndRotation(NewLocation, Rotation);

	//DrawDebugSphere(GetWorld(), Location, 5.0f, 8, FColor::Red, true, 10.0f);
	//DrawDebugSphere(GetWorld(), Location, 2.5f, 8, FColor::Green, true, 10.0f);
}

void USnakePawnMovementComponent::ReplicateInputVector_Implementation(FVector_NetQuantize100 InputVector)
{
	TurnFactor = FMath::Clamp(InputVector.Y, -1.0f, 1.0f);
	RushFactor = FMath::Clamp(InputVector.X, 0.0f, 1.0f);
}

bool USnakePawnMovementComponent::ReplicateInputVector_Validate(FVector_NetQuantize100 InputVector)
{
	return true;
}
