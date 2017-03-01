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
	if (IsValid(StorageComp))
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


UResource* UStorageMap::RequestResource(APOTLStructure* Requester, FString ResourceId)
{
	//TODO: Sort storages by distance to requester

	//TODO: Use QuantityMapOfType instead

	for (auto& Entry : Storages)
	{
		FVector WorldLocation = Entry.Key;
		UStorageComponent* StorageComp = Entry.Value;
		if (IsValid(StorageComp))
		{
			UResource* Resource = StorageComp->RequestResource(Requester, ResourceId);
			if (Resource)
			{
				return Resource;
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
