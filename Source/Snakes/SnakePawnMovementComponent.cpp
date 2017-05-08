// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakePawnMovementComponent.h"
#include "Net/UnrealNetwork.h"

USnakePawnMovementComponent::USnakePawnMovementComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	// Move 2.5 units per second.
	NormalSpeed = 250.0f;
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

	// Treat Velocity as a direction.
	Velocity = Velocity.GetSafeNormal() * NormalSpeed;

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
	if (!HasValidData() || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	if (GEngine && CurrentTickMessage < TickMessageCount)
	{
		CurrentTickMessage++;

		const UEnum* EnumNetRole = FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetRole"), true);
		FString Message = FString::Printf(TEXT("TickComponent %s %s Velocity=%s"),
			*GetOwner()->GetName(), *EnumNetRole->GetEnumName((int32)GetOwner()->Role), *Velocity.ToString());
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.0f, FColor::Orange, Message, false);
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PawnOwner->Role == ROLE_Authority)
	{
		PerformMovement(DeltaTime);
	}
	else if (PawnOwner->Role == ROLE_AutonomousProxy)
	{
		//ReplicateMoveToServer(DeltaTime, ConsumeInputVector());
	}

	UpdateComponentVelocity();
}

void USnakePawnMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(USnakePawnMovementComponent, Location);
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
	UpdatedComponent->SetWorldLocation(Location, true);
}

bool USnakePawnMovementComponent::HasValidData() const
{
	bool bIsValid = UpdatedComponent && IsValid(PawnOwner);
	return bIsValid;
}

void USnakePawnMovementComponent::ReplicateMoveToServer(float DeltaTime, FVector InputVector)
{
	PerformMovement(DeltaTime);
	ReplicateInputVector(DeltaTime, InputVector);
}

void USnakePawnMovementComponent::PerformMovement(float DeltaTime)
{
	if (!HasValidData())
	{
		return;
	}

	FVector MovementDelta = Velocity * DeltaTime;
	UpdatedComponent->MoveComponent(MovementDelta, UpdatedComponent->GetComponentQuat(), true);

	Location = GetActorLocation();
}

void USnakePawnMovementComponent::ReplicateInputVector_Implementation(float DeltaTime, FVector_NetQuantize100 InputVector)
{
	if (!HasValidData() || !IsActive())
	{
		return;
	}

	PerformMovement(DeltaTime);
}

bool USnakePawnMovementComponent::ReplicateInputVector_Validate(float DeltaTime, FVector_NetQuantize100 InputVector)
{
	return true;
}

void USnakePawnMovementComponent::SetMovementDirection(const FVector& NewDirection)
{
	Velocity = NewDirection.GetSafeNormal() * NormalSpeed;

	if (GEngine)
	{
		const UEnum* EnumNetRole = FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetRole"), true);
		FString Message = FString::Printf(TEXT("SetMovementDirection %s %s Velocity=%s"),
			*GetOwner()->GetName(), *EnumNetRole->GetEnumName((int32)GetOwner()->Role), *Velocity.ToString());
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.0f, FColor::Orange, Message, false);
	}
}
