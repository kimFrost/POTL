// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "UObjects/Allocations/UAllocatable.h"
#include "UAllocationSlot.h"




UAllocationSlot::UAllocationSlot()
{
	AllowedAllocationClass = UAllocatable::StaticClass();
}


UAllocationSlot::~UAllocationSlot()
{
	
}

void UAllocationSlot::Allocate(UAllocatable* Allocatable)
{
	if (Allocatable)
	{
		Allocated = Allocatable;

		Allocated->OnUnallocatedDelegate.AddDynamic(this, &UAllocationSlot::Unallocate);

		// TODO
		//Allocated->AllocatedTo = 

		OnAllocatedDelegate.Broadcast(this, Allocated);
		OnAllocatedChange.Broadcast(this, Allocated);
	}
}

void UAllocationSlot::Unallocate(UAllocatable* Allocatable)
{
	if (Allocated)
	{
		Allocated->OnUnallocatedDelegate.RemoveDynamic(this, &UAllocationSlot::Unallocate);
		PrevAllocated = Allocated;
		Allocated = nullptr;

		// TODO
		//Allocated->AllocatedTo = 

		OnUnallocatedDelegate.Broadcast(this, PrevAllocated);
		OnAllocatedChange.Broadcast(this, PrevAllocated);
	}
}

void UAllocationSlot::Toggle()
{
	if (Allocated)
	{
		Unallocate();
	}
	else
	{
		RequestAllocation();
	}
}

void UAllocationSlot::RequestAllocation()
{
	if (!Allocated)
	{
		if (OnRequestAllocatable.IsBound())
		{
			//UAllocatable* Allocatable = OnRequestAllocatable.Execute(this);
			UAllocatable* Allocatable = OnRequestAllocatable.Execute(AllowedAllocationClass, AllowedAllocationID);
			if (Allocatable)
			{
				Allocate(Allocatable);
			}
			/*
			EHandleType Response = Delegate.Value->Execute(Hex);
			if (Response == EHandleType::HandledBreak)
			{
				return false;
			}
			*/
		}
		//OnRequestAllocatable.Broadcast();
	}
}