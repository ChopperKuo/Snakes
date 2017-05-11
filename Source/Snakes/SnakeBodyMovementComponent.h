// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "SnakeBodyMovementComponent.generated.h"

#define ERROR_TOLERANCE (0.1f)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SNAKES_API USnakeBodyMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	struct FSnakeMove
	{
		FVector Location;

		FRotator Rotation;

		float DistanceToNextMove;
	};

public:	
	// Sets default values for this component's properties
	USnakeBodyMovementComponent();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(Category = Gameplay, BlueprintCallable)
	void SetFollowActor(AActor* Target);
	UFUNCTION(Category = Gameplay, BlueprintCallable)
	AActor* GetFollowActor() const;

	UFUNCTION(Category = Gameplay, BlueprintCallable)
	virtual void Stop();

private:
	bool HasValidData() const;

	bool CaptureTargetMove(FSnakeMove& OutNewMove);
	void AddMoveToHead(FSnakeMove& NewMove);
	bool CanCombineMoves(const FSnakeMove& BaseMove, const FSnakeMove& NewMove);

	void Follow(float DeltaTime);

private:
	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float FollowDistance;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bDrawDebugFollowPath;

	AActor* FollowActor;
	TDoubleLinkedList<FSnakeMove> FollowPath;
};
