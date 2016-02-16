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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
	TArray<AHexDecal*> DecalsType1;


	/****** FUNCTIONS ******/

	UFUNCTION(BlueprintCallable, Category = "Decal")
	void HighlightHex(UPARAM(ref) FST_Hex& Hex, EHighlightType Type, bool Clear);

	UFUNCTION(BlueprintCallable, Category = "Decal")
	void HighlightHexes(UPARAM(ref) TArray<FST_Hex>& Hexes, EHighlightType Type, bool Clear);


	//~~ BeginPlay ~~//
	virtual void BeginPlay() override;

	//~~ Tock ~~//
	virtual void Tick(float DeltaSeconds) override;


};
