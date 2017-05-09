// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SnakeBody.generated.h"

struct FSnakeMove
{
	FVector Location;

	FRotator Rotation;

	float DeltaTime;
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

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	void SetFollowTarget(AActor* Target);
	FORCEINLINE AActor* GetFollowTarget() const;

private:
	void CaptureTargetMove(float DeltaTime);
	static bool TryCombineTwoMoves(const FSnakeMove& FirstMove, const FSnakeMove& SecondMove, FSnakeMove& NewMove);

private:
	UPROPERTY(Category = Gameplay, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AActor* FollowTarget;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float FollowDistance;

	TArray<FSnakeMove> FollowPath;
};
