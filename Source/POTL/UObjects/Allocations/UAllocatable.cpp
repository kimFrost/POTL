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

void UAllocatable::AllocateTo(UAllocationSlot* AllocationSlot)
{
	if (AllocationSlot)
	{
		//~~ All logic is happening in AllocationSlot ~~//
		AllocationSlot->Allocate(this);
		//OnAllocatedDelegate.Broadcast(this);
		/*
		if (AllocatedTo == AllocationSlot)
		{
		}
		*/
	}
}

void UAllocatable::Unallocate()
{
	if (AllocatedTo)
	{
		//~~ All logic is happening in AllocationSlot ~~//
		AllocatedTo->Unallocate(this);
		//OnUnallocatedDelegate.Broadcast(this);
	}
}

void UAllocatable::OnAllocated_Implementation(UAllocatable* Allocatable)
{}
void UAllocatable::OnUnallocated_Implementation(UAllocatable* Allocatable)
{}