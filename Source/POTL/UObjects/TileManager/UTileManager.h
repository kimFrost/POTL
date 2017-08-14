// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UObjects/UHexTIle.h"
#include "UTileManager.generated.h"



UCLASS(Blueprintable, BlueprintType)
class POTL_API UTileManager : public UObject
{
	GENERATED_BODY()

public:
	UTileManager();
	~UTileManager();


private:


	TArray<UHexTile*> Tiles;


public:

	
};
