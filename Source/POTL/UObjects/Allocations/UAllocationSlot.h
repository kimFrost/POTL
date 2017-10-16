// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UObjects/Allocations/UAllocatable.h"
#include "UAllocationSlot.generated.h"


//~~~~~ Forward Declarations ~~~~~//
//class UAllocationSlot;


//~~~~~ Deleagtes ~~~~~//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAllocatableUnallocated, UAllocationSlot*, AllocationSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAllocatableAllocated, UAllocationSlot*, AllocationSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAllocatableChange, UAllocationSlot*, AllocationSlot);
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Allocation")
	UAllocatable* Allocated;

	UAllocatable* PrevAllocated;

	UFUNCTION(BlueprintCallable, Category = "Allocation")
	void Allocate(UAllocatable* Allocatable);
	
	UFUNCTION(BlueprintCallable, Category = "Allocation")
	void Unallocate();

	UFUNCTION(BlueprintCallable, Category = "Allocation")
	void Toggle();

	UFUNCTION(BlueprintCallable, Category = "Allocation")
	void RequestAllocation();

	UPROPERTY(BlueprintAssignable, Category = "Allocation|Event")
	FOnAllocatableUnallocated OnUnallocatedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Allocation|Event")
	FOnAllocatableAllocated OnAllocatedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Allocation|Event")
	FOnAllocatableChange OnAllocatedChange;

	FRequestAllocatable OnRequestAllocatable;


};
