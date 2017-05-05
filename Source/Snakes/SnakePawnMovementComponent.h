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

	virtual void InitializeComponent() override;

protected:
	UPROPERTY(Category = Snake, EditAnywhere, BlueprintReadWrite)
	float NormalSpeed;

protected:
	bool HasValidData() const;

	void ReplicateMoveToServer(float DeltaTime, FVector InputVector);

	void PerformMovement(float DeltaTime);

	UFUNCTION(Category = Snake, Server, Unreliable, WithValidation)
	void ReplicateInputVector(FVector_NetQuantize100 InputVector);
	void ReplicateInputVector_Implementation(FVector_NetQuantize100 InputVector);
	bool ReplicateInputVector_Validate(FVector_NetQuantize100 InputVector);
};
