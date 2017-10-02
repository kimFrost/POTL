// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UStructureTaskComponent.h"
#include "UObjects/UHexTile.h"
#include "UGatherComponent.generated.h"


class UResource;
class UProviderComponent;


//~~ DELEGATES ~~//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProductionChangedDelegate, TMap<FString, int>, Production);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProductionChangedDelegate, const TArray<FST_ResourceQuantity>&, TotalProduction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProductionChangedDelegate, const TArray<UResource*>&, TotalProduction);



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class POTL_API UGatherComponent : public UStructureTaskComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UGatherComponent();


	UProviderComponent* ProviderComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gather")
	bool bGatherFromAll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gather")
	int GatherRange;

	TArray<UHexTile*> GatherFrom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gather")
	TArray<FString> GatherResources;

	int MaxGatheredPerCycle;

	FTimerHandle GatherCheckTimer;

	void ValidateRequirements();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gather")
	TArray<FST_TileConversion> TileConversions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gather")
	TMap<FString, int> PetalProduction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gather")
	TMap<FString, int> StoredPetals;


	TArray<FST_ResourceQuantity> TotalResourceProduction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gather")
	TArray<UResource*> ProducedResources;


	UFUNCTION(BlueprintCallable, Category = "Gather")
	TArray<FST_ResourceQuantity> GetTotalPetalProduction();

	UFUNCTION(BlueprintCallable, Category = "Gather")
	TArray<FST_ResourceQuantity> GetTileResourceOutput(UHexTile* Hex);
	//void GetConversion(UHexTile* Hex, UPARAM(ref) TArray<FST_TileConversion>& Conversions);
	
	UFUNCTION(BlueprintCallable, Category = "Gather")
	void CalcPetalProduction();

	void AddPetal(FString PetalId, int Quantity);
	void CollectPetals();
	void ConvertPetals();
	bool IsHexWorkable(UHexTile* Hex);

	//int32 CalcAvaiableResources(UDataTable* RecipeTable);

	UFUNCTION(Category = "Structure")
	EHandleType ParseAllocateHex(UHexTile* Hex);

	UFUNCTION(Category = "Structure")
	EHandleType ParseUnallocateHex(UHexTile* Hex);

	UPROPERTY(BlueprintAssignable, Category = "Gather|Event")
	FOnProductionChangedDelegate OnProductionChangedDelegate;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Production")
	void OnGathered();



	virtual void Init() override;

	virtual void OnProgressComplete() override;

	virtual void BeginPlay() override;
};
