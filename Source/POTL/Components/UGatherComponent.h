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
	int GatherRange;

	TArray<UHexTile*> HexesInRange;



	// Production storage. Storage on required resources x 1 ?

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Production")
	TMap<FString, int> MissingResources;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Production")
	TMap<FString, int> RequiredResources;

	// Production Item and quantity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Production")
	TMap<FString, int> Production;

	FTimerHandle ProductionCheckTimer;

	void CheckProduction();

	//int32 CalcAvaiableResources(UDataTable* RecipeTable);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Production")
	void OnProduction();



	virtual void Init() override;

	virtual void OnProgressComplete() override;

	virtual void BeginPlay() override;
};
