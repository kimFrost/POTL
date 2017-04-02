// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "UObjects/UResource.h"
#include "Components/UStorageComponent.h"
#include "UStorageMap.h"




UStorageMap::UStorageMap()
{
	
}


UStorageMap::~UStorageMap()
{

}


void UStorageMap::IncludeStorage(UStorageComponent* StorageComp)
{
	if (StorageComp)
	{
		if (StorageComp->ParentStructure)
		{
			FVector WorldLocation = StorageComp->ParentStructure->GetActorLocation();
			if (Storages.Contains(WorldLocation))
			{
				Storages[WorldLocation] = StorageComp;
			}
			else
			{
				Storages.Add(WorldLocation, StorageComp);
			}
			//StorageComp->OnStorageUpdate.Clear();
			StorageComp->OnStorageUpdate.RemoveDynamic(this, &UStorageMap::StorageMapUpdated);
			StorageComp->OnStorageUpdate.AddDynamic(this, &UStorageMap::StorageMapUpdated);
			//StorageComp->OnComponentDestroyed.AddDynamic(this, &UStorageMap::);
			//StorageComp->OnComponentDeactivated.AddDynamic(this, &UStorageMap::);
			//StorageComp->OnComponentActivated.AddDynamic(this, &UStorageMap::);
			StorageMapUpdated(nullptr, nullptr);
		}
	}
}


TArray<UResource*> UStorageMap::GetResourceList()
{
	TArray<UResource*> ResourceList = TArray<UResource*>();
	for (auto& Entry : Storages)
	{
		if (Entry.Value)
		{
			UStorageComponent* StorageComp = Entry.Value;
			ResourceList.Append(StorageComp->StoredResources);
		}
	}
	return ResourceList;
}

/******************** RequestResource *************************/
UResource* UStorageMap::RequestResource(APOTLStructure* Requester, FString ResourceId)
{
	//TODO: Sort storages by distance to requester

	//TODO: Use QuantityMapOfType instead
	
	if (Requester)
	{
		for (auto& Entry : Storages)
		{
			if (Entry.Value)
			{
				if (Entry.Value->IsValidLowLevel())
				{
					FVector WorldLocation = Entry.Key;
					UStorageComponent* StorageComp = Entry.Value;
					if (StorageComp && StorageComp->bAllowFlowOut)
					{
						UResource* Resource = StorageComp->RequestResource(Requester, ResourceId);
						if (Resource)
						{
							return Resource;
						}
					}
				}
				else
				{
					// Trigger Warning
				}
			}
		}
	}
	return nullptr;
}

/******************** RequestResourceByTag *************************/
UResource* UStorageMap::RequestResourceByTag(APOTLStructure * Requester, FString Tag)
{
	if (Requester)
	{
		for (auto& Entry : Storages)
		{
			if (Entry.Value)
			{
				if (Entry.Value->IsValidLowLevel())
				{
					FVector WorldLocation = Entry.Key;
					UStorageComponent* StorageComp = Entry.Value;
					if (StorageComp)
					{
						UResource* Resource = StorageComp->RequestResourceByTag(Requester, Tag);
						if (Resource)
						{
							return Resource;
						}
					}
				}
				else
				{
					// Trigger Warning
				}
			}
		}
	}
	return nullptr;
}

/******************** HasResourceAvailable *************************/
bool UStorageMap::HasResourceAvailable(TArray<FString> ResourceIds)
{
	TArray<UResource*> ResourceList = GetResourceList();
	for (auto& Resource : ResourceList)
	{
		if (Resource)
		{
			if (ResourceIds.Contains(Resource->ResourceId))
			{
				ResourceIds.RemoveSingle(Resource->ResourceId);
				if (ResourceIds.Num() == 0)
				{
					return true;
				}
			}
		}
	}
	return (ResourceIds.Num() <= 0);
}

/******************** RequestResources *************************/
TArray<UResource*> UStorageMap::RequestResources(APOTLStructure* Requester, TArray<FString> ResourceIds)
{
	TArray<UResource*> RequestedResources;
	if (Requester)
	{
		// Check if total request can be met
		if (HasResourceAvailable(ResourceIds))
		{
			// Get resoruces
			for (auto& ResourceId : ResourceIds)
			{
				UResource* Resource = RequestResource(Requester, ResourceId);
				if (Resource)
				{
					RequestedResources.Add(Resource);
				}
			}
		}
	}

	return RequestedResources;
}

/******************** StorageMapUpdated *************************/
void UStorageMap::StorageMapUpdated_Implementation(UStorageComponent* StorageComp, UResource* Resource)
{
	//TODO: Maintain complete resource list for fast overview
	
	OnStorageMapUpdated.Broadcast(StorageComp, Resource);
}
