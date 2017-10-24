// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UAllocatable.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class UAllocationSlot;
class APOTLStructure;

//~~~~~ Deleagtes ~~~~~//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAllocatableUnallocate, UAllocatable*, Allocatable);


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
	APOTLStructure* StructureOwner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAllocationSlot* AllocatedTo;

	UFUNCTION(BlueprintCallable)
	void Unallocate();

	UPROPERTY(BlueprintAssignable, Category = "Allocation|Event")
	FOnAllocatableUnallocate OnUnallocatedDelegate;
};
