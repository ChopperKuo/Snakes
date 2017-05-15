// Fill out your copyright notice in the Description page of Project Settings.

#include "Snakes.h"
#include "SnakePawn.h"
#include "SnakePawnMovementComponent.h"
#include "SnakeBody.h"
#include "SnakeBodyMovementComponent.h"
#include "SnakesGameModeBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

FName ASnakePawn::SnakeTagName(TEXT("Snake"));

ASnakePawn::ASnakePawn()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	// 關閉自動位置同步，SnakePawnMovementComponent負責Client的移動。
	bReplicateMovement = false;
	Tags.AddUnique(SnakeTagName);

	// 初始化UBoxComponent。
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->bGenerateOverlapEvents = true;
	CollisionComponent->InitBoxExtent(FVector(50.0f));
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASnakePawn::OnBeginOverlap);

	// 指定RootComponent為UBoxComponent。
	RootComponent = CollisionComponent;

	// 初始化USnakePawnMovementComponent。
	SnakePawnMovementComponent = CreateDefaultSubobject<USnakePawnMovementComponent>(TEXT("SnakePawnMovementComponent"));
	SnakePawnMovementComponent->UpdatedComponent = RootComponent;

	InitialBodyNum = 2;
	SnakeBodyClass = ASnakeBody::StaticClass();
}

void ASnakePawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASnakePawn, BodyNum);
	DOREPLIFETIME(ASnakePawn, bIsDead);
}

void ASnakePawn::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// Server Logic，初始隨機位置和移動方向。
		Reborn();
	}
}

UPawnMovementComponent* ASnakePawn::GetMovementComponent() const
{
	return SnakePawnMovementComponent;
}

void ASnakePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 註冊WASD控制事件。
	InputComponent->BindAxis(TEXT("MoveForward"), this, &ASnakePawn::MoveForward);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &ASnakePawn::MoveRight);
}

int32 ASnakePawn::GetBodyNum() const
{
	return BodyNum;
}

void ASnakePawn::SetBodyNum(int32 NewNum)
{
	if (BodyNum != NewNum)
	{
		BodyNum = NewNum;
		if (Role == ROLE_Authority || Role == ROLE_None)
		{
			OnRep_BodyNum();
		}
	}
}

bool ASnakePawn::GetIsDead() const
{
	return bIsDead;
}

void ASnakePawn::SetIsDead(bool bNewIsDead)
{
	if (bIsDead != bNewIsDead)
	{
		// 透過Variable Replication同步給Client。
		bIsDead = bNewIsDead;

		// Server端必須手動呼叫。
		if (Role == ROLE_Authority || Role == ROLE_None)
		{
			OnRep_bIsDead();
		}
	}
}

void ASnakePawn::OnRep_BodyNum()
{
	if (BodyNum > BodyNodes.Num())
	{
		if (SnakeBodyClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;

			AActor* FollowTarget = BodyNodes.Num() == 0 ? this : Cast<AActor>(BodyNodes.Last());

			int32 Count = BodyNum - BodyNodes.Num();
			for (int Index = 0; Index < Count; ++Index)
			{
				ASnakeBody* SnakeBody = GetWorld()->SpawnActor<ASnakeBody>(SnakeBodyClass, GetActorLocation(), GetActorRotation(), SpawnParams);
				if (SnakeBody)
				{
					//SnakeBody->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
					SnakeBody->GetMovementComponent()->SetFollowActor(FollowTarget);

					BodyNodes.Add(SnakeBody);

					FollowTarget = SnakeBody;
				}
			}
		}
	}
	else if (BodyNum < BodyNodes.Num())
	{
		for (int Index = BodyNum; Index < BodyNodes.Num(); ++Index)
		{
			BodyNodes[Index]->Destroy();
		}
		BodyNodes.RemoveAt(BodyNum, BodyNodes.Num() - BodyNum);
	}
}

void ASnakePawn::OnRep_bIsDead()
{
	CollisionComponent->bGenerateOverlapEvents = !bIsDead;
	if (SnakePawnMovementComponent)
	{
		SnakePawnMovementComponent->SetComponentTickEnabled(!bIsDead);
	}

	if (bIsDead)
	{
		for (auto* SnakeBody : BodyNodes)
		{
			SnakeBody->Die();
		}
	}
	else
	{
		for (auto* SnakeBody : BodyNodes)
		{
			SnakeBody->Reborn();
		}
	}

	ReceivebIsDead();
}

void ASnakePawn::MoveForward(float AxisValue)
{
	// 將訊號轉給USnakePawnMovementComponent。
	AddMovementInput(FVector::ForwardVector, AxisValue);
}

void ASnakePawn::MoveRight(float AxisValue)
{
	// 將訊號轉給USnakePawnMovementComponent。
	AddMovementInput(FVector::RightVector, AxisValue);
}

void ASnakePawn::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Server Logic
	if (HasAuthority())
	{
		if (OtherActor->ActorHasTag(SnakeTagName))
		{
			// 碰撞到自己身體不會死亡。
			if (OtherActor->GetOwner() == this)
			{
				return;
			}
		}
		else if (OtherActor->ActorHasTag(TEXT("Candy")))
		{
			HandleEatCandy();
			return;
		}

		HandleDeathAndReborn();
	}
}

void ASnakePawn::HandleEatCandy()
{
	if (PlayerState)
	{
		PlayerState->Score += 10.0f;

		int32 NewBodyNum = InitialBodyNum + FMath::FloorToInt(PlayerState->Score / 10.0f);
		SetBodyNum(NewBodyNum);

		FString Message = FString::Printf(TEXT("Score = %f, BodyNum = %d"), PlayerState->Score, NewBodyNum);
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.0f, FColor::Green, Message, false);
	}
}

void ASnakePawn::HandleDeathAndReborn()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.0f, FColor::Green, FString(TEXT("HandleDeathAndReborn")), false);
	SetIsDead(true);

	// 在隨機位置和隨機方向重生。
	GetWorld()->GetTimerManager().SetTimer(TimeHandle_DeferredReborn, this, &ASnakePawn::Reborn, 2.0f);
}

void ASnakePawn::Reborn()
{
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(GetWorld());
	if (!GameModeBase)
	{
		return;
	}

	ASnakesGameModeBase* SnakeGameMode = Cast<ASnakesGameModeBase>(GameModeBase);
	if (!SnakeGameMode)
	{
		return;
	}

	// 把Actor搬到隨機位置和隨機方向。
	FBox WorldBox = SnakeGameMode->GetWorldBox();
	FVector NewLocation = FMath::RandPointInBox(WorldBox);
	FRotator NewRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f);
	SetActorLocationAndRotation(NewLocation, NewRotator);

	// 分數歸零。
	if (PlayerState)
	{
		PlayerState->Score = 0.0f;
	}

	// 身體長度初始化。
	SetBodyNum(InitialBodyNum);

	// 設定為復活狀態。
	SetIsDead(false);
}
