// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UAllocatable.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class UAllocationSlot;
class APOTLStructure;

//~~~~~ Deleagtes ~~~~~//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUAllocatableOnAllocate, UAllocatable*, Allocatable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUAllocatableOnUnallocate, UAllocatable*, Allocatable);


UCLASS(Blueprintable, BlueprintType)
class POTL_API UAllocatable : public UObject
{
	GENERATED_BODY()

public:
	UAllocatable();
	~UAllocatable();

private:



public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APOTLStructure* Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAllocationSlot* AllocatedTo;

	UFUNCTION(BlueprintCallable)
	void Allocate(UAllocationSlot* AllocationSlot);

	UFUNCTION(BlueprintCallable)
	void Unallocate();

	UPROPERTY(BlueprintAssignable, Category = "Allocation|Event")
	FUAllocatableOnAllocate OnAllocatedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Allocation|Event")
	FUAllocatableOnUnallocate OnUnallocatedDelegate;
};
