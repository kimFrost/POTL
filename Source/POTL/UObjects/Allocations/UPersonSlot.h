// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UObjects/Allocations/UAllocationSlot.h"
#include "UPersonSlot.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class UPerson;


UCLASS(Blueprintable, BlueprintType)
class POTL_API UPersonSlot : public UAllocationSlot
{
	GENERATED_BODY()

public:
	UPersonSlot();
	~UPersonSlot();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Allocation")
	UPerson* AllocatedPerson;
	
};
