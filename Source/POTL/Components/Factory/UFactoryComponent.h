// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/UStructureComponent.h"
#include "UFactoryComponent.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class UResource;
class UAllocationSlot;
class UAllocationSetSlot;
class UPersonSlot;
class UHexSlot;
class UProviderComponent;

//~~ DELEGATES ~~//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUFactoryComponentOnProductionChangedDelegate, const TArray<UResource*>&, TotalProduction);



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class POTL_API UFactoryComponent : public UStructureComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UFactoryComponent();

private:

	UProviderComponent* ProviderComponent;

	//UFUNCTION(Category = "Gather")
	//void IncludeResource(UAllocationSlot* AllocationSlot, UAllocatable* Allocatable);

	//UFUNCTION(Category = "Gather")
	//void ExcludeResource(UAllocationSlot* AllocationSlot, UAllocatable* Allocatable);

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Factory")
	FST_Factory BaseData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Factory")
	TArray<UPersonSlot*> AllocatedPersonSlots;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Factory")
	TArray<UAllocationSetSlot*> AllocatedSetSlots;
	//TArray<UResource*> AllocatedResourceSlots;

	//TMap<UPerson*, TArray<UAllocationSetSlot*>>

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory")
	TArray<FST_ResourceConversion> ResourceConversions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Factory")
	TArray<UResource*> ProducedResources;


	UAllocatable* RequestAllocatable(UClass* AllocatableClass, FString AllocatableID);

	UFUNCTION(Category = "Structure")
	void UpdateMaxSlotSets(UAllocationSlot* AllocationSlot, UAllocatable* Allocatable);

	void ProcessBaseData();


	UPROPERTY(BlueprintAssignable, Category = "Gather|Event")
	FUFactoryComponentOnProductionChangedDelegate OnProductionChangedDelegate;

	//UResource* GetNearestUnallocatedResource();

	virtual void Init() override;

	virtual void BeginPlay() override;
};
