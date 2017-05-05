// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "SnakePawn.generated.h"

UCLASS()
class SNAKES_API ASnakePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASnakePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

protected:
	UPROPERTY(Category = Snake, VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* CollisionComponent;
	UPROPERTY(Category = Snake, VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* VisibleMeshComponent;
	UPROPERTY(Category = Snake, VisibleAnywhere, BlueprintReadOnly)
	USpringArmComponent* CameraSpringArmComponent;
	UPROPERTY(Category = Snake, VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* CameraComponent;

protected:
	UPROPERTY(Category = Snake, VisibleAnywhere, BlueprintReadOnly)
	class USnakePawnMovementComponent* SnakePawnMovementComponent;

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
};
