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
	bIsPrivate = true;
	bAllowFlowIn = false;
	ProcentConstructed = 0.f;
	ConstructionTimeLeft = 5.f;
}



void UConstructionComponent::ValidateRequirements()
{
	// Check missing resources and request them
	if (ParentStructure)
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


void UConstructionComponent::Init()
{
	Super::Init();

	TotalConstructionTime = ParentStructure->StructureBaseData.ConstructionTime;
	ConstructionTimeLeft = TotalConstructionTime;

	MissingResources.Empty();
	if (ParentStructure)
	{
		for (auto& CostEntry : ParentStructure->StructureBaseData.ConstructionCost)
		{
			MissingResources.Add(CostEntry.Id, CostEntry.Amount);
		}
	}

	//~~ Subtract StoredConstructionResources from missingResources ~~//
	for (auto& Resource : StoredConstructionResources)
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


	/*
	if (!ParentStructure || (ParentStructure && !ParentStructure->AttachedTo))
	{
		bIsOn = false;
	}
	*/

	// ValidateRequirements every second
	GetWorld()->GetTimerManager().SetTimer(ValidateCheckTimer, this, &UConstructionComponent::ValidateRequirements, 1.f, true);
}


void UConstructionComponent::BeginPlay()
{
	Super::BeginPlay();

}


void UConstructionComponent::OnTimeUpdate(float Time, float TimeProgressed)
{
	Super::OnTimeUpdate(Time, TimeProgressed);

	if (ProcentConstructed < 100.f)
	{
		if (bIsWorking && ParentStructure)
		{
			ConstructionTimeLeft -= TimeProgressed;
			ProcentConstructed = FMath::Abs(ConstructionTimeLeft - TotalConstructionTime) / TotalConstructionTime * 100; // 5 is construction time from data
			if (ConstructionTimeLeft <= 0)
			{
				ProcentConstructed = 100.f;
				GetWorld()->GetTimerManager().ClearTimer(ValidateCheckTimer);
				//~~ Consume all resources in StoredConstructionResources ~~//
				for (auto& Resource : StoredConstructionResources)
				{
					if (Resource)
					{
						Resource->Consume();
					}
				}
				OnComplete.Broadcast();
			}
		}
	}
	
	// ProgressConstruction though stages
}
