// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/UStructureComponent.h"
#include "UFactoryComponent.generated.h"


class UHexTile;
class UResource;
class UProviderComponent;


//~~ DELEGATES ~~//



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class POTL_API UFactoryComponent : public UStructureComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UFactoryComponent();

	//UResource* GetNearestUnallocatedResource();

	virtual void Init() override;

	virtual void BeginPlay() override;
};
