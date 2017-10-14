// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
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

	}
}

void UAllocationSlot::Unallocate()
{
	if (Allocated)
	{

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
		OnRequestAllocatable.Broadcast();
	}
}