// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "UObjects/UResource.h"
#include "UObjects/UHexTile.h"
#include "UObjects/UPerson.h"
#include "UObjects/Allocations/UAllocationSlot.h"
#include "UObjects/Allocations/UAllocationSetSlot.h"
#include "UObjects/Allocations/UPersonSlot.h"
#include "UObjects/Allocations/UHexSlot.h"
#include "POTLGameMode.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "POTLUtilFunctionLibrary.h"
#include "Components/UProviderComponent.h"
#include "UFactoryComponent.h"



// Sets default values for this component's properties
UFactoryComponent::UFactoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}
UAllocatable* UFactoryComponent::RequestAllocatable(UClass* AllocatableClass, FString AllocatableID)
{
	if (ParentStructure)
	{
		return ParentStructure->RequestAllocatable(AllocatableClass, AllocatableID);
	}
	return nullptr;
}
void UFactoryComponent::UpdateMaxSlotSets(UAllocationSlot* AllocationSlot, UAllocatable* Allocatable)
{
	int32 MaxNumberOfSets = 0;
	for (auto& Slot : AllocatedPersonSlots)
	{
		if (Slot && Slot->Allocated)
		{
			UPerson* Person = Cast<UPerson>(Slot->Allocated);
			if (Person)
			{
				MaxNumberOfSets += 3;
			}
		}
	}

	int32 DiffCount = MaxNumberOfSets - AllocatedSetSlots.Num();
	if (DiffCount > 0)
	{
		// Add Set slots
		for (int32 i = 0; i < DiffCount; i++)
		{
			UAllocationSetSlot* SetSlot = NewObject<UAllocationSetSlot>(this);
			if (SetSlot)
			{
				// IncludeSet
				// ExcludeSet

				//PersonSlot->OnRequestAllocatable.BindUObject(this, &UFactoryComponent::RequestAllocatable);

				/*
				HexSlot->OnAllocationDelegate.AddDynamic(this, &UGatherComponent::IncludeHex);
				HexSlot->OnUnallocationDelegate.AddDynamic(this, &UGatherComponent::ExcludeHex);
				*/
				AllocatedSetSlots.Add(SetSlot);
			}
		}
	}
	else if (DiffCount < 0)
	{
		// Remove Set slots
		for (int32 i = DiffCount; i < 0; i++)
		{
			UAllocationSetSlot* LastSetSlot = AllocatedSetSlots[AllocatedSetSlots.Num() - 1];
			if (LastSetSlot)
			{
				/*
				LastHexSlot->Unallocate();
				LastHexSlot->OnAllocationDelegate.RemoveDynamic(this, &UGatherComponent::IncludeHex);
				LastHexSlot->OnUnallocationDelegate.RemoveDynamic(this, &UGatherComponent::ExcludeHex);
				AllocatedSetSlots.RemoveAt(AllocatedSetSlots.Num() - 1);
				*/
			}
		}
	}

	// Person to 3 x UAllocationSetSlot

	// Sort. Put free slots last
	AllocatedPersonSlots.Sort([this](const UPersonSlot& PersonSlot1, const UPersonSlot& PersonSlot2) {
		return (PersonSlot1.Allocated != nullptr);
	});
	/*
	AllocatedSetSlots.Sort([this](const UAllocationSetSlot& SetSlot1, const UAllocationSetSlot& SetSlot2) {
		return (HexSlot1.Allocated != nullptr);
	});
	*/
}
void UFactoryComponent::ProcessBaseData()
{
	for (auto& Entry : BaseData.AllocationSlots)
	{
		for (int32 i = 0; i < Entry.Value; i++)
		{
			UPersonSlot* PersonSlot = NewObject<UPersonSlot>(this);
			//UAllocationSlot* PersonSlot = NewObject<UAllocationSlot>(this);
			if (PersonSlot)
			{
				PersonSlot->AllowedAllocationClass = UPerson::StaticClass();
				PersonSlot->AllowedAllocationID = TEXT("Laborer");
				//~~ Bind RequestAllocatable on slot to RequestAllocatable for processing ~~//
				PersonSlot->OnRequestAllocatable.BindUObject(this, &UFactoryComponent::RequestAllocatable);
				//~~ Listen for allocation state change ~~//
				PersonSlot->OnAllocationChangeDelegate.AddDynamic(this, &UFactoryComponent::UpdateMaxSlotSets);
				AllocatedPersonSlots.Add(PersonSlot);
			}
		}
	}
}
void UFactoryComponent::Init()
{
	Super::Init();

	if (!ParentStructure)
	{
		bIsOn = false;
	}

	ProcessBaseData();
}
void UFactoryComponent::BeginPlay()
{
	Super::BeginPlay();

}
