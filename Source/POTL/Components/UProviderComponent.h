// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UStructureComponent.h"
#include "UObjects/UHexTile.h"
#include "UProviderComponent.generated.h"



class UResource;


//~~ DELEGATES ~~//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProduction, float, Amount);



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class POTL_API UProviderComponent : public UStructureComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UProviderComponent();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Provider")
	int ProviderRange;

	TArray<UHexTile*> HexesInRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Provider")
	TArray<UResource*> Resources;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Provider")
	TMap<FString, int> Provides;

	//TMap<FString, TArray<UResource*>> Provides;


	FTimerHandle ProvideCheckTimer;

	void ToggleProvider();
	void ActivateProvider();
	void DeactivateProvider();

	void ValidateRequirements();

	UFUNCTION(BlueprintCallable, Category = "Provider")
	void SetProduction(const TArray<UResource*>& ResourceList);
	//void SetProduction(const TArray<FST_ResourceQuantity>& ResourceList);


	virtual void ActivateComponent() override;

	virtual void DeactivateComponent() override;

	virtual void Init() override;

	virtual void BeginPlay() override;
};
