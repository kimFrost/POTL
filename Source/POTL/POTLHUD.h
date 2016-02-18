// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "HexDecal.h"
#include "POTLHUD.generated.h"






UCLASS()
class POTL_API APOTLHUD : public AHUD
{
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties
	APOTLHUD(const FObjectInitializer &ObjectInitializer);
	
	//UFUNCTION(BlueprintCallable, Category = "Map")
	//APOTLStructure* GetNearestStructure(FVector Location, TSubclassOf<APOTLStructure> StructureClass);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
	TArray<AHexDecal*> FreeDecals;
	//ADecalActor


	/****** FUNCTIONS ******/

	UFUNCTION(BlueprintCallable, Category = "Decal")
	void SpawnDecals();

	UFUNCTION(BlueprintCallable, Category = "Decal")
	void ClearHighlightedHexes(EHighlightType Type);

	UFUNCTION(BlueprintCallable, Category = "Decal")
	void ClearDecals(UPARAM(ref) TArray<AHexDecal*>& Decals);

	UFUNCTION(BlueprintCallable, Category = "Decal")
	AHexDecal* HighlightHex(UPARAM(ref) FST_Hex& Hex, EHighlightType Type);

	//void HighlightHex(UPARAM(ref) FST_Hex& Hex, EHighlightType Type, Decals);

	UFUNCTION(BlueprintCallable, Category = "Decal")
	TArray<AHexDecal*> HighlightHexes(UPARAM(ref) TArray<FST_Hex>& Hexes, EHighlightType Type);


	//~~ BeginPlay ~~//
	virtual void BeginPlay() override;

	//~~ Tock ~~//
	virtual void Tick(float DeltaSeconds) override;


};
