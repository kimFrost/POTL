// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLGameMode.h"
#include "UStorageComponent.h"



// Sets default values for this component's properties
UStorageComponent::UStorageComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	/*
	StorageCapacity = 0.f;
	PowerUpkeep = 0.f;
	PowerGenerated = 0.f;

	StoredPower = 0.f;
	SurplusPower = 0.f;
	*/
}


/*
float UProductionComponent::GeneratePower()
{
	return PowerGenerated;

}


// Called when the game starts
void UProductionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(Module))
	{
		AGameModeBattle* GameMode = Cast<AGameModeBattle>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{

		}
	}
}


// Called every frame
void UProductionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
*/
