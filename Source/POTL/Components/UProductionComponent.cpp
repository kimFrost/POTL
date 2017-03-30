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
	PrimaryComponentTick.bCanEverTick = false;

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
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("UProductionComponent::CheckProduction"));

	//TODO: Better validation Logic for attachedTo

	if (ParentStructure && ParentStructure->AttachedTo)
	{
		UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
		if (GameInstance)
		{
			for (auto& Entry : MissingResources)
			{
				int NumOfResourcesFound = 0;
				for (int i = 0; i < Entry.Value; i++)
				{
					UResource* Resource = GameInstance->RequestResource(ParentStructure, Entry.Key);
					if (Resource)
					{
						NumOfResourcesFound++;
						// Trigger transaction that consumes resource and adds wealth
						GameInstance->TransferResource(Resource, this, true, false);
					}
					else
					{
						break; // Break quantity loop if no resource found.
					}
				}
				Entry.Value -= NumOfResourcesFound;
			}
			for (auto& Entry : MissingResources)
			{
				if (Entry.Value < 1)
				{
					MissingResources.Remove(Entry.Key);
				}
			}
			MissingResources.Compact(); // Remove invalid entries if any
		}

		if (MissingResources.Num() > 0) {
			bIsWorking = false;
		}
		else {
			bIsWorking = true;
		}
	}
	else
	{
		bIsWorking = false;
	}
}


/******************** OnProgressComplete *************************/
void UProductionComponent::OnProgressComplete()
{
	Super::OnProgressComplete();

	OnProduction();

	//UE_LOG(LogTemp, Log, TEXT("ULaborComponent::OnProgressComplete"));
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("UProductionComponent::OnProgressComplete"));

	
	if (ParentStructure)
	{
		UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
		if (GameInstance)
		{
			// Add resource to storage
			for (auto& ProductionItem : Production)
			{
				for (int i = 0; i < ProductionItem.Value; i++)
				{
					UResource* ProducedResource = GameInstance->CreateResource(ProductionItem.Key);
					if (ProducedResource)
					{
						ParentStructure->StoreResource(ProducedResource);
					}
				}
				//ParentStructure->AddResource(ProductionItem.Key, ProductionItem.Value);
			}

			if (ParentStructure->AttachedTo)
			{

			}
		}
	}
}


void UProductionComponent::Init()
{
	Super::Init();

	if (!ParentStructure || (ParentStructure && !ParentStructure->AttachedTo))
	{
		bIsOn = false;
	}
	// CheckProduction every second
	GetWorld()->GetTimerManager().SetTimer(ProductionCheckTimer, this, &UProductionComponent::CheckProduction, 1.f, true);
}


void UProductionComponent::BeginPlay()
{
	Super::BeginPlay();

}
