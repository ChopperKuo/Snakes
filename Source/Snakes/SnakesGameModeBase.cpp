// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakesGameModeBase.h"
#include "CandyActor.h"

void ASnakesGameModeBase::BeginPlay()
{
	GetWorld()->GetTimerManager().SetTimer(TimeHandle_SpawnCandy, this, &ASnakesGameModeBase::SpawnCandy, SpawnCandyInterval, true);
}

void ASnakesGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.0f, FColor::Green, FString(TEXT("PostLogin(Begin)")), false);
	}

	Super::PostLogin(NewPlayer);
	
	APawn* NewPawn = NewPlayer->GetPawn();
	if (NewPawn && GEngine)
	{
		const UEnum* EnumNetRole = FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetRole"), true);
		FString Message = FString::Printf(TEXT("PostLogin(End) %s %s"),
			*NewPawn->GetName(), *EnumNetRole->GetEnumName((int32)NewPawn->Role));
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.0f, FColor::Green, Message, false);
	}
}

FBox ASnakesGameModeBase::GetWorldBox() const
{
	return WorldBox;
}

void ASnakesGameModeBase::SpawnCandy()
{
	if (!GetWorld() || !CandyActorClass)
	{
		return;
	}

	FVector NewLocation = FMath::RandPointInBox(SpawnCandyBox);
	GetWorld()->SpawnActor<ACandyActor>(CandyActorClass, NewLocation, FRotator::ZeroRotator);
}
