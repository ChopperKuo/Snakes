// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PawnMovementComponent.h"
#include "SnakePawnMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SNAKES_API USnakePawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
public:
	USnakePawnMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadWrite, Transient, Replicated)
	float Speed;
	UPROPERTY(Transient, Replicated)
	FVector Location;
	UPROPERTY(Transient, Replicated)
	FRotator Rotation;
	float TurnFactor;
	float RushFactor;

protected:
	bool HasValidData() const;

	void ServerMove(float DeltaTime);

	void ClientMove(float DeltaTime);

	UFUNCTION(Category = Gameplay, Server, Unreliable, WithValidation)
	void ReplicateInputVector(FVector_NetQuantize100 InputVector);
	void ReplicateInputVector_Implementation(FVector_NetQuantize100 InputVector);
	bool ReplicateInputVector_Validate(FVector_NetQuantize100 InputVector);

public:
	void SetDirection(const FVector& NewDirection, bool bConstrainToOriginalDirectioin = false);
	FORCEINLINE FVector GetDirection() const;
};
