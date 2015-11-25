// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "POTLStructure.generated.h"


//~~~~~ Forward Declarations ~~~~~


//~~~~~ STRUCTS ~~~~~



 
//~~~~~ ENUMS ~~~~~



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
	TArray<APOTLStructure*> BroadcastTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	//TArray<APOTLStructure*> EmitTo;
	APOTLStructure* EmitTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool IsRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	APOTLStructure* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FString TreeId;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	//FST_Hex& Hex; // Need to be put as a spawn paramater

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	int32 BroadcastRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	int32 BroadcastHexIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<int32> BroadcastGridHexIndexes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool IsResolvedThisTurn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool InRangeOfEmitTo;

	/** Construction */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool IsPlaceholder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool IsUnderConstruction;

	/** Resources */

	UPROPERTY(EditAnywhere, Category = "Resources")
	TMap<FString, int32> FreeResources;

	UPROPERTY(EditAnywhere, Category = "Resources")
	//TArray<FST_ResourceAllocation> AllocatedResources;
	TMap<int32, FST_ResourceAllocation> AllocatedResources;

	//UPROPERTY(EditAnywhere, Category = "Resources")
	//TMap<FString, int32> Resources;

	//UPROPERTY(EditAnywhere, Category = "Resources")
	//TMap<FString, int32> ResourceAlterations;
	
	//UPROPERTY(EditAnywhere, Category = "Resources")
	//TMap<FString, int32> ResourceRequirements;

	//UPROPERTY(EditAnywhere, Category = "Resources")
	//TMap<FString, int32> ResourcesPromisedAway;

	UPROPERTY(EditAnywhere, Category = "Resources")
	TMap<FString, int32> ResourceUpkeep;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	TArray<FST_Factory> Factories;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	FST_Factory ProductionFactory;

	/*********** FUNCTIONS **************/

	/** Resources */

	UFUNCTION(BlueprintCallable, Category = "Resources")
	bool AddResource(FString Id, int32 Quantity , EResourceList Type);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	TArray<FST_Resource> GetResourcesAsList(EResourceList Type);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void OptimizeAllocatedResources();

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void CalculateUpkeep(bool Broadcast);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void ResolveTree();

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void ResolveAllocations(EAllocationType Type, bool Broadcast);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	int32 AllocateResource(APOTLStructure* From, FString ResourceKey, int32 Quantity, EAllocationType Type, bool KeyLoop);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void ResolveUpkeep(bool Broadcast);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void ProcessFactories(bool Broadcast);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void ResolveFactories(bool Broadcast);

	//UFUNCTION(Category = "Resources") // Not a UFunction. Can only be called in c++ because of the TMap, which isn't supported in Blueprint
	//TMap<FName, FST_Resource> RequestResources(bool Bubble, const APOTLStructure* RequestFrom, const TMap<FName, FST_Resource>& Request, int32 Steps);
	//TMap<FName, FST_Resource> RequestResources(bool Bubble, APOTLStructure* RequestFrom, TMap<FName, FST_Resource>& Request, int32 Steps);
	bool RequestResources(bool Bubble, APOTLStructure* RequestFrom, TMap<FString, int32>& Request, int32 Steps, EAllocationType Type, bool Consume);


	/** Construction */

	UFUNCTION(BlueprintCallable, Category = "Construction")
	FST_Factory CreateFactory();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Construction") // BlueprintNativeEvent
	void UpdateConstrunction();
	

	/** Map */

	UFUNCTION(BlueprintCallable, Category = "Map")
	APOTLStructure* GetNearestStructure();


	//Delegates
	UFUNCTION()
	void BindToDelegate(float Turn);


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
};
