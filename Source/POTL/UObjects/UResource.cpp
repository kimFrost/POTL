// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "Components/UStorageComponent.h"
#include "POTLGameInstance.h"
#include "POTLUtilFunctionLibrary.h"
#include "UResource.h"




UResource::UResource()
{
	StoredIn = nullptr;
	Value = 0.f;
}


UResource::~UResource()
{
	
}

APOTLStructure * UResource::GetOwner()
{
	if (StoredIn)
	{
		return StoredIn->ParentStructure;
	}
	return nullptr;
}

void UResource::Consume()
{
	// Get all with reference to this. For debugging
	TArray<UObject*> ReferredToObjs;
	UPOTLUtilFunctionLibrary::GetObjReferenceCount(this, &ReferredToObjs);
	for (UObject* Each : ReferredToObjs)
	{
		if (Each)
		{
			//UE_LOG(YourLog, Warning, TEXT("%s"), *Each->GetName());
		}
	}

	// Remove from storage in StoredIn
	if (StoredIn)
	{
		StoredIn->RemoveResourceFromStorage(this);
	}
	StoredIn = nullptr;
	// Destroy self
	this->ConditionalBeginDestroy(); //instantly clears UObject out of memory
}


bool UResource::Transfer(UStorageComponent* Storage)
{
	if (Storage)
	{
		if (StoredIn)
		{
			StoredIn->RemoveResourceFromStorage(this);
		}
		return Storage->StoreResource(this);
	}
	return false;
}


void UResource::Init()
{
	// Get resource data
	UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
	if (GameInstance)
	{
		//GameInstance->DATA_Resources->
	}

	// Create resource throught game instance instead
}
