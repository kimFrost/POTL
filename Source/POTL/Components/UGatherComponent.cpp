// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "UObjects/UResource.h"
#include "UObjects/UHexTile.h"
#include "POTLGameMode.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "POTLUtilFunctionLibrary.h"
#include "UGatherComponent.h"



// Sets default values for this component's properties
UGatherComponent::UGatherComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	GatherRange = 3;
	TaskLength = 5.f;
	MaxGatheredPerCycle = 1;
}


/******************** OnGathered *************************/
void UGatherComponent::OnGathered_Implementation()
{
	//MissingResources = RequiredResources;
	ValidateRequirements();
}


void UGatherComponent::ValidateRequirements()
{
	//TODO: Better validation Logic for attachedTo

	//if (ParentStructure && ParentStructure->AttachedTo)
	if (ParentStructure)
	{
		bIsWorking = true;
		/*
		bool anyResourceInRange = false;
		for (auto& Hex : HexesInRange)
		{
			if (Hex)
			{
				
			}
		}
		bIsWorking = anyResourceInRange;
		*/
		/*
		UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
		if (GameInstance)
		{
			
		}
		*/
	}
	else
	{
		bIsWorking = false;
	}
}


/******************** OnProgressComplete *************************/
void UGatherComponent::OnProgressComplete()
{
	Super::OnProgressComplete();

	OnGathered();
	
	if (ParentStructure)
	{
		UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
		if (GameInstance)
		{
			// Gather from hexes
			//ParentStructure->HexesInRange

			// Or from allocated hexes with picker

			bool AnyGathered = false; 
			for (auto& Hex : AllocatedHexes) //GatherFrom
			{
				if (Hex && Hex->Resources.Num() > 0)
				{
					//int RandIndex = FMath::RandRange(0, GatherResources.Num() - 1);
					//FString ResourceId = GatherResources[RandIndex];

					// Shuffle resources to gather
					TArray<FString> GatherResourcesShuffled = GatherResources;
					GatherResourcesShuffled.Sort([this](const FString Item1, const FString Item2) {
						return FMath::FRand() < 0.5f;
					});
					for (auto& ResourceId : GatherResourcesShuffled)
					{
						if (Hex->Resources.Contains(ResourceId))
						{
							UResource* Resource = GameInstance->CreateResource(ResourceId);
							if (Resource)
							{
								ParentStructure->StoreResource(Resource);
								AnyGathered = true;
								break;
							}
						}
					}
					if (AnyGathered)
					{
						break;
					}
				}
			}
			if (!AnyGathered)
			{
				// No resource found on hexes in range

			}
		}
	}
}


void UGatherComponent::Init()
{
	Super::Init();

	//if (!ParentStructure || (ParentStructure && !ParentStructure->AttachedTo))
	if (!ParentStructure)
	{
		bIsOn = false;
	}
	if (bGatherFromAll)
	{
		GatherFrom = HexesInRange;
	}
	else
	{
		GatherFrom = AllocatedHexes;
	}
	/*
	UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
	if (GameInstance)
	{
		if (GatherRange > 0)
		{
			TArray<FVector> Cubes = UPOTLUtilFunctionLibrary::GetCubesInRange(ParentStructure->CubeCoord, GatherRange, false);
			for (int32 i = 0; i < Cubes.Num(); i++)
			{
				FVector2D OffsetCoords = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(Cubes[i]);
				int32 HexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(OffsetCoords, GameInstance->GridXCount);
				if (GameInstance->Hexes.IsValidIndex(HexIndex))
				{
					GatherFrom.Add(GameInstance->Hexes[HexIndex]);
				}
			}
		}
	}
	*/

	// ValidateRequirements every second
	GetWorld()->GetTimerManager().SetTimer(GatherCheckTimer, this, &UGatherComponent::ValidateRequirements, 1.f, true);
}


void UGatherComponent::BeginPlay()
{
	Super::BeginPlay();

}
