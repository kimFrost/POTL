// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DecalActor.h"
#include "POTLDataHolder.h"
#include "ARangeDecal.generated.h"


class UHexTile;


UCLASS()
class POTL_API ARangeDecal : public ADecalActor
{
	GENERATED_BODY()
	
public:

	void DrawHexes(const TArray<UHexTile*>& Hexes);

	void DrawStructureInfo(const FST_Structure& StructureInfo);

	UFUNCTION(BlueprintNativeEvent)
	void OnClear();

	//UFUNCTION(BlueprintNativeEvent)
	//void OnDrawHexes(const TArray<UHexTile*>& Hexes);

	UFUNCTION(BlueprintNativeEvent)
	void OnDrawHex(const UHexTile* Hex, const FLinearColor Color);

	UFUNCTION(BlueprintNativeEvent)
	void OnDrawHexes(const TArray<FHexLinearColor>& HexColorList);


	//FHexLinearColor

	UFUNCTION(BlueprintNativeEvent)
	void OnDrawStructureInfo(const FST_Structure& StructureInfo);
};
