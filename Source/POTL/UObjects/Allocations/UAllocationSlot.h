// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UObjects/Allocations/UAllocatable.h"
#include "UAllocationSlot.generated.h"


//~~~~~ Forward Declarations ~~~~~//
//class UAllocationSlot;


//~~~~~ Deleagtes ~~~~~//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUAllocationSlotOnUnallocation, UAllocationSlot*, AllocationSlot, UAllocatable*, Allocatable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUAllocationSlotOnAllocation, UAllocationSlot*, AllocationSlot, UAllocatable*, Allocatable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUAllocationSlotOnChange, UAllocationSlot*, AllocationSlot, UAllocatable*, Allocatable);
//DECLARE_DELEGATE_RetVal(UAllocatable*, FRequestAllocatable);

//DECLARE_DELEGATE_RetVal_TwoParams(UAllocatable*, FRequestAllocatable, TSubclassOf<class UAllocatable>, FString);
DECLARE_DELEGATE_RetVal_TwoParams(UAllocatable*, FRequestAllocatable, UClass*, FString);

UCLASS(Blueprintable, BlueprintType)
class POTL_API UAllocationSlot : public UObject
{
	GENERATED_BODY()

public:
	UAllocationSlot();
	~UAllocationSlot();

	//TSubclassOf<class UAllocatable> AllowedAllocationClass;
	UClass* AllowedAllocationClass;
	FString AllowedAllocationID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APOTLStructure* Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Allocation")
	UAllocatable* Allocated;

	UAllocatable* PrevAllocated;

	UFUNCTION(BlueprintCallable, Category = "Allocation")
	void Allocate(UAllocatable* Allocatable);
	
	UFUNCTION(BlueprintCallable, Category = "Allocation")
	void Unallocate(UAllocatable* Allocatable = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Allocation")
	void Toggle();

	UFUNCTION(BlueprintCallable, Category = "Allocation")
	void RequestAllocation();

	UPROPERTY(BlueprintAssignable, Category = "Allocation|Event")
	FUAllocationSlotOnUnallocation OnUnallocationDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Allocation|Event")
	FUAllocationSlotOnAllocation OnAllocationDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Allocation|Event")
	FUAllocationSlotOnChange OnAllocationChangeDelegate;

	FRequestAllocatable OnRequestAllocatable;


	UFUNCTION(BlueprintNativeEvent)
	void OnAllocation(UAllocationSlot* Slot);

	UFUNCTION(BlueprintNativeEvent)
	void OnUnallocation(UAllocationSlot* Slot);
};
