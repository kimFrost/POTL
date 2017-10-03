// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UObjects/UHexPoint.h"
#include "HexDecal.h"
#include "UHexTile.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class APOTLStructure;
class UStructureComponent;
class UProviderComponent;
class UResource;


//~~~~~ Delegates ~~~~~//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHexToggleAllocate, UHexTile*, Hex);
DECLARE_DELEGATE_RetVal_OneParam(EHandleType, FOnHexClickedDelegate, UHexTile*);
//DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam

UCLASS(Blueprintable, BlueprintType)
class POTL_API UHexTile : public UObject
{
	GENERATED_BODY()

public:
	UHexTile();
	~UHexTile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	UWorld* WorldRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	AHexDecal* HexDecal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	UHexPoint* Point0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	UHexPoint* Point1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	UHexPoint* Point2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	UHexPoint* Point3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	UHexPoint* Point4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	UHexPoint* Point5;

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
	TArray<UHexTile*> HexNeighbors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FST_HexResourceInfo HexResourceInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	FString HexTileType;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hex")
	TMap<FString, int> Resources;

	TArray<UResource*> AvailableResources;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hex")
	TArray<UProviderComponent*> Providers;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hex")
	bool bSelected;

	//UTileManager* TileManager;

	// SupplyTo // GatheredBy // Timer??

	//UStructureComponent* AllocatedTo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hex")
	APOTLStructure* AllocatedTo;

	FOnHexToggleAllocate OnHexToggleAllocate;

	//TArray<FOnHexClickedDelegate*> OnHexClickedDelegates;
	TMap<UObject*, FOnHexClickedDelegate*> OnHexClickedDelegates;

	FOnHexClickedDelegate* BindToOnHexClicked(UObject* Listener, int Priority);
	void UnbindToHexClicked(UObject* Listener);


	//void ListenForClick(UObject* Listener, FHandleDelegate Delegate);

	void Select();
	void Deselect();
	void ClickHex();

	void AddProvider(UProviderComponent* Provider);
	void RemoveProvider(UProviderComponent* Provider);

	void UpdateResources();

	void SortResources();

	UFUNCTION(BlueprintNativeEvent, Category = "Hex")
	void OnHexClicked();	


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	//TArray<FST_ConstructLocation> ConstructLocations;
	FST_ConstructLocation ConstructInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	//TArray<FST_ConstructLocation> ConstructLocations;
	bool DebugMe;


	UFUNCTION(BlueprintCallable, Category = "Hex")
	UHexTile* GetNeighbourHex(int Direction);

	UFUNCTION(BlueprintCallable, Category = "Hex")
	UHexTile* GetNeighbourByOffset(FVector CubeCoord);

	UFUNCTION(BlueprintCallable, Category = "Decal")
	void HideDecal();

	UFUNCTION(BlueprintCallable, Category = "Decal")
	void ShowDecal(EDecalType Type);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Hex")
	void Init();

	bool IsBuildable(); //const FST_BuildInfo& BuildInfo



	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	AActor* HighlightDecal; 
	*/
	// This caused a Pointer Access violation somewhere. Thus crash the game on the first gabage collection.
	// NOTE: It was caused by not having it be initiate in FST_Hex() to be nullptr

	// [2015.09.28-17.57.23:650][ 63]LogWindows: === Critical error: ===
	//Fatal error : [File:D : \BuildFarm\buildmachine_++depot + UE4 - Releases + 4.9\Engine\Source\Runtime\CoreUObject\Private\UObject\GarbageCollection.cpp] [Line:383]
	// Invalid object in GC : 0x000007fe028f000a, ReferencingObject : GI_POTL_C / Engine / Transient.UnrealEdEngine_0 : GI_POTL_C_0, ReferencingObjectClass : BlueprintGeneratedClass / Game / Blueprint / GameInstance / GI_POTL.GI_POTL_C, Property Name : HighlightDecal, Offset : 384, TokenIndex : 16

	
private:


	
};
