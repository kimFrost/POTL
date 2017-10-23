// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "UAllocatable.h"




UAllocatable::UAllocatable()
{
	
}


UAllocatable::~UAllocatable()
{
	
}

void UAllocatable::Unallocate()
{
	OnUnallocatedDelegate.Broadcast(this);
}
