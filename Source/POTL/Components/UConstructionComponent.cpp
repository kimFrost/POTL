// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "UObjects/UResource.h"
#include "UObjects/UHexTile.h"
#include "POTLGameMode.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "POTLUtilFunctionLibrary.h"
#include "UConstructionComponent.h"



// Sets default values for this component's properties
UConstructionComponent::UConstructionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	IncludeInStorageMap = false;
}



void UConstructionComponent::ValidateRequirements()
{
	//TODO: Better validation Logic for attachedTo
	/*
	if (!ParentStructure)
	{
		bIsWorking = false;
	}
	else if (bRequireAttached)
	{
		if (ParentStructure && ParentStructure->AttachedTo)
		{
			bIsWorking = true;
		}
		else
		{
			bIsWorking = false;
		}
	}
	*/
}



void UConstructionComponent::Init()
{
	Super::Init();

	/*
	if (!ParentStructure || (ParentStructure && !ParentStructure->AttachedTo))
	{
		bIsOn = false;
	}
	*/

	// ValidateRequirements every second
	//GetWorld()->GetTimerManager().SetTimer(ProvideCheckTimer, this, &UConstructionComponent::ValidateRequirements, 1.f, true);
}


void UConstructionComponent::BeginPlay()
{
	Super::BeginPlay();

}
