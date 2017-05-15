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
	class USnakePawnMovementComponent* SnakePawnMovementComponent;

public:
	//~ Begin UObject Interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UObject Interface

protected:
	//~ Begin AActor Interface
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

	UFUNCTION(Category = Gameplay, BlueprintCallable)
	void SetBodyNum(int32 NewNum);

	UFUNCTION(Category = Gameplay, BlueprintCallable)
	bool GetIsDead() const;

	UFUNCTION(Category = Gameplay, BlueprintCallable)
	void SetIsDead(bool bNewIsDead);

protected:
	/** Replication Notification Callbacks */
	UFUNCTION()
	virtual void OnRep_BodyNum();
	UFUNCTION(Category = Gameplay, BlueprintImplementableEvent, BlueprintCosmetic, meta = (DisplayName = "OnRep_BodyNum"))
	void ReceiveBodyNum();

	UFUNCTION()
	virtual void OnRep_bIsDead();
	UFUNCTION(Category = Gameplay, BlueprintImplementableEvent, BlueprintCosmetic, meta = (DisplayName = "OnRep_bIsDead"))
	void ReceivebIsDead();

private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void HandleEatCandy();
	void HandleDeathAndReborn();

	void Reborn();

public:
	static FName SnakeTagName;

private:
	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 InitialBodyNum;

	UPROPERTY(Category = Gameplay, VisibleDefaultsOnly, Transient, ReplicatedUsing = OnRep_BodyNum, meta = (AllowPrivateAccess = "true"))
	int32 BodyNum;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ASnakeBody> SnakeBodyClass;

	TArray<ASnakeBody*> BodyNodes;

	UPROPERTY(Category = Gameplay, VisibleDefaultsOnly, Transient, ReplicatedUsing = OnRep_bIsDead, meta = (AllowPrivateAccess = "true"))
	bool bIsDead;

	/** Handle for reborn timer */
	FTimerHandle TimeHandle_DeferredReborn;
};
