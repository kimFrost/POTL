// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "UObjects/Allocations/UAllocationSlot.h"
#include "UAllocatable.h"




UAllocatable::UAllocatable()
{
	Owner = nullptr;
}


UAllocatable::~UAllocatable()
{
	
}

void UAllocatable::Allocate(UAllocationSlot* AllocationSlot)
{
	if (AllocationSlot)
	{
		AllocationSlot->Allocate(this);
	}
}

void UAllocatable::Unallocate()
{
	OnUnallocatedDelegate.Broadcast(this);
}

