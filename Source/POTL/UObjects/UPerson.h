// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UObjects/Allocations/UAllocatable.h"
#include "UPerson.generated.h"


//~~~~~ Forward Declarations ~~~~~//


UCLASS(Blueprintable, BlueprintType)
class POTL_API UPerson : public UAllocatable
{
	GENERATED_BODY()

public:
	UPerson();
	~UPerson();

	
};
