// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
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
		Storages.Add(FVector(0.f, 0.f, 0.f), StorageComp);
		StorageComp->OnStorageUpdate.AddDynamic(this, &UStorageMap::StorageMapUpdated);
	}
}

/******************** StorageMapUpdated *************************/
void UStorageMap::StorageMapUpdated_Implementation()
{
	OnStorageMapUpdated.Broadcast();

}
