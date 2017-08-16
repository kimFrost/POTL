// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UObjects/UHexTIle.h"
#include "UTileManager.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class AIsland;

//~~~~~ Delegates/Event dispatcher ~~~~~//
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTileManagerOnGridReady);


UCLASS(Blueprintable, BlueprintType)
class POTL_API UTileManager : public UObject
{
	GENERATED_BODY()

public:
	UTileManager();
	~UTileManager();

	void Setup(AIsland* Island);

private:

	float HexWidth;
	float HexHeight;
	int32 GridXCount;
	int32 GridYCount;
	TArray<UHexPoint*> Points;
	TArray<UHexTile*> Hexes;
	bool HexGridReady;

	AIsland* WorldActor;
	UWorld* CurrentWorld;


	// DecalDrawer

	/*
	void InitializeWorld();

	void TraceLandscape();
	void CreateHexes();
	void CleanHexes();
	void EnrichHexes();
	void CalcHexesRot();
	void AnalyseLandscape();
	void CalcHexResourceDensity();
	void InjectTestValuesToHexes();
	*/


public:

	/*********** Delegates **************/

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Grid")
	FTileManagerOnGridReady OnMapReady;
	
};
