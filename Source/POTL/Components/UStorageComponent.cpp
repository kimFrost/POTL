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
	PrimaryComponentTick.bCanEverTick = false;

	StorageCapacity = 0;
	IncludeInStorageMap = true;
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
		StoredResources.Add(Resource);
		Stored = true;
		OnStorageUpdated(Resource);
		StoredResourceCompleteList.Add(Resource);

		/*
		if (StoredResourcesMap.Contains(Resource->ResourceId))
		{
			StoredResourcesMap[Resource->ResourceId].Add(Resource);
		}
		else
		{
			TArray<UResource*> NewStorageList;
			NewStorageList.Add(Resource);
			StoredResourcesMap.Add(Resource->ResourceId, NewStorageList);
		}
		*/

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
		if (Resource->StoredIn == this)
		{
			Resource->StoredIn = nullptr;
		}
		StoredResourceCompleteList.Remove(Resource);
		
		if (StoredResources.Contains(Resource))
		{
			int NumRemoved = StoredResources.Remove(Resource);
			if (NumRemoved > 0)
			{
				OnStorageUpdated(nullptr);
			}
		}

		/*
		if (StoredResourcesMap.Contains(Resource->ResourceId))
		{
			TArray<UResource*>& EstimatedList = StoredResourcesMap[Resource->ResourceId];
			int NumRemoved = EstimatedList.Remove(Resource);
		}
		StoredResourcesMap.Compact();
		*/
		
	}
}


/******************** RequestResource *************************/
UResource* UStorageComponent::RequestResource(APOTLStructure* Requester, FString ResourceId)
{
	if (Requester)
	{
		// Clean StoredResources tmap ?

		for (int i = 0; i < StoredResources.Num(); i++)
		{
			UResource* Resource = StoredResources[i];
			if (Resource && Resource->ResourceId == ResourceId)
			{
				return Resource;
			}
		}

		/*
		for (auto& List : StoredResourcesMap)
		{
			if (List.Key == ResourceId)
			{
				for (int i = 0; i < List.Value.Num(); i++)
				{
					UResource* Resource = List.Value[i];
					if (Resource)
					{
						return Resource;
					}
				}
			}
		}
		*/

		/*
		if (StoredResourcesMap.Contains(ResourceId))
		{
			TArray<UResource*>& ResourceList = StoredResourcesMap[ResourceId];
			for (int i = 0; i < ResourceList.Num(); i++)
			{
				UResource* Resource = ResourceList[i];
				if (Resource)
				{
					//Return resource
				}
			}
		}
		*/
	}

	return nullptr; // Return nullptr since no other return has happenend
}


/******************** RequestResourceByTag *************************/
UResource* UStorageComponent::RequestResourceByTag(APOTLStructure* Requester, FString Tag)
{
	if (Requester)
	{
		for (int i = 0; i < StoredResources.Num(); i++)
		{
			UResource* Resource = StoredResources[i];
			if (Resource && Resource->Tags.Contains(Tag))
			{
				return Resource;
			}
		}
	}
	return nullptr; // Return nullptr since no other return has happenend
}


/******************** OnStorageUpdated *************************/
void UStorageComponent::OnStorageUpdated_Implementation(UResource* Resource)
{
	OnStorageUpdate.Broadcast(this, Resource);

}


// Called by parent structure when ready
void UStorageComponent::Init()
{
	Super::Init();

	// Add self to UStorageMap in GameInstance
	if (IncludeInStorageMap)
	{
		UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
		if (GameInstance)
		{
			GameInstance->IncludeStorage(this);
		}
	}
}


// Called when the game starts
void UStorageComponent::BeginPlay()
{
	Super::BeginPlay();
	
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

