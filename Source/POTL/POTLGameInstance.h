// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "POTLDataHolder.h"
#include "UObjects/UHexPoint.h"
#include "UObjects/UHexTile.h"
#include "UObjects/UResourceMap.h"
#include "UObjects/UStorageMap.h"
#include "UObjects/UTransaction.h"
#include "POTLUtilFunctionLibrary.h"
#include "POTLGameInstance.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class APOTLStructure;
class UStructureComponent;
class UStorageComponent;
class AIsland;



//~~~~~ Delegates/Event dispatcher ~~~~~//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurnSwitched);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams();
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMapReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTransaction, UResourceTransaction*, Transaction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStructurePlanted, APOTLStructure*, Structure);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStructureSelectedDelegate, APOTLStructure*, Structure);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStructureDeselectedDelegate, APOTLStructure*, Structure);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnMessageDelegate, FString, Message, EMessageType, Type, FVector, WorldLocation);


//DECLARE_EVENT(FTurnSwitched);




UCLASS()
class POTL_API UPOTLGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	// Sets default values for this's properties
	UPOTLGameInstance(const FObjectInitializer &ObjectInitializer);


	/*********** VARIABLES **************/

	// Map
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid")
	float HexWidth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Calculated")
	float HexHeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Calculated")
	int32 GridXCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Calculated")
	int32 GridYCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid")
	TArray<UHexPoint*> Points;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid")
	TArray<UHexTile*> Hexes;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource") // Nested containers not supported
	//TMap<FString, TArray<UHexTile*>> ResourceHexMap;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid")
	bool HexGridReady;
	
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid")
	//AActor* Landscape;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid")
	AIsland* WorldActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid")
	UWorld* CurrentWorld;

	int ResourceUniqueIdCounter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Resource")
	UResourceMap* ResourceMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Resource")
	UStorageMap* StorageMap;

	TArray<FString> CachedStructureIds;
	TArray<FString> CachedResourceIds;
	TArray<FString> CachedCollectionIds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	UDataTable* DATA_Recipes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	UDataTable* DATA_Structures;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	UDataTable* DATA_Resources;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	UDataTable* DATA_Collections;


	FTimerHandle TurnTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid")
	TEnumAsByte<ECollisionChannel> ChannelLandscape;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid")
	TEnumAsByte<ECollisionChannel> ChannelFoliage;


	//~~ Structure ~~//

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	TArray<APOTLStructure*> Structures;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	TArray<APOTLStructure*> PlaceholderStructures;


	/*********** FUNCTIONS **************/

	//~~ Data ~~//

	void InitializeWorld();

	UFUNCTION(BlueprintCallable, Category = "Data")
	void ReadTables();

	//~~ Structure ~~//

	UFUNCTION(BlueprintCallable, Category = "Structure")
	bool IsHexBlocked(const UHexTile* Hex);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	bool IsHexBuildable(const UHexTile* Hex);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	bool IsHexTerrainBuildable(const UHexTile* Hex);

	bool ValidatePlaceStructureOnHex(UHexTile* Hex, const FST_BuildInfo& BuildInfo); // FString StructureId, , int Rotation

	FST_Structure* GetStructureRowData(FString RowName);

	//class TreeMeshClass;
	TSubclassOf<UStaticMesh> TreeMeshClass;
	//UStaticMesh* TreeStaticMesh;
	UStaticMeshComponent* TreeStaticMesh;

	UFUNCTION(BlueprintCallable, Category = "World")
	void PlantForest(UHexTile* OnHex, int Density);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	APOTLStructure* PlantPlaceholderStructure(FVector CubeCoord, int32 RotationDirection, FString RowName, APOTLStructure* AttachTo, bool InstaBuild);
	
	UFUNCTION(BlueprintCallable, Category = "Structure")
	APOTLStructure* PlantStructure(FVector CubeCoord, int32 RotationDirection, FString RowName, APOTLStructure* AttachTo, bool InstaBuild, bool IsPlaceholder);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	void RemoveStructure(APOTLStructure* Structure);

	/*
private:
	TArray<UHexTile*> StoredPotentialDetailsHexes;
public:
	void ShowPotentialPlacementDetails(UHexTile* Hex);
	void CleanupPlacementDetails();
*/

	//~~ Map - Creation ~~//

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void TraceLandscape();

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void CreateHexes();

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void CleanHexes();

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void EnrichHexes();

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void CalcHexesRot();

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void AnalyseLandscape();

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void CalcHexResourceDensity();

	void InjectTestValuesToHexes();

	//~~ Map - Resources ~~//

	void IncludeStorage(UStorageComponent* StorageComp);

	UResource* CreateResource(FString ResourceId);
	UResource* CreateResourceByTag(FString TagId);
	UResource* RequestResource(APOTLStructure* Requester, FString ResourceId);
	UResource* RequestResourceByTag(APOTLStructure* Requester, FString Tag);


	void TransferResource(UResource* Resource, UStructureComponent* ToComp, bool Consume, bool IsFree);




	//~~ Util - Hex ~~//

	UFUNCTION(BlueprintCallable, Category = "Util")
	UHexTile* MouseToHex();

	UFUNCTION(BlueprintCallable, Category = "Util")
	UHexTile* LocationToHex(FVector Location);


	//~~ Util - Feedback ~~//
	UPROPERTY(BlueprintAssignable, Category = "Util")
	FOnMessageDelegate OnMessageDelegate;

	void ShowFeedbackMsg(FString Message, EMessageType Type, FVector WorldLocation);

	//~~ Global dispatchers ~~//

	// Structure selected

	// Hex selected


	//~~ Debug - Log ~~//

	UFUNCTION(BlueprintCallable, Category = "Debug")
	static void Log(FString Msg, float Duration, FColor DebugColor, int32 GroupIndex);


	/*********** Delegates **************/

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Grid")
	FOnMapReady OnMapReady;

	UPROPERTY(BlueprintAssignable, Category = "Structure")
	FStructurePlanted OnStructurePlanted;

	UPROPERTY(BlueprintAssignable, Category = "Structure")
	FOnTransaction OnTransaction;

	UPROPERTY(BlueprintAssignable, Category = "Structure")
	FOnStructureSelectedDelegate OnStructureSelectedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Structure")
	FOnStructureDeselectedDelegate OnStructureDeselectedDelegate;

};
