// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakeBody.h"
#include "SnakeBodyMovementComponent.h"

ASnakeBody::ASnakeBody()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;

	// 初始化UBoxComponent。
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitBoxExtent(FVector(50.0f));
	CollisionComponent->SetCollisionProfileName(TEXT("NoCollision"));

	// 指定RootComponent為UBoxComponent。
	RootComponent = CollisionComponent;

	// 初始化UStaticMeshComponent，作為蛇頭。
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleMeshComponent"));
	BodyMesh->SetupAttachment(RootComponent);
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f));
	BodyMesh->SetCollisionProfileName(TEXT("NoCollision"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (DefaultMeshAsset.Succeeded())
	{
		BodyMesh->SetStaticMesh(DefaultMeshAsset.Object);
	}

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
	SetActorTickEnabled(false);
}

void ASnakeBody::Reborn_Implementation()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
}
