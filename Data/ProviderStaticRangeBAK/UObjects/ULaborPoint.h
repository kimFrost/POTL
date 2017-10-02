// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "ULaborPoint.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class APOTLStructure;


UCLASS(Blueprintable, BlueprintType)
class POTL_API ULaborPoint : public UObject
{
	GENERATED_BODY()

public:
	ULaborPoint();
	~ULaborPoint();

	
	APOTLStructure* GetOwner();

	void Init();

	
};
