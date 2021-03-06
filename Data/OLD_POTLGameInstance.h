// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "POTLDataHolder.h"
#include "UObjects/UHexPoint.h"
#include "UObjects/UHexTile.h"
#include "UObjects/UResourceMap.h"
#include "UObjects/UStorageMap.h"
#include "POTLUtilFunctionLibrary.h"
#include "POTLGameInstance.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class APOTLStructure;
class UStructureComponent;
class UStorageComponent;




//~~~~~ Delegates/Event dispatcher ~~~~~//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurnSwitched);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams();
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMapReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTurnSwitched, float, Turn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewTurn, float, Turn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStructurePlanted, APOTLStructure*, Structure);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCityUpdated, APOTLStructure*, CityRootStructure);
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid")
	bool HexGridReady;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid")
	AActor* Landscape;


	//TMap<FVector, UStorageComponent*> StorageMap;

	//TMap<FString, UStorageComponent*> ResourceMap; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Resource")
	UResourceMap* ResourceMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Resource")
	UStorageMap* StorageMap;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	UDataTable* DATA_Recipes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	UDataTable* DATA_Structures;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structure")
	UDataTable* DATA_Resources;


	FTimerHandle TurnTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid")
	TEnumAsByte<ECollisionChannel> ChannelLandscape;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid")
	TEnumAsByte<ECollisionChannel> ChannelFoliage;



	//~~ People ~~//

	UPROPERTY(EditDefaultsOnly, Category = "People")
	TMap<int32, FST_Person> PeopleInMap;


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
	TArray<UHexTile*> GetConstructLocations(APOTLStructure* Structure, bool IncludeChildren);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	TArray<int32> GetConstructLocationIndexes(APOTLStructure* Structure, bool IncludeChildren);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	bool IsHexBlocked(const UHexTile* Hex);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	bool IsHexBuildable(const UHexTile* Hex);

	UFUNCTION(BlueprintCallable, Category = "Structure")
	bool IsHexTerrainBuildable(const UHexTile* Hex);

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


	//~~ Turn ~~//

	UFUNCTION(BlueprintCallable, Category = "Turn")
	void SwitchTurn();

	UFUNCTION(BlueprintCallable, Category = "Turn")
	void NewTurn(float WaitTime);


	//~~ Map - Structure ~~//

	UFUNCTION(BlueprintCallable, Category = "Grid")
	APOTLStructure* GetNearestStructure(FVector Location, TSubclassOf<APOTLStructure> StructureClass);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	APOTLStructure* GetNearestCity(FVector Location);


	//~~ Map - Resources ~~//

	void IncludeStorage(UStorageComponent* StorageComp);

	UStorageComponent* FindResource(FString ResourceId, int Quantity);

	UResource* RequestResource(APOTLStructure* Requester, FString ResourceId);

	void TransferResource(UResource* Resource, UStructureComponent* ToComp, bool Consume, bool IsFree);


	//~~ People ~~//

	UFUNCTION(BlueprintCallable, Category = "People")
	int32 CreatePerson(FString FirstName, FString FamilyName, FString NickName, int32 Age, EPersonGender Gender, APOTLStructure* Home);

	UFUNCTION(BlueprintCallable, Category = "People")
	void SwitchHome(UPARAM(ref) FST_Person& Person, APOTLStructure* NewHome);

	UFUNCTION(BlueprintCallable, Category = "People")
	FST_Person GetPerson(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "People")
	TArray<FST_Person> GetPeople(const TArray<int32>& Indexes);

	UFUNCTION(BlueprintCallable, Category = "People")
	void SetPersonData(APOTLStructure* AssignedTo, EPersonBaseTaskList AssignedTask, int32 OwnIndex);


	//~~ Util - Hex ~~//

	UFUNCTION(BlueprintCallable, Category = "Util")
	UHexTile* MouseToHex();

	UFUNCTION(BlueprintCallable, Category = "Util")
	UHexTile* LocationToHex(FVector Location);


	//~~ Debug - Log ~~//

	UFUNCTION(BlueprintCallable, Category = "Debug")
	static void Log(FString Msg, float Duration, FColor DebugColor, int32 GroupIndex);


	/*********** Delegates **************/

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Grid")
	FOnMapReady OnMapReady;

	UPROPERTY(BlueprintAssignable, Category = "Turn")
	FTurnSwitched OnTurnSwitched;

	UPROPERTY(BlueprintAssignable, Category = "Turn")
	FNewTurn OnNewTurn;

	UPROPERTY(BlueprintAssignable, Category = "Structure")
	FStructurePlanted OnStructurePlanted;

	UPROPERTY(BlueprintAssignable, Category = "Structure")
	FCityUpdated OnCityUpdated;
	

};
