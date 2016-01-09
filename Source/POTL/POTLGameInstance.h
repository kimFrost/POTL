// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "POTLDataHolder.h"
#include "POTLUtilFunctionLibrary.h"
#include "POTLGameInstance.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class APOTLStructure;




//~~~~~ Delegates/Event dispatcher ~~~~~//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurnSwitched);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams();
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTurnSwitched, float, Turn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewTurn, float, Turn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStructurePlanted, APOTLStructure*, Structure);
//DECLARE_EVENT(FTurnSwitched);



 



/**
*
*/
UCLASS()
class POTL_API UPOTLGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	// Sets default values for this's properties
	UPOTLGameInstance(const FObjectInitializer &ObjectInitializer);


	/*********** VARIABLES **************/

	// Map
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Map")
	float HexWidth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Calculated")
	float HexHeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Calculated")
	int32 GridXCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Calculated")
	int32 GridYCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Map")
	TArray<FST_Point> Points;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Map")
	TArray<FST_Hex> Hexes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Map")
	bool HexGridReady;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Map")
	AActor* Landscape;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Resources")
	//UDataTable* RecipeTable;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	//UDataTable* StructureTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	UDataTable* DATA_Recipes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	UDataTable* DATA_Structures;

	FTimerHandle TurnTimerHandle;

	//~~ Structure ~~//

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	TArray<APOTLStructure*> Structures;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	TArray<APOTLStructure*> RootStructures;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	TArray<APOTLStructure*> PlaceholderStructures;

	//~~ Debug ~~//
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Debug")
	FString FindmeString;

	/*********** FUNCTIONS **************/

	//~~ Data ~~//

	UFUNCTION(BlueprintCallable, Category = "Data")
	void ReadTables();

	//~~ Structure ~~//

	UFUNCTION(BlueprintCallable, Category = "Structure")
	TArray<FST_Hex> GetConstructLocations(APOTLStructure* Structure, bool IncludeChildren);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	TArray<int32> GetConstructLocationIndexes(APOTLStructure* Structure, bool IncludeChildren);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	bool IsHexBlocked(const FST_Hex& Hex);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	bool IsHexBuildable(const FST_Hex& Hex);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	bool IsHexTerrainBuildable(const FST_Hex& Hex);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	APOTLStructure* PlantPlaceholderStructure(FVector CubeCoord, int32 RotationDirection, FString RowName, FString TreeId, APOTLStructure* EmitTo, bool InstaBuild);
	
	UFUNCTION(BlueprintCallable, Category = "Structure")
	APOTLStructure* PlantStructure(FVector CubeCoord, int32 RotationDirection, FString RowName, FString TreeId, APOTLStructure* EmitTo, bool InstaBuild, bool IsPlaceholder);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	void RemoveStructure(APOTLStructure* Structure);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	void CreateStructureConnection(APOTLStructure* From, APOTLStructure* To);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	void RemoveStructureConnection(APOTLStructure* From, APOTLStructure* To);


	//~~ Map - Creation ~~//

	UFUNCTION(BlueprintCallable, Category = "Map")
	void TraceLandscape(ECollisionChannel CollisionChannel);

	UFUNCTION(BlueprintCallable, Category = "Map")
	void CreateHexes(ECollisionChannel CollisionChannel);

	UFUNCTION(BlueprintCallable, Category = "Map")
	void CleanHexes();

	UFUNCTION(BlueprintCallable, Category = "Map")
	void EnrichHexes();

	UFUNCTION(BlueprintCallable, Category = "Map")
	void CalcHexesRot();

	UFUNCTION(BlueprintCallable, Category = "Map")
	void AnalyseLandscape(ECollisionChannel LandscapeCollisionChannel, ECollisionChannel FoliageCollisionChannel);

	UFUNCTION(BlueprintCallable, Category = "Map")
	void CalcHexResourceDensity();

	//~~ Turn ~~//

	UFUNCTION(BlueprintCallable, Category = "Turn")
	void SwitchTurn();

	UFUNCTION(BlueprintCallable, Category = "Turn")
	void NewTurn(float WaitTime);

	//~~ Map - Structure ~~//

	UFUNCTION(BlueprintCallable, Category = "Map")
	APOTLStructure* GetNearestStructure(FVector Location, TSubclassOf<APOTLStructure> StructureClass);

	UFUNCTION(BlueprintCallable, Category = "Map")
	APOTLStructure* GetNearestCity(FVector Location);

	//~~ Util - Hex ~~//

	UFUNCTION(BlueprintCallable, Category = "Util")
	FST_Hex MouseToHex(ECollisionChannel CollisionChannel);

	UFUNCTION(BlueprintCallable, Category = "Util")
	FST_Hex LocationToHex(FVector Location);


	//~~ Debug - Log ~~//

	UFUNCTION(BlueprintCallable, Category = "Debug")
	static void Log(FString Msg, float Duration, FColor DebugColor, int32 GroupIndex);


	/*********** Delegates **************/

	UPROPERTY(BlueprintAssignable, Category = "Turn")
	FTurnSwitched OnTurnSwitched;

	UPROPERTY(BlueprintAssignable, Category = "Turn")
	FNewTurn OnNewTurn;

	UPROPERTY(BlueprintAssignable, Category = "Structure")
	FStructurePlanted OnStructurePlanted;

};
