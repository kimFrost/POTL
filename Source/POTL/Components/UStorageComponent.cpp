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

	StorageCapacity = 0;
}


/******************** AddResource *************************/
bool UStorageComponent::AddResource(FString ResourceId, int Quantity)
{
	bool AddedSuccessfully = false;

	if (AllowedResources.Num() > 0)
	{
		if (!AllowedResources.Contains(ResourceId))
		{
			return false;
		}
	}


	// if Enough storage

	// Create resource

	// Add to storage list
	if (StoredResources.Contains(ResourceId))
	{
		StoredResources[ResourceId] += Quantity;
		AddedSuccessfully = true;
	}
	else 
	{ 
		StoredResources.Add(ResourceId, Quantity);
		AddedSuccessfully = true;
	}

	// Call resource map update // Broadcast storage update
	
	return AddedSuccessfully;
}

/******************** StoreResource *************************/
void UStorageComponent::StoreResource(UResource* Resource)
{

}


/******************** StorageUpdate *************************/
void UStorageComponent::StorageUpdate_Implementation()
{
	OnStorageUpdate.Broadcast();

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

