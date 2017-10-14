// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UAllocationSlot.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class UAllocatable;

//~~~~~ Deleagtes ~~~~~//
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRequestAllocatable);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRequestAllocatable);
//DECLARE_DELEGATE_RetVal(UAllocatable*, FRequestAllocatable);
//DECLARE_DELEGATE_RetVal_OneParam(EHandleType, FHandleDelegate, UObject*);

UCLASS(Blueprintable, BlueprintType)
class POTL_API UAllocationSlot : public UObject
{
	GENERATED_BODY()

public:
	UAllocationSlot();
	~UAllocationSlot();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Allocation")
	UAllocatable* Allocated;

	UAllocatable* PrevAllocated;

	void Allocate(UAllocatable* Allocatable);
	
	void Unallocate();

	void Toggle();

	void RequestAllocation();

	FRequestAllocatable OnRequestAllocatable;

};
