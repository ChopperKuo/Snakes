// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "SnakePawn.generated.h"

UCLASS()
class SNAKES_API ASnakePawn : public APawn
{
	GENERATED_BODY()

public:
	ASnakePawn();

private:
	UPROPERTY(Category = Gameplay, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionComponent;

	UPROPERTY(Category = Gameplay, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* HeadMesh;

	UPROPERTY(Category = Gameplay, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraSpringArmComponent;
	UPROPERTY(Category = Gameplay, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	UPROPERTY(Category = Gameplay, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* ParticleSystemComponent;

	UPROPERTY(Category = Gameplay, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USnakePawnMovementComponent* SnakePawnMovementComponent;

	//~ Begin AActor Interface
protected:
	virtual void BeginPlay() override;
	//~ End AActor Interface

	//~ Begin APawn Interface
public:
	virtual UPawnMovementComponent* GetMovementComponent() const override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//~ End APawn Interface

public:
	UFUNCTION(Category = Gameplay, BlueprintCallable)
	int32 GetBodyNum() const;
	void SetBodyNum(int32 NewNum);

private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void RebornOnRandomLocationAndDirection();

private:
	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ASnakeBody> SnakeBodyClass;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 InitialBodyNum;

	TArray<ASnakeBody*> BodyNodes;
};
