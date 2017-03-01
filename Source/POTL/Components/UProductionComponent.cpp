// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "UObjects/UResource.h"
#include "POTLGameMode.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "UProductionComponent.h"



// Sets default values for this component's properties
UProductionComponent::UProductionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//TaskLength = 5.f;

	/*
	StorageCapacity = 0.f;
	PowerUpkeep = 0.f;
	PowerGenerated = 0.f;

	StoredPower = 0.f;
	SurplusPower = 0.f;
	*/
}


/******************** OnProduction *************************/
void UProductionComponent::OnProduction_Implementation()
{
	MissingResources = RequiredResources;
	CheckProduction();
}


void UProductionComponent::CheckProduction()
{
	UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
	if (GameInstance)
	{

	}

	// Fetch resource with transaction
	// Subtract for missing resource. When empty, start production.

	// Transactions of resources.

	if (MissingResources.Num() > 0) {
		bIsOn = false;
	}
}


/******************** OnProgressComplete *************************/
void UProductionComponent::OnProgressComplete()
{
	Super::OnProgressComplete();

	OnProduction();

	//UE_LOG(LogTemp, Log, TEXT("ULaborComponent::OnProgressComplete"));
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("UProductionComponent::OnProgressComplete"));

	if (IsValid(ParentStructure))
	{
		// Add resource to storage
		for (auto& ProductionItem : Production)
		{
			for (int i = 0; i < ProductionItem.Value; i++)
			{
				UResource* ProducedResource = NewObject<UResource>();
				ProducedResource->ResourceId = ProductionItem.Key;
				ParentStructure->StoreResource(ProducedResource);
			}
			//ParentStructure->AddResource(ProductionItem.Key, ProductionItem.Value);
		}

		if (ParentStructure->AttachedTo)
		{

		}
	}
}

void UProductionComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(ProductionCheckTimer, this, &UProductionComponent::CheckProduction, 1.f, true);
}
