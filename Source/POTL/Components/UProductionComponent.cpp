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
	if (ParentStructure)
	{
		UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
		if (GameInstance && GameInstance->StorageMap)
		{
			for (auto& Entry : MissingResources)
			{
				int NumOfResourcesFound = 0;
				for (int i = 0; i < Entry.Value; i++)
				{
					UResource* Resource = GameInstance->StorageMap->RequestResource(ParentStructure, Entry.Key);
					if (Resource)
					{
						NumOfResourcesFound++;
						// Trigger transaction that consumes resource and adds wealth
						//Resource->Consume();
					}
					else
					{
						break; // Break quantity loop if no resource found.
					}
				}
				Entry.Value -= NumOfResourcesFound;
			}

			/*
			A.RemoveAll([](const FBla* Ptr) {
				return Ptr == nullptr;
			});
			*/

			for (int32 Index = MissingResources.Num() - 1; Index >= 0; --Index)
			{
				//MissingResources[Index]
				/*
				if (MissingResources[Index] == nullptr)
				{
					const bool bAllowShrinking = false;
					A.RemoveAt(Index, 1, bAllowShrinking);
				}
				*/
			}
			/*
			MissingResources.KeySort([](FString A, FString B) {
				return A.Len()  > B.Len(); // sort strings by length 
			});
			*/

			/*
			MissingResources.ValueSort([](const int& A, const int& B) {
				return A< B; // sort keys in reverse
			});
			*/


			for (auto& Entry : MissingResources)
			{
				if (Entry.Value < 1)
				{
					//MissingResources.Remove()
				}
			}
		}
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
