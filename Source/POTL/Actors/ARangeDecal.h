// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DecalActor.h"
#include "ARangeDecal.generated.h"


class UHexTile;


UCLASS()
class POTL_API ARangeDecal : public ADecalActor
{
	GENERATED_BODY()
	
public:	
	
	UFUNCTION(BlueprintNativeEvent)
	void OnSpawnHexRange(FVector Location, const TArray<UHexTile*>& Hexes);
};
