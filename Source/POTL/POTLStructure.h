// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "UObjects/UResource.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "UObjects/UHexTile.h"
#include "POTLStructure.generated.h"


//~~ DELEGATES ~~//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConstructionComplete);

//~~~~~ Forward Declarations ~~~~~

//~~~~~ STRUCTS ~~~~~
 
//~~~~~ ENUMS ~~~~~


//~~~~~ Forward Declarations ~~~~~//
class UConstructionComponent;


UCLASS()
class POTL_API APOTLStructure : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	APOTLStructure(const FObjectInitializer &ObjectInitializer);
	//AVehicle(const class FPostConstructInitializeProperties& PCIP, FString Path, FString Name);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	UPOTLGameInstance* GameInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	APOTLStructure* AttachedTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<APOTLStructure*> AttachedStructures;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	UHexTile* Hex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<UHexTile*> OccupiedHexes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	int32 HexIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FVector CubeCoord;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FVector CubeCoords;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FST_Structure StructureBaseData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FString StructureRowName;


	/** Construction */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool IsPlaceholder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool IsUnderConstruction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool bIsInitialized;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool BlockPathing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure")
	UConstructionComponent* ConstructionComponent;

	//FTimerHandle ConstructionProgressCheckTimer;


	//OnConstructionProgress(float ProcentConstructed)


	/*********** FUNCTIONS **************/

	/** Resources */

	UFUNCTION(BlueprintCallable, Category = "Resources")
	int AddResource(FString ResourceId, int32 Quantity); // , EResourceList Type

	UFUNCTION(BlueprintCallable, Category = "Resources")
	bool StoreResource(UResource* Resource);

	void AddWealth(float Amount);

	void SubtractWealth(float Amount);

	// Security. Constant. Church gives security if religious

	//bool AddNeed(ENeedType Need, float Amount);


	/** Construction */

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Construction")
	void UpdateConstrunction();
	
	UFUNCTION(BlueprintCallable, Category = "Construction")
	void Init();

	UFUNCTION(BlueprintCallable, Category = "Construction")
	void RemoveStructure();

	UFUNCTION(BlueprintCallable, Category = "Construction")
	void ProcessBaseData();

	UFUNCTION(BlueprintCallable, Category = "Construction")
	void AttachToStructure(APOTLStructure* Structure);

	UFUNCTION(BlueprintCallable, Category = "Construction")
	void DetachFromStructure();

	UFUNCTION(BlueprintCallable, Category = "Construction")
	void CompleteConstruction();





	/** Map */

	UFUNCTION(BlueprintCallable, Category = "Map")
	APOTLStructure* GetNearestStructure();


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time")
	void OnTimeUpdate(float Time, float TimeProgressed);

	UFUNCTION(BlueprintNativeEvent, Category = "Structure")
	void OnInit();

	UFUNCTION(BlueprintNativeEvent, Category = "Structure")
	void OnConstructionComplete();


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
};
