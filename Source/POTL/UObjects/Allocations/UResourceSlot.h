// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UObjects/Allocations/UAllocationSlot.h"
#include "UResourceSlot.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class UResource;


UCLASS(Blueprintable, BlueprintType)
class POTL_API UResourceSlot : public UAllocationSlot
{
	GENERATED_BODY()

public:
	UResourceSlot();
	~UResourceSlot();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Allocation")
	UResource* AllocatedResource;
	
};
