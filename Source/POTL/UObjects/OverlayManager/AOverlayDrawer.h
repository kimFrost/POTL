// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DecalActor.h"
#include "AOverlayDrawer.generated.h"

/**
 * 
 */
UCLASS()
class POTL_API AOverlayDrawer : public ADecalActor
{
	GENERATED_BODY()
	
	

public:


	UFUNCTION(BlueprintNativeEvent, Category = "Overlay")
	void OnDrawOverlay(const TArray<UHexTile*>& Hexes);
	
};
