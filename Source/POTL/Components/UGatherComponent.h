// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UStructureTaskComponent.h"
#include "UObjects/UResource.h"
#include "UObjects/UHexTile.h"
#include "UGatherComponent.generated.h"



//~~ DELEGATES ~~//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProduction, float, Amount);



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class POTL_API UGatherComponent : public UStructureTaskComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UGatherComponent();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gather")
	bool bGatherFromAll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gather")
	int GatherRange;

	TArray<UHexTile*> GatherFrom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Production")
	TArray<FString> GatherResources;

	int MaxGatheredPerCycle;

	FTimerHandle GatherCheckTimer;

	void ValidateRequirements();

	//int32 CalcAvaiableResources(UDataTable* RecipeTable);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Production")
	void OnGathered();



	virtual void Init() override;

	virtual void OnProgressComplete() override;

	virtual void BeginPlay() override;
};
