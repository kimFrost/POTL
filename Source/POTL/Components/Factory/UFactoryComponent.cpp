// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "UObjects/UResource.h"
#include "UObjects/UHexTile.h"
#include "UObjects/UPerson.h"
#include "UObjects/Allocations/UAllocationSlot.h"
#include "UObjects/Allocations/UPersonSlot.h"
#include "UObjects/Allocations/UHexSlot.h"
#include "POTLGameMode.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "POTLUtilFunctionLibrary.h"
#include "Components/UProviderComponent.h"
#include "UFactoryComponent.h"



// Sets default values for this component's properties
UFactoryComponent::UFactoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}
UAllocatable* UFactoryComponent::RequestAllocatable(UClass* AllocatableClass, FString AllocatableID)
{
	if (ParentStructure)
	{
		return ParentStructure->RequestAllocatable(AllocatableClass, AllocatableID);
	}
	return nullptr;
}
void UFactoryComponent::Init()
{
	Super::Init();

}
void UFactoryComponent::BeginPlay()
{
	Super::BeginPlay();

}
