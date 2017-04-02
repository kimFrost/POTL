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
	IncludeInStorageMap = true;
	bIsCompleted = false;
	bIsPrivate = true;
	bAllowFlowIn = false;
	bAllowFlowOut = false;
	ProcentConstructed = 0.f;
	ConstructionTimeLeft = 5.f;
}



void UConstructionComponent::ValidateRequirements()
{
	// Check missing resources and request them
	if (ParentStructure && !bIsCompleted)
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
						GameInstance->TransferResource(Resource, this, false, false);
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


void UConstructionComponent::CompleteConstruction()
{
	bIsCompleted = true;
	ConstructionTimeLeft = 0;
	ProcentConstructed = 100.f;
	GetWorld()->GetTimerManager().ClearTimer(ValidateCheckTimer);
	//~~ Consume all resources in StoredConstructionResources ~~//
	//for (auto& Resource : StoredConstructionResources)
	for (auto& Resource : StoredResources)
	{
		if (Resource)
		{
			Resource->Consume();
		}
	}
	StoredResources.Empty();
	OnComplete.Broadcast();
}


void UConstructionComponent::Init()
{
	Super::Init();

	if (ParentStructure && !bConstructionComponentInitialized)
	{
		TotalConstructionTime = ParentStructure->StructureBaseData.ConstructionTime;
		ConstructionTimeLeft = TotalConstructionTime;

		MissingResources.Empty();

		for (auto& CostEntry : ParentStructure->StructureBaseData.ConstructionCost)
		{
			MissingResources.Add(CostEntry.Id, CostEntry.Amount);
		}

		//~~ Subtract StoredConstructionResources from missingResources ~~//
		for (auto& Resource : StoredResources)
		{
			if (Resource)
			{
				if (MissingResources.Contains(Resource->ResourceId))
				{
					MissingResources[Resource->ResourceId]--;
				}
			}
		}
		for (auto& Entry : MissingResources)
		{
			if (Entry.Value < 1)
			{
				MissingResources.Remove(Entry.Key);
			}
		}
		MissingResources.Compact(); // Remove invalid entries if any

		bConstructionComponentInitialized = true; //!! Must come before CompleteConstruction() Else infinity loop !!//

		if (ConstructionTimeLeft > 0)
		{
			// ValidateRequirements every second
			UWorld* World = GetWorld();
			if (World)
			{
				World->GetTimerManager().SetTimer(ValidateCheckTimer, this, &UConstructionComponent::ValidateRequirements, 1.f, true);
			}
		}
		else
		{
			CompleteConstruction();
		}

		/*
		if (!ParentStructure || (ParentStructure && !ParentStructure->AttachedTo))
		{
			bIsOn = false;
		}
		*/

	}
}


void UConstructionComponent::BeginPlay()
{
	Super::BeginPlay();

	//Deactivate();
	//Activate(false);
	//bIsActive
}


void UConstructionComponent::OnTimeUpdate(float Time, float TimeProgressed)
{
	Super::OnTimeUpdate(Time, TimeProgressed);
	
	if (!bIsCompleted)
	{
		if (bIsWorking && ParentStructure)
		{
			ConstructionTimeLeft -= TimeProgressed;
			ProcentConstructed = FMath::Abs(ConstructionTimeLeft - TotalConstructionTime) / TotalConstructionTime * 100; // 5 is construction time from data
			if (ConstructionTimeLeft <= 0)
			{
				CompleteConstruction();				
			}
		}
	}
	
	// ProgressConstruction though stages
}
