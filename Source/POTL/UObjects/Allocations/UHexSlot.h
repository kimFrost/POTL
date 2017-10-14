// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UObjects/Allocations/UAllocationSlot.h"
#include "UHexSlot.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class UHexTile;


UCLASS(Blueprintable, BlueprintType)
class POTL_API UHexSlot : public UAllocationSlot
{
	GENERATED_BODY()

public:
	UHexSlot();
	~UHexSlot();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Allocation")
	UHexTile* AllocatedHex;
	
};
