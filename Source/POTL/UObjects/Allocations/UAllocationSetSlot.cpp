// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "UObjects/Allocations/UAllocatable.h"
#include "UObjects/Allocations/UAllocationSlot.h"
#include "UAllocationSetSlot.h"




UAllocationSetSlot::UAllocationSetSlot()
{}
UAllocationSetSlot::~UAllocationSetSlot()
{}
void UAllocationSetSlot::AllocatableUnallocateToSlot(UAllocationSlot* AllocationSlot, UAllocatable* Allocatable)
{
	// Unallocate all slots, since it need to work as a set

	//!! Watch out for inifite loops !!//

	for (auto& Slot : AllocationSlots)
	{
		if (Slot)
		{
			Slot->OnAllocationDelegate.RemoveDynamic(this, &UAllocationSetSlot::AllocatableUnallocateToSlot);
			Slot->OnUnallocationDelegate.RemoveDynamic(this, &UAllocationSetSlot::AllocatableAllocateToSlot);
			Slot->Unallocate();
		}
	}
}
void UAllocationSetSlot::AllocatableAllocateToSlot(UAllocationSlot* AllocationSlot, UAllocatable* Allocatable)
{
	//Validate if all slots are allocated
	for (auto& Slot : AllocationSlots)
	{
		if (Slot)
		{
			if (!Slot->Allocated)
			{
				return;
			}
		}
	}
	//OnAllocated.Broadcast(this);
}
void UAllocationSetSlot::Construct(TMap<UClass*, FString> _SlotEntries)
{
	SlotEntries = _SlotEntries;
	for (auto& Entry : SlotEntries)
	{
		UAllocationSlot* Slot = NewObject<UAllocationSlot>(this);
		if (Slot)
		{
			Slot->AllowedAllocationClass = Entry.Key;
			Slot->AllowedAllocationID = Entry.Value;

			Slot->OnUnallocationDelegate.AddDynamic(this, &UAllocationSetSlot::AllocatableUnallocateToSlot);
			Slot->OnAllocationChangeDelegate.AddDynamic(this, &UAllocationSetSlot::AllocatableAllocateToSlot);

			//!! Add binding on allocate instead !!//

			//Slot->OnAllocatedChange.AddDynamic(this, &UAllocationSetSlot::Update);

			//!! On switch resources in a slot, might trigger some unwanted behaviour. Since it will trigger unallocate delegate.

			AllocationSlots.Add(Slot);
		}
	}
}
void UAllocationSetSlot::RequestAllocations()
{
	// Request as a package, that can come from multiple sources. If set is not complete, then don't allocate any of them.

	for (auto& Slot : AllocationSlots)
	{
		if (Slot && !Slot->Allocated)
		{

		}
	}


	/*
	if (!Allocated)
	{
		if (OnRequestAllocatable.IsBound())
		{
			UAllocatable* Allocatable = OnRequestAllocatable.Execute(AllowedAllocationClass, AllowedAllocationID);
			if (Allocatable)
			{
				Allocate(Allocatable);
			}
		}
	}
	*/
}