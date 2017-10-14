// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UAllocatable.generated.h"


//~~~~~ Forward Declarations ~~~~~//


UCLASS(Blueprintable, BlueprintType)
class POTL_API UAllocatable : public UObject
{
	GENERATED_BODY()

public:
	UAllocatable();
	~UAllocatable();

	
};
