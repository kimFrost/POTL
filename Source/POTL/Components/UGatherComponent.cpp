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
	TileConversions = TArray<FST_TileConversion>();
}


/******************** OnGathered *************************/
void UGatherComponent::OnGathered_Implementation()
{
	//MissingResources = RequiredResources;
	ValidateRequirements();
	CalcPetalProduction();
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
TArray<FST_ResourceQuantity> UGatherComponent::GetTotalPetalProduction()
{
	TArray<FST_ResourceQuantity> TotalPetalProduction;
	for (auto& Entry : PetalProduction)
	{
		TotalPetalProduction.Add(FST_ResourceQuantity(Entry.Key, Entry.Value));
	}
	return TotalPetalProduction;
}
TArray<FST_ResourceQuantity> UGatherComponent::GetTileResourceOutput(UHexTile* Hex)
{
	TArray<FST_ResourceQuantity> TileResourceOutput;
	if (Hex)
	{
		for (auto& TileConversion : TileConversions)
		{
			if (Hex->HexTileType == TileConversion.TileTypeId)
			{
				for (auto& Output : TileConversion.PetalsOutput)
				{
					TileResourceOutput.Add(FST_ResourceQuantity(Output.Key, Output.Value));
				}
			}
		}
	}
	return TileResourceOutput;
}
void UGatherComponent::CalcPetalProduction()
{
	PetalProduction.Empty();
	for (auto& Hex : AllocatedHexes)
	{
		if (Hex)
		{
			for (auto& TileConversion : TileConversions)
			{
				if (Hex->HexTileType == TileConversion.TileTypeId)
				{
					for (auto& Output : TileConversion.PetalsOutput)
					{
						if (PetalProduction.Contains(Output.Key))
						{
							PetalProduction[Output.Key] += Output.Value;
						}
						else
						{
							PetalProduction.Add(Output.Key, Output.Value);
						}
					}
				}
			}
		}
	}
	OnProductionChangedDelegate.Broadcast(GetTotalPetalProduction());
}
void UGatherComponent::AddPetal(FString PetalId, int Quantity)
{
	if (StoredPetals.Contains(PetalId))
	{
		StoredPetals[PetalId] += Quantity;
	}
	else
	{
		StoredPetals.Add(PetalId, Quantity);
	}
}
void UGatherComponent::CollectPetals()
{
	bool AnyCollected = false;

	for (auto& Entry : PetalProduction)
	{
		AddPetal(Entry.Key, Entry.Value);
	}
}
void UGatherComponent::ConvertPetals()
{
	UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
	if (GameInstance && ParentStructure)
	{
		for (auto& Entry : StoredPetals)
		{
			//TODO: Lookup conversion rates for Entry.Key
			int ConversionRate = 3;
			if (Entry.Value >= ConversionRate)
			{
				int NumOfCombined = FPlatformMath::FloorToInt(Entry.Value / ConversionRate);
				if (NumOfCombined > 0)
				{
					Entry.Value = Entry.Value - (NumOfCombined * ConversionRate);
					for (int i = 0; i < NumOfCombined; i++)
					{
						UResource* Resource = GameInstance->CreateResource(Entry.Key);
						if (Resource)
						{
							ParentStructure->StoreResource(Resource);
						}
					}
				}
			}
		}
		// Clean tmap
		for (auto& Entry : StoredPetals)
		{
			if (Entry.Value == 0)
			{
				StoredPetals.Remove(Entry.Key);
			}
		}
	}
}
bool UGatherComponent::IsHexWorkable(UHexTile* Hex)
{
	if (Hex)
	{
		for (auto& TileConversion : TileConversions)
		{
			if (Hex->HexTileType == TileConversion.TileTypeId)
			{
				return true;
			}
		}
	}
	return false;
}
EHandleType UGatherComponent::ParseAllocateHex(UHexTile* Hex)
{
	// Get Required labor or other static resources
	if (Hex && ParentStructure)
	{
		// Request Labor
		for (auto& TileConversion : TileConversions)
		{
			if (Hex->HexTileType == TileConversion.TileTypeId)
			{
				if (ParentStructure->RequestLabor(TileConversion.LaborRequired))
				{
					return EHandleType::Handled;
				}
				else
				{
					UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
					if (GameInstance)
					{
						GameInstance->ShowFeedbackMsg(TEXT("Not enough labor points"), EMessageType::Common, Hex->Location);
					}
					return EHandleType::HandledBreak;
				}
			}
		}
	}
	return EHandleType::Unhandled;
}
EHandleType UGatherComponent::ParseUnallocateHex(UHexTile* Hex)
{
	if (Hex && ParentStructure)
	{
		for (auto& TileConversion : TileConversions)
		{
			if (Hex->HexTileType == TileConversion.TileTypeId)
			{
				if (ParentStructure->StoreLabor(TileConversion.LaborRequired))
				{
					return EHandleType::Handled;
				}
				else
				{
					UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
					if (GameInstance)
					{
						GameInstance->ShowFeedbackMsg(TEXT("Unable to return labor points"), EMessageType::Warning, Hex->Location);
					}
					return EHandleType::HandledBreak;
				}
			}
		}
	}
	return EHandleType();
}


/******************** OnProgressComplete *************************/
void UGatherComponent::OnProgressComplete()
{
	Super::OnProgressComplete();

	OnGathered();
	
	CollectPetals();
	ConvertPetals();

	/*
	if (ParentStructure)
	{
		UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
		if (GameInstance)
		{
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
	*/
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

	if (ParentStructure)
	{
		//~~ Bind to allocate hex in parent structure ~~//
		FOnHexAllocateDelegate* AllocateDelegate = ParentStructure->BindToOnHexAllocate(this, 0);
		if (AllocateDelegate)
		{
			AllocateDelegate->BindUObject(this, &UGatherComponent::ParseAllocateHex);
		}
		//~~ Bind to unallocate hex in parent structure ~~//
		FOnHexUnallocateDelegate* UnallocateDelegate = ParentStructure->BindToOnHexUnallocate(this, 0);
		if (UnallocateDelegate)
		{
			UnallocateDelegate->BindUObject(this, &UGatherComponent::ParseUnallocateHex);
		}
	}

	CalcPetalProduction();

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
