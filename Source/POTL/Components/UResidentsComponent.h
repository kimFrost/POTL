// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "POTLStructure.h"
#include "Components/UStructureComponent.h"
#include "UResidentsComponent.generated.h"


//~~~~~ Delegates/Event dispatcher ~~~~~//


//~~ Forward declarations ~~//


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POTL_API UResidentsComponent : public UStructureComponent
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this component's properties
	UResidentsComponent();



	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	
};
