// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLGameMode.h"
#include "POTLGameInstance.h"
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
int UStorageComponent::AddResource(FString ResourceId, int Quantity)
{
	int LeftOvers = Quantity;

	if (AllowedResources.Num() > 0)
	{
		if (!AllowedResources.Contains(ResourceId))
		{
			return Quantity;
		}
	}


	// if Enough storage

	// Create resource

	// Add to storage list
	if (StoredResources.Contains(ResourceId))
	{
		StoredResources[ResourceId] += Quantity;
		LeftOvers = 0;
		StorageUpdate();
		//AddedSuccessfully = true;
	}
	else 
	{ 
		StoredResources.Add(ResourceId, Quantity);
		LeftOvers = 0;
		StorageUpdate();
		//AddedSuccessfully = true;
	}

	// Call resource map update // Broadcast storage update
	
	return LeftOvers;
}

/******************** StoreResource *************************/
void UStorageComponent::StoreResource(UResource* Resource)
{

}


/******************** RequestResouce *************************/
bool UStorageComponent::RequestResouce(APOTLStructure* Requester, FString ResourceId, int Quantity)
{
	bool RequestMet = false;
	if (Requester)
	{
		if (StoredResources.Contains(ResourceId))
		{
			if (StoredResources[ResourceId] >= Quantity)
			{
				RequestMet = true;
				StoredResources[ResourceId] -= Quantity;
				Requester->AddResource(ResourceId, Quantity);
			}
		}
	}
	return RequestMet;
}


/******************** StorageUpdate *************************/
void UStorageComponent::StorageUpdate_Implementation()
{
	OnStorageUpdate.Broadcast();

}




// Called when the game starts
void UStorageComponent::BeginPlay()
{
	Super::BeginPlay();

	// Add self to UStorageMap in GameInstance
	UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
	if (GameInstance)
	{
		GameInstance->IncludeStorage(this);
	}
	
}

/*
float UProductionComponent::GeneratePower()
{
	return PowerGenerated;

}




// Called every frame
void UProductionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
*/

