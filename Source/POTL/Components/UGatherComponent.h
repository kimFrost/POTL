// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UStructureTaskComponent.h"
#include "UObjects/UHexTile.h"
#include "UGatherComponent.generated.h"


class UResource;
class UProviderComponent;
class UAllocationSlot;
class UPersonSlot;
class UHexSlot;


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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gather")
	TArray<UPersonSlot*> AllocatedPersonSlots;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gather")
	TArray<UHexSlot*> AllocatedTileSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gather")
	int GatherRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gather")
	TArray<FST_TileConversion> TileConversions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gather")
	TArray<UResource*> GatheredResources;

	TMap<UHexTile*, TArray<UResource*>> GatheredResourcesByHex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gather")
	FST_Gatherer BaseData;

	UFUNCTION(Category = "Gather")
	void IncludeHex(UAllocationSlot* AllocationSlot, UAllocatable* Allocatable);

	UFUNCTION(Category = "Gather")
	void ExcludeHex(UAllocationSlot* AllocationSlot, UAllocatable* Allocatable);

	UFUNCTION(Category = "Structure")
	EHandleType ParseToggleAllocateHex(UHexTile* Hex, bool bUpdate);

	UFUNCTION(Category = "Structure")
	EHandleType ParseAllocateHex(UHexTile* Hex);

	UFUNCTION(Category = "Structure")
	EHandleType ParseUnallocateHex(UHexTile* Hex);

	UFUNCTION(Category = "Structure")
	void UpdateMaxTiles(UAllocationSlot* AllocationSlot, UAllocatable* Allocatable);

	void UpdateGatheredResources();

	void ProcessBaseData();

	UAllocatable* RequestAllocatable(UClass* AllocatableClass, FString AllocatableID);



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gather")
	bool bGatherFromAll;

	TArray<UHexTile*> GatherFrom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gather")
	TArray<FString> GatherResources;

	int MaxGatheredPerCycle;

	FTimerHandle GatherCheckTimer;

	void ValidateRequirements();

	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gather")
	TMap<FString, int> PetalProduction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gather")
	TMap<FString, int> StoredPetals;


	TArray<FST_ResourceQuantity> TotalResourceProduction;

	
	UFUNCTION(BlueprintCallable, Category = "Gather")
	TArray<UHexTile*> GetTogglableHexes();
		 
	UFUNCTION(BlueprintCallable, Category = "Gather")
	TArray<FST_ResourceQuantity> GetTotalPetalProduction();

	UFUNCTION(BlueprintCallable, Category = "Gather")
	TArray<FST_ResourceQuantity> GetTileResourceOutput(UHexTile* Hex);
	//void GetConversion(UHexTile* Hex, UPARAM(ref) TArray<FST_TileConversion>& Conversions);
	
	//UFUNCTION(BlueprintCallable, Category = "Gather")
	//void CalcPetalProduction();

	//void AddPetal(FString PetalId, int Quantity);
	//void CollectPetals();
	//void ConvertPetals();
	bool IsHexWorkable(UHexTile* Hex);

	//int32 CalcAvaiableResources(UDataTable* RecipeTable);


	UPROPERTY(BlueprintAssignable, Category = "Gather|Event")
	FOnProductionChangedDelegate OnProductionChangedDelegate;

	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Production")
	//void OnGathered();
	

	virtual void OnEnterEditMode() override;

	virtual void OnLeaveEditMode() override;

	virtual void Init() override;

	virtual void OnProgressComplete() override;

	virtual void BeginPlay() override;
};
