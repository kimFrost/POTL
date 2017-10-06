// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "FactoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POTL_API UFactoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFactoryComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	FString Recipe;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 MaxQuantity;

	UPROPERTY(EditAnywhere, Category = "Resource")
	TMap<FString, int32> Requirements;

	UPROPERTY(EditAnywhere, Category = "Resource")
	TMap<FString, int32> Invoice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 AllocationIndex;

	//~~ Calculate Requirements for total requirements ~~//
	//UFUNCTION(BlueprintCallable, Category = "Resources")
	int32 ProcessInvoice(UDataTable* RecipeTable);

	//~~ Resolve factory ~~//
	//UFUNCTION(BlueprintCallable, Category = "Resources")
	void Resolve(APOTLStructure* Caller, TMap<FString, int32>& FreeResources, UDataTable* RecipeTable, TMap<FString, int32>& Production, TMap<FString, int32>& Billing);


	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	
};
