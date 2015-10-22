// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "POTLGameInstance.h"
#include "POTLStructure.generated.h"


//~~~~~ Forward Declarations ~~~~~


//~~~~~ STRUCTS ~~~~~

//~~~~~ ENUMS ~~~~~
UENUM(BlueprintType)
enum class EResourceList : uint8
{
	Storage UMETA(DisplayName = "Storage"),
	Requirements UMETA(DisplayName = "Requirements"),
	Alterations UMETA(DisplayName = "Alterations")
};


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
	FName TreeId;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	//FST_Hex Hex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	int32 HexIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FST_Structure StructureBaseData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	int32 BroadcastRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<int32> BroadcastGridHexIndexes;

	

	/** Resources */

	UPROPERTY(EditAnywhere, Category = "Resources")
	TMap<FString, int32> Resources;
	//TArray<FST_Resource> Resources;
	//TMap<FName, FST_Resource> Resources;

	UPROPERTY(EditAnywhere, Category = "Resources")
	TMap<FString, int32> ResourceAlterations;
	//TArray<FST_Resource> ResourceAlterations;
	//TMap<FName, FST_Resource> ResourceAlterations;
	
	UPROPERTY(EditAnywhere, Category = "Resources")
	TMap<FString, int32> ResourceRequirements;
	//TArray<FST_Resource> ResourceRequirements;
	//TMap<FName, FST_Resource> ResourceRequirements;
	
	UPROPERTY(EditAnywhere, Category = "Resources")
	TMap<FString, int32> ResourcesPromisedAway;

	UPROPERTY(EditAnywhere, Category = "Resources")
	TMap<FString, int32> ResourceUpkeep;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	TArray<FST_Factory> Factories;

	/*********** FUNCTIONS **************/

	/** Resources */

	UFUNCTION(BlueprintCallable, Category = "Resources")
	bool AddResource(FString Id, int32 Quantity , EResourceList Type);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	TArray<FST_Resource> GetResourcesAsList(EResourceList Type);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void CalculateUpkeep();

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void ResolveTree();

	//UFUNCTION(Category = "Resources") // Not a UFunction. Can only be called in c++ because of the TMap, which isn't supported in Blueprint
	//TMap<FName, FST_Resource> RequestResources(bool Bubble, const APOTLStructure* RequestFrom, const TMap<FName, FST_Resource>& Request, int32 Steps);
	//TMap<FName, FST_Resource> RequestResources(bool Bubble, APOTLStructure* RequestFrom, TMap<FName, FST_Resource>& Request, int32 Steps);
	bool RequestResources(bool Bubble, APOTLStructure* RequestFrom, TMap<FString, int32>& Request, TMap<FString, int32>& Allocations, int32 Steps);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
};
