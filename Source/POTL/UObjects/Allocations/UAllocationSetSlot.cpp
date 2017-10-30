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

			//Slot->OnAllocatedChange.AddDynamic(this, &UAllocationSetSlot::Update);

			//!! On switch resources in a slot, might trigger some unwanted behaviour. Since it will trigger unallocate delegate.

			AllocationSlots.Add(Slot);
		}
	}
}
void UAllocationSetSlot::Toggle()
{
	if (bHasAllocation)
	{
		Unallocate();
	}
	else
	{
		RequestAllocations();
	}
}
void UAllocationSetSlot::Allocate(TArray<UAllocatable*> AllocatableSet)
{
	if (!bHasAllocation)
	{
		for (auto& Allocatable : AllocatableSet)
		{
			if (Allocatable)
			{
				for (auto& Slot : AllocationSlots)
				{
					if (Slot && !Slot->Allocated)
					{
						Slot->Allocate(Allocatable);
					}
				}
			}
		}
		bHasAllocation = true;
		for (auto& Slot : AllocationSlots)
		{
			if (Slot && !Slot->Allocated)
			{
				bHasAllocation = false;
				break;
			}
		}
	}
}
void UAllocationSetSlot::Unallocate()
{
	if (bHasAllocation)
	{
		for (auto& Slot : AllocationSlots)
		{
			if (Slot)
			{
				Slot->Unallocate();
			}
		}
		bHasAllocation = false;
	}
}
void UAllocationSetSlot::RequestAllocations()
{
	// Request as a package, that can come from multiple sources. If set is not complete, then don't allocate any of them.

	// Multiple requests or single set request??

	//TMap<UClass*, FString>
	if (OnRequestAllocatableSet.IsBound())
	{
		TArray<UAllocatable*> AllocatableSet = OnRequestAllocatableSet.Execute(SlotEntries);
		Allocate(AllocatableSet);
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