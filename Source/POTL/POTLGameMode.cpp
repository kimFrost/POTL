// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLGameMode.h"

// Sets default values
APOTLGameMode::APOTLGameMode(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
	Time = 0.f;
	PlayRate = 1.f;
}



/******************** ProgressTime *************************/
void APOTLGameMode::ProgressTime(float Amount)
{
	Time = Time + Amount;
	/*
	TurnTimeLeft = TurnTimeLeft - Amount;
	if (TurnTimeLeft <= 0)
	{
		ETurnStep NewStep = ProgressTurnStep();

		switch (NewStep)
		{
		case ETurnStep::Planning:
			NewTurn();
			break;
		case ETurnStep::Moving:
			TurnTimeLeft = 1.f;
			SetPlayRate(1.f, 1.f);
			break;
		case ETurnStep::Shooting:
			TurnTimeLeft = 1.f;
			break;
		case ETurnStep::SpawningModules:
			TurnTimeLeft = 1.f;
			UpdateOrders(EOrderType::SpawnModule);
			break;
		case ETurnStep::Maneuvering:
			TurnTimeLeft = 2.f;
			break;
		}

		OnStepChange.Broadcast(NewStep);
	}
	*/
	OnTimeUpdated.Broadcast(Time, Amount);
}


/******************** BeginPlay *************************/
void APOTLGameMode::BeginPlay()
{
	Super::BeginPlay();

	//UGameplayStatics::CreatePlayer(GetWorld());

	//ConstructGrid();

	//FTimerHandle UnusedHandle;
	//GetWorldTimerManager().SetTimer(UnusedHandle, this, &AGameModeBattle::NewTurn, 0.5f, false);
}


/******************** Tick *************************/
void APOTLGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayRate > 0.0001)
	{
		ProgressTime(DeltaTime * PlayRate);
	}
}