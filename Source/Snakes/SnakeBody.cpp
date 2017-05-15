// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakeBody.h"
#include "SnakeBodyMovementComponent.h"

ASnakeBody::ASnakeBody()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
	Tags.AddUnique(TEXT("Snake"));

	// 初始化UBoxComponent。
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitBoxExtent(FVector(50.0f));
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// 指定RootComponent為UBoxComponent。
	RootComponent = CollisionComponent;

	MovementComponent = CreateDefaultSubobject<USnakeBodyMovementComponent>(TEXT("SnakeBodyMovementComponent"));
}

USnakeBodyMovementComponent* ASnakeBody::GetMovementComponent() const
{
	return MovementComponent;
}

void ASnakeBody::Die_Implementation()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	//SetActorTickEnabled(false);
	MovementComponent->Stop();
}

void ASnakeBody::Reborn_Implementation()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	//SetActorTickEnabled(true);
	MovementComponent->TeleportToFollowActor();
}
