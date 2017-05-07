// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakesGameModeBase.h"

void ASnakesGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	APawn* NewPawn = NewPlayer->GetPawn();
	if (NewPawn != nullptr)
	{
		FVector NewLocation(-500.0f, -500.0f + 200.0f * GetNumPlayers(), 100.0f);
		NewPawn->SetActorLocation(NewLocation);
	}
}
