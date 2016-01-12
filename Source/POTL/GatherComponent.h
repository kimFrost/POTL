// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "GatherComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POTL_API UGatherComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGatherComponent();

	UPROPERTY(EditAnywhere, Category = "Resource")
	FString GatherRecipe;

	UPROPERTY(EditAnywhere, Category = "Resource")
	TArray<int32> GatherFromIndexes;


	//UFUNCTION(BlueprintCallable, Category = "Resources")
	int32 CalcAvaiableResources(UDataTable* RecipeTable);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void Resolve(APOTLStructure* Caller);


	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;


	
};
