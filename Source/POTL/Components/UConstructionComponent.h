// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UStorageComponent.h"
#include "UObjects/UResource.h"
#include "UObjects/UHexTile.h"
#include "UConstructionComponent.generated.h"



//~~ DELEGATES ~~//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProduction, float, Amount);



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class POTL_API UConstructionComponent : public UStorageComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UConstructionComponent();
	
	// Steps/Stages for meshes. Like catherals in anno

	// Stored resources for construction

	// StructureData

	// MissingResources

	// StoredResources


	FTimerHandle ProvideCheckTimer;

	void ValidateRequirements();


	virtual void Init() override;

	virtual void BeginPlay() override;
};
