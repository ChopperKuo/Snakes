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
	USnakePawnMovementComponent();

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadOnly, Replicated)
	float Speed;
	UPROPERTY(Category = Gameplay, VisibleAnywhere, Transient, Replicated)
	FVector Location;
	UPROPERTY(Category = Gameplay, VisibleAnywhere, Transient, Replicated)
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
};
