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

void UStorageMap::UpdateMap()
{

}

void UStorageMap::IncludeStorage(UStorageComponent* StorageComp)
{
	if (IsValid(StorageComp))
	{
		StorageComp->OnStorageUpdate.AddDynamic(this, &UStorageMap::UpdateMap);
	}
}

