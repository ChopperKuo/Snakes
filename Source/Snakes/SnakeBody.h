// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SnakeBody.generated.h"

UCLASS()
class SNAKES_API ASnakeBody : public AActor
{
	GENERATED_BODY()
	
public:	
	ASnakeBody();

private:
	UPROPERTY(Category = Gameplay, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionComponent;

	UPROPERTY(Category = Gameplay, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BodyMesh;

	UPROPERTY(Category = Gameplay, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USnakeBodyMovementComponent* MovementComponent;

public:
	UFUNCTION(Category = Gameplay, BlueprintCallable)
	virtual USnakeBodyMovementComponent* GetMovementComponent() const;

	UFUNCTION(Category = Gameplay, BlueprintNativeEvent)
	void Die();
	virtual void Die_Implementation();

	UFUNCTION(Category = Gameplay, BlueprintNativeEvent)
	void Reborn();
	virtual void Reborn_Implementation();
};
