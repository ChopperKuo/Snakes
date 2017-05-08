// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakesGameModeBase.h"

FBox ASnakesGameModeBase::GetWorldBox() const
{
	return WorldBox;
}
