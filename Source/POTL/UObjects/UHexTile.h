// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "HexDecal.h"
#include "UHexTile.generated.h"


/**
 * 
 */
UCLASS()
class POTL_API UHexTile : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	AHexDecal* HexDecal;


	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_Point Point0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_Point Point1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_Point Point2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_Point Point3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_Point Point4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_Point Point5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	bool Remove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	APOTLStructure* AttachedBuilding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	AActor* AttachedActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	AActor* AttachedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FVector HexCubeCoords;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FVector2D HexOffsetCoords;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	int32 HexIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	TArray<int32> HexNeighborIndexes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_HexResourceInfo Resources;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	//TArray<FST_ConstructLocation> ConstructLocations;
	FST_ConstructLocation ConstructInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	AActor* HighlightDecal; 
	// This caused a Pointer Access violation somewhere. Thus crash the game on the first gabage collection.
	// NOTE: It was caused by not having it be initiate in FST_Hex() to be nullptr

	// [2015.09.28-17.57.23:650][ 63]LogWindows: === Critical error: ===
	//Fatal error : [File:D : \BuildFarm\buildmachine_++depot + UE4 - Releases + 4.9\Engine\Source\Runtime\CoreUObject\Private\UObject\GarbageCollection.cpp] [Line:383]
	// Invalid object in GC : 0x000007fe028f000a, ReferencingObject : GI_POTL_C / Engine / Transient.UnrealEdEngine_0 : GI_POTL_C_0, ReferencingObjectClass : BlueprintGeneratedClass / Game / Blueprint / GameInstance / GI_POTL.GI_POTL_C, Property Name : HighlightDecal, Offset : 384, TokenIndex : 16

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	//TArray<FST_ConstructLocation> ConstructLocations;
	bool DebugMe;

	FST_Hex()
	{
		Location = { -1, -1, -1 };
		Rotation = { -1, -1, -1 };
		Remove = false;
		AttachedBuilding = nullptr;
		AttachedActor = nullptr;
		AttachedMesh = nullptr;
		HexCubeCoords = {-1, -1, -1};
		HexOffsetCoords = {-1, -1};
		HexIndex = -1;
		HexNeighborIndexes.Init(-1, 6);
		HighlightDecal = nullptr;
		DebugMe = false;
	}
	*/
	
};
