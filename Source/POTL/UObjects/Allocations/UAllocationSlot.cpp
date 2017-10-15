// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "UObjects/Allocations/UAllocatable.h"
#include "UAllocationSlot.h"




UAllocationSlot::UAllocationSlot()
{
	
}


UAllocationSlot::~UAllocationSlot()
{
	
}

void UAllocationSlot::Allocate(UAllocatable* Allocatable)
{
	if (Allocatable)
	{
		Allocated = Allocatable;
		OnAllocatedDelegate.Broadcast(this);
	}
}

void UAllocationSlot::Unallocate()
{
	if (Allocated)
	{
		Allocated = nullptr;
		OnUnallocatedDelegate.Broadcast(this);
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