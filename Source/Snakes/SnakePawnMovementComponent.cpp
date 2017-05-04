// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakePawnMovementComponent.h"

void USnakePawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (PawnOwner == nullptr || UpdatedComponent == nullptr || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	MoveUpdatedComponent(ConsumeInputVector(), UpdatedComponent->GetComponentRotation(), false);
	UpdateComponentVelocity();
}
