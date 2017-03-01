// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/UStructureComponent.h"
#include "UObjects/UResource.h"
#include "UProductionComponent.generated.h"



//~~ DELEGATES ~~//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProduction, float, Amount);



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class POTL_API UProductionComponent : public UStructureComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UProductionComponent();

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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Production")
	void OnProduction();

	virtual void OnProgressComplete() override;

	virtual void BeginPlay() override;
};
