// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UObjects/Allocations/UAllocatable.h"
#include "UAllocationSetSlot.generated.h"



DECLARE_DELEGATE_RetVal_TwoParams(UAllocatable*, FRequestAllocatable, UClass*, FString);
//DECLARE_DELEGATE_RetVal_TwoParams(UAllocatable*, FUAllocationSetSlotRequestAllocatableSet, TMap<UClass*, FString>);



UCLASS(Blueprintable, BlueprintType)
class POTL_API UAllocationSetSlot : public UObject
{
	GENERATED_BODY()

public:
	UAllocationSetSlot();
	~UAllocationSetSlot();

	//TSet<FString> FruitSet;

	//TPair
	//TPair<int32, FVector>& OutPair = OutData.Last();
	//OutPair.Key = MeshIndices[InfluencedTriangle * 3 + Index];
	//OutPair.Value = MeshVertices[OutPair.Key];

	//TMap<UClass*, UAllocationSlot*>
	//TMap<FString, UAllocationSlot*>

private:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Allocation")
	TMap<UClass*, FString> SlotEntries;

	void AllocatableUnallocateToSlot(UAllocationSlot* AllocationSlot, UAllocatable* Allocatable);
	void AllocatableAllocateToSlot(UAllocationSlot* AllocationSlot, UAllocatable* Allocatable);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Allocation")
	TArray<UAllocationSlot*> AllocationSlots;

	void Construct(TMap<UClass*, FString> Slots);

	UFUNCTION(BlueprintCallable, Category = "Allocation")
	void RequestAllocations();

	FRequestAllocatable OnRequestAllocatable;
};



// RequestResourceSet