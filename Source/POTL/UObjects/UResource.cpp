// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "Components/UStorageComponent.h"
#include "POTLGameInstance.h"
#include "POTLUtilFunctionLibrary.h"
#include "UResource.h"




UResource::UResource()
{
	Value = 0.f;
	Locked = false;
}


UResource::~UResource()
{
	
}

void UResource::Consume(EConsumeType ConsumeType, bool bRemoveFromStorage)
{
	Unallocate();

	// Remove from storage in StoredIn
	if (bRemoveFromStorage)
	{
		RemoveFromStorage();
	}

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

	// Destroy self
	this->ConditionalBeginDestroy(); //instantly clears UObject out of memory
}


void UResource::RemoveFromStorage()
{
	/*
	if (StoredIn)
	{
		StoredIn->RemoveResourceFromStorage(this);
	}
	*/
	//StoredIn = nullptr;
}


bool UResource::Transfer(UStorageComponent* Storage)
{
	if (Storage)
	{
		/*
		if (StoredIn)
		{
			StoredIn->RemoveResourceFromStorage(this);
		}
		return Storage->StoreResource(this);
		*/
	}
	return false;
}

bool UResource::AllocateTo(APOTLStructure* Structure)
{
	//!! Handled by UAllocatedable !!//
	/*
	if (Structure)
	{
		Unallocate();

		Structure->AllocateResource(this); 

		AllocatedTo = Structure;
		OnResourceAllocated.Broadcast(this);
		return true;
	}
	*/
	return false;
}

bool UResource::Unallocate()
{
	//!! Handled by UAllocatedable !!//
	/*
	if (AllocatedTo)
	{
		AllocatedTo->UnallocateResource(this);

		AllocatedTo = nullptr;
		OnResourceUnallocated.Broadcast(this);
		return true;
	}
	*/
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
