// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakePawnMovementComponent.h"
#include "Net/UnrealNetwork.h"

USnakePawnMovementComponent::USnakePawnMovementComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	Speed = 400.0f;
	Velocity = FVector::ForwardVector;
	bReplicates = true;
}

void USnakePawnMovementComponent::InitializeComponent()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.0f, FColor::Orange, FString(TEXT("InitializeComponent(Begin)")), false);
	}

	Super::InitializeComponent();

	if (GEngine)
	{
		const UEnum* EnumNetRole = FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetRole"), true);
		FString Message = FString::Printf(TEXT("InitializeComponent(End) %s %s Velocity=%s"),
			*GetOwner()->GetName(), *EnumNetRole->GetEnumName((int32)GetOwner()->Role), *Velocity.ToString());
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.0f, FColor::Orange, Message, false);
	}
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
	
	DOREPLIFETIME(USnakePawnMovementComponent, Location);
	DOREPLIFETIME(USnakePawnMovementComponent, Rotation);
	DOREPLIFETIME(USnakePawnMovementComponent, TurnFactor);
	DOREPLIFETIME(USnakePawnMovementComponent, RushFactor);
	DOREPLIFETIME(USnakePawnMovementComponent, Speed);
}

void USnakePawnMovementComponent::BeginPlay()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.0f, FColor::Orange, FString(TEXT("BeginPlay(Begin)")), false);
	}

	Super::BeginPlay();

	if (GEngine)
	{
		const UEnum* EnumNetRole = FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetRole"), true);
		FString Message = FString::Printf(TEXT("BeginPlay(End) %s %s Velocity=%s"),
			*GetOwner()->GetName(), *EnumNetRole->GetEnumName((int32)GetOwner()->Role), *Velocity.ToString());
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.0f, FColor::Orange, Message, false);
	}
}

void USnakePawnMovementComponent::OnRep_Location()
{
	//UpdatedComponent->SetWorldLocation(Location);
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

	float Factor = RushFactor + 1.0f;

	Rotation = UpdatedComponent->GetComponentRotation();
	Rotation.Yaw += TurnFactor * Factor;
	UpdatedComponent->MoveComponent(FVector::ZeroVector, Rotation, true);

	Velocity = UpdatedComponent->GetForwardVector() * Speed * Factor;
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

	FVector NewLocation = Location;
	if (Speed > 0.0f)
	{
		FVector DeltaMovement = NewLocation - CurrentLocation;
		float Factor = FMath::Clamp(DeltaMovement.Size() / (Speed * DeltaTime), 0.0f, 1.0f);
		NewLocation = FMath::Lerp(CurrentLocation, NewLocation, Factor);
	}

	UpdatedComponent->SetWorldLocationAndRotation(NewLocation, Rotation);
}

void USnakePawnMovementComponent::ReplicateInputVector_Implementation(FVector_NetQuantize100 InputVector)
{
	TurnFactor = InputVector.Y;
	RushFactor = FMath::Clamp(InputVector.X, 0.0f, 1.0f);
}

bool USnakePawnMovementComponent::ReplicateInputVector_Validate(FVector_NetQuantize100 InputVector)
{
	return true;
}

void USnakePawnMovementComponent::SetDirection(const FVector& NewDirection, bool bConstrainToOriginalDirectioin)
{
	//if (NewDirection.IsNearlyZero())
	//{
	//	return;
	//}
	//
	//if (bConstrainToOriginalDirectioin)
	//{
	//	float ScaleProjection = NewDirection | Direction;
	//	if (ScaleProjection >= 0.0f)
	//	{
	//		Direction = NewDirection.GetSafeNormal();
	//	}
	//}
	//else
	//{
	//	Direction = NewDirection.GetSafeNormal();
	//}
}

FORCEINLINE FVector USnakePawnMovementComponent::GetDirection() const
{
	return FVector::ForwardVector;
}
