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
			Storages.Add(WorldLocation, StorageComp);
			StorageComp->OnStorageUpdate.AddDynamic(this, &UStorageMap::StorageMapUpdated);
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
					if (StorageComp)
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

/******************** StorageMapUpdated *************************/
void UStorageMap::StorageMapUpdated_Implementation(UStorageComponent* StorageComp, UResource* Resource)
{
	OnStorageMapUpdated.Broadcast(StorageComp, Resource);

}
