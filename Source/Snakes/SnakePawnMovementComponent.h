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
	virtual void InitializeComponent() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

protected:
	UPROPERTY(Category = Gameplay, VisibleAnywhere, BlueprintReadOnly, Transient, ReplicatedUsing = OnRep_Location)
	FVector Location;
	UPROPERTY(Category = Gameplay, VisibleAnywhere, BlueprintReadOnly, Transient, ReplicatedUsing = OnRep_Location)
	FRotator Rotation;
	UPROPERTY(Category = Gameplay, VisibleAnywhere, BlueprintReadOnly, Transient, Replicated)
	float TurnFactor;
	UPROPERTY(Category = Gameplay, VisibleAnywhere, BlueprintReadOnly, Transient, Replicated)
	float RushFactor;
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite, Transient, Replicated)
	float Speed;

	UFUNCTION()
	void OnRep_Location();

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
