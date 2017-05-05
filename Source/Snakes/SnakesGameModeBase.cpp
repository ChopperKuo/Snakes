// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakesGameModeBase.h"

void ASnakesGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	APawn* NewPawn = NewPlayer->GetPawn();
	if (NewPawn != nullptr)
	{
		FVector NewLocation(FMath::RandRange(-500.0f, 500.0f), FMath::RandRange(-500.0f, 500.0f), 100.0f);
		NewPawn->SetActorLocation(NewLocation);
	}

	if (GEngine != nullptr)
	{
		FString Message(NewPlayer->GetFullName() + TEXT(" PostLogin"));
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0f, FColor::Yellow, Message);
	}
}
