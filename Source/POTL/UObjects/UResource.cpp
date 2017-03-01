// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "Components/UStorageComponent.h"
#include "UResource.h"




UResource::UResource()
{
	StoredIn = nullptr;
}


UResource::~UResource()
{

}

void UResource::Consume()
{
	// Remove from storage in StoredIn
	if (StoredIn)
	{
		StoredIn->RemoveResourceFromStorage(this);
	}
	StoredIn = nullptr;
	// Destroy self
}