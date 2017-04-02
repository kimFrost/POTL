// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UStorageComponent.h"
#include "UObjects/UResource.h"
#include "UObjects/UHexTile.h"
#include "UConstructionComponent.generated.h"



//~~ DELEGATES ~~//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStageSwitch, int, StageIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnComplete);



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class POTL_API UConstructionComponent : public UStorageComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UConstructionComponent();
	
	// Steps/Stages for meshes. Like catherals in anno

	// Stored resources for construction

	// StructureDataOnComplete

	// MissingResources

	// StoredResources

	bool bConstructionComponentInitialized; 

	bool bIsCompleted;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Construction")
	float ProcentConstructed;

	float TotalConstructionTime;

	float ConstructionTimeLeft;

	//TArray<UResource*> StoredConstructionResources; Using StoredResources instead

	TMap<FString, int> MissingResources;

	FTimerHandle ValidateCheckTimer;

	void ValidateRequirements();

	void CompleteConstruction();


	UPROPERTY(BlueprintAssignable, Category = "Construction|Event")
	FOnComplete OnComplete;


	virtual void Init() override;

	virtual void BeginPlay() override;

	virtual void OnTimeUpdate(float Time, float TimeProgressed) override;
};
