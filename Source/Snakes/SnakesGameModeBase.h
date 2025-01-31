// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "SnakesGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SNAKES_API ASnakesGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void PostLogin(APlayerController* NewPlayer) override;

public:
	UFUNCTION(Category = Gameplay, BlueprintCallable)
	FORCEINLINE FBox GetWorldBox() const;

protected:
	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadWrite)
	FBox WorldBox;
};
