// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SnakeBody.generated.h"

#define ERROR_TOLERANCE (0.1f)

struct FSnakeMove
{
	FVector Location;

	FRotator Rotation;

	float DistanceToNextMove;
};

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

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bDrawDebugFollowPath;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	void SetFollowTarget(AActor* Target);
	FORCEINLINE AActor* GetFollowTarget() const;

private:
	bool HasValidData() const;

	bool CaptureTargetMove(FSnakeMove& OutNewMove);
	void AddMoveToHead(FSnakeMove& NewMove);
	bool CanCombineMoves(const FSnakeMove& BaseMove, const FSnakeMove& NewMove);

	void Follow(float DeltaTime);

private:
	UPROPERTY(Category = Gameplay, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AActor* FollowTarget;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float FollowDistance;

	TDoubleLinkedList<FSnakeMove> FollowPath;
};
