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

	/*
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
	*/

	// Call resource map update // Broadcast storage update
	
	return LeftOvers;
}

/******************** StoreResource *************************/
bool UStorageComponent::StoreResource(UResource* Resource)
{
	bool Stored = false;
	if (Resource)
	{
		// Check for allowed storage type
		if (AllowedResources.Num() > 0)
		{
			if (!AllowedResources.Contains(Resource->ResourceId))
			{
				return false;
			}
		}
		// Check for room in storage
		if (StorageCapacity > 0)
		{
			if (StoredResourceCompleteList.Num() == StorageCapacity)
			{
				return false;
			}
		}
		// Store resource
		if (StoredResources.Contains(Resource->ResourceId))
		{
			StoredResources[Resource->ResourceId].Add(Resource);
			StoredResourceCompleteList.Add(Resource);
			Stored = true;
			StorageUpdate(Resource);
		}
		else
		{
			TArray<UResource*> NewStorageList;
			NewStorageList.Add(Resource);
			StoredResources.Add(Resource->ResourceId, NewStorageList);
			StoredResourceCompleteList.Add(Resource);
			Stored = true;
			StorageUpdate(Resource);
		}
		if (Stored)
		{
			Resource->StoredIn = this;
		}
	}
	return Stored;
}


/******************** RemoveResourceFromStorage *************************/
void UStorageComponent::RemoveResourceFromStorage(UResource* Resource)
{
	if (Resource)
	{
		StoredResourceCompleteList.Remove(Resource);
		if (StoredResources.Contains(Resource->ResourceId))
		{
			TArray<UResource*>& EstimatedList = StoredResources[Resource->ResourceId];
			int NumRemoved = EstimatedList.Remove(Resource);
			if (NumRemoved > 0)
			{
				StorageUpdate(nullptr);
			}
		}
	}
}


/******************** RequestResource *************************/
UResource* UStorageComponent::RequestResource(APOTLStructure* Requester, FString ResourceId)
{
	if (Requester)
	{
		if (StoredResources.Contains(ResourceId))
		{
			for (auto& Resource : StoredResources[ResourceId])
			{
				if (IsValid(Resource))
				{
					//Requester->AddResource(ResourceId, Quantity);
					return Resource;
				}
			}
		}
	}
	return nullptr; // Return nullptr since no other return has happenend
}



/******************** StorageUpdate *************************/
void UStorageComponent::StorageUpdate_Implementation(UResource* Resource)
{
	OnStorageUpdate.Broadcast(this, Resource);

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

