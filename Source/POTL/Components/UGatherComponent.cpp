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

	// Construct provider component

	/*
	ProviderComponent = CreateDefaultSubobject<UProviderComponent>(TEXT("ProviderComponent"));
	if (ProviderComponent)
	{
		//ProviderComponent->AttachTo(Rootcomponent);
		//ProviderComponent->AttachParent
		//ProviderComponent->AttachToComponent(this);
		//AttachToComponent
		//ProviderComponent->SetupAttachment()
		//Mesh->SetupAttachment(RootComponent);
		//AddOwnedComponent(ProviderComponent);
	}
	*/

}

/*
void UGatherComponent::OnGathered_Implementation()
{
	//MissingResources = RequiredResources;
	ValidateRequirements();
	CalcPetalProduction();
}
*/
void UGatherComponent::ValidateRequirements()
{
	//TODO: Better validation Logic for attachedTo

	//if (ParentStructure && ParentStructure->AttachedTo)
	if (ParentStructure)
	{
		bIsWorking = true;
	}
	else
	{
		bIsWorking = false;
	}
}
TArray<UHexTile*> UGatherComponent::GetTogglableHexes()
{
	TArray<UHexTile*> TogglableHexes = TArray<UHexTile*>();
	if (ParentStructure)
	{
		for (auto& Hex : ParentStructure->HexesInRange)
		{
			if (Hex)
			{
				if (Hex->AllocatedTo && !AllocatedTileSlots.Contains(Hex->AllocatedTo))
				{
					continue;
				}
				if (Hex->AttachedBuilding)
				{
					continue;
				}
				if (!IsHexWorkable(Hex)) {
					continue;
				}
				TogglableHexes.Add(Hex);
			}
		}
	}
	return TogglableHexes;
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
			if (Hex->HexTileType == TileConversion.TileTypeID)
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
/*
void UGatherComponent::CalcPetalProduction()
{
	PetalProduction.Empty();
	if (ParentStructure)
	{
		//for (auto& Hex : AllocatedHexes)
		for (auto& Hex : ParentStructure->AllocatedHexes)
		{
			if (Hex)
			{
				for (auto& TileConversion : TileConversions)
				{
					if (Hex->HexTileType == TileConversion.TileTypeID)
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
	}

	TMap<FString, int32> ResourceProductionMap;
	for (auto& Entry : PetalProduction)
	{
		if (Entry.Value >= 5)
		{
			if (ResourceProductionMap.Contains(Entry.Key))
			{
				ResourceProductionMap[Entry.Key] += FMath::FloorToInt(Entry.Value / 5);
			}
			else
			{
				ResourceProductionMap.Add(Entry.Key, FMath::FloorToInt(Entry.Value / 5));
			}
		}
	}


	// Sort so free resources are first in array. !!Not tested yet!!
	GatheredResources.Sort([this](const UResource& Resource1, const UResource& Resource2) {
		return (Resource1.AllocatedTo == nullptr);
	});

	// Validate all stored UResources up against ResourceProductionMap 
	for (int i = GatheredResources.Num() - 1; i >= 0; i--)
	{
		UResource* Resource = GatheredResources[i];
		if (Resource)
		{
			if (ResourceProductionMap.Contains(Resource->ResourceId) && ResourceProductionMap[Resource->ResourceId] >= 1)
			{
				ResourceProductionMap[Resource->ResourceId] -= 1;
				if (ResourceProductionMap[Resource->ResourceId] < 1)
				{
					ResourceProductionMap.Remove(Resource->ResourceId);
				}
			}
			else {
				Resource->Unallocate();
				GatheredResources.RemoveAt(i);
				Resource->Consume(EConsumeType::Undefined);
			}
		}
		else
		{
			GatheredResources.RemoveAt(i);
		}
	}

	// If any left in array, then create new resources
	UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
	if (GameInstance)
	{
		for (auto& Entry : ResourceProductionMap)
		{
			for (int32 i = 0; i < Entry.Value; i++)
			{
				UResource* Resource = GameInstance->CreateResource(Entry.Key);
				if (Resource)
				{
					Resource->Owner = ParentStructure;
					GatheredResources.Add(Resource);
				}
			}
		}
	}

	OnProductionChangedDelegate.Broadcast(GatheredResources);
}
*/
/*
void UGatherComponent::AddPetal(FString PetalId, int32 Quantity)
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
*/
/*
void UGatherComponent::CollectPetals()
{
	bool AnyCollected = false;
	for (auto& Entry : PetalProduction)
	{
		AddPetal(Entry.Key, Entry.Value);
	}
}
*/
/*
void UGatherComponent::ConvertPetals()
{
	UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
	if (GameInstance && ParentStructure)
	{
		for (auto& Entry : StoredPetals)
		{
			//TODO: Lookup conversion rates for Entry.Key
			int32 ConversionRate = 5;
			if (Entry.Value >= ConversionRate)
			{
				int32 NumOfCombined = FPlatformMath::FloorToInt(Entry.Value / ConversionRate);
				if (NumOfCombined > 0)
				{
					Entry.Value = Entry.Value - (NumOfCombined * ConversionRate);
					for (int32 i = 0; i < NumOfCombined; i++)
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
*/
bool UGatherComponent::IsHexWorkable(UHexTile* Hex)
{
	if (Hex)
	{
		for (auto& TileConversion : TileConversions)
		{
			if (Hex->HexTileType == TileConversion.TileTypeID)
			{
				return true;
			}
		}
	}
	return false;
}
void UGatherComponent::IncludeHex(UAllocationSlot* AllocationSlot, UAllocatable* Allocatable)
{
	UHexTile* Hex = Cast<UHexTile>(Allocatable);
	if (Hex && !GatheredResourcesByHex.Contains(Hex))
	{
		// Add resources from hex to gatheredResources
		UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
		if (GameInstance)
		{
			for (auto& TileConversion : TileConversions)
			{
				if (Hex->HexTileType == TileConversion.TileTypeID)
				{
					TArray<UResource*> Resources = TArray<UResource*>();
					for (auto& Output : TileConversion.PetalsOutput)
					{
						for (int32 i = 0; i < Output.Value; i++)
						{
							UResource* Resource = GameInstance->CreateResource(Output.Key);
							if (Resource)
							{
								Resources.Add(Resource);
							}
						}
					}
					GatheredResourcesByHex.Add(Hex, Resources);
				}
			}
		}
		UpdateGatheredResources();
	}
}
void UGatherComponent::ExcludeHex(UAllocationSlot* AllocationSlot, UAllocatable* Allocatable)
{
	UHexTile* Hex = Cast<UHexTile>(Allocatable);
	if (Hex && GatheredResourcesByHex.Contains(Hex))
	{
		TArray<UResource*>& Resources = GatheredResourcesByHex[Hex];
		for (auto& Resource : Resources)
		{
			if (Resource)
			{
				Resource->Unallocate();
			}
		}
		GatheredResourcesByHex.Remove(Hex);
		UpdateGatheredResources();
	}
}
EHandleType UGatherComponent::ParseToggleAllocateHex(UHexTile* Hex, bool bUpdate)
{
	if (Hex)
	{
		//?? This seem like its the wrong way to do it. Might be some more dynamic way??
		if (Hex->AllocatedTo && AllocatedTileSlots.Contains(Hex->AllocatedTo))
		{
			return ParseUnallocateHex(Hex);
		}
		else if (!Hex->AllocatedTo)
		{
			return ParseAllocateHex(Hex);
		}
		/*
		bool bChanged = false;
		if (Hex->AllocatedTo == this)
		{
			bChanged = UnallocateHex(Hex);
		}
		else
		{
			bChanged = AllocateHex(Hex);
		}

		if (bChanged)
		{
			DrawInRangeInfo();

			OnAllocatedHexesChangedDelegate.Broadcast();
			OnAllocatedHexesChanged();
		}
		*/
	}

	return EHandleType::Unhandled;
}
EHandleType UGatherComponent::ParseAllocateHex(UHexTile* Hex)
{
	if (Hex)
	{
		for (auto& TileConversion : TileConversions)
		{
			if (Hex->HexTileType == TileConversion.TileTypeID)
			{
				for (auto& HexSlot : AllocatedTileSlots)
				{
					if (HexSlot)
					{
						if (HexSlot->Allocated == nullptr)
						{
							HexSlot->Allocate(Hex);
							return EHandleType::Handled;
						}
					}
				}
			}
		}
	}
	return EHandleType::HandledBreak;

	// Get Required labor or other static resources
	if (Hex && ParentStructure)
	{
		// Request Labor
		for (auto& TileConversion : TileConversions)
		{
			if (Hex->HexTileType == TileConversion.TileTypeID)
			{
				// Move logic to here to allocate hex

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
		UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
		if (GameInstance)
		{
			GameInstance->ShowFeedbackMsg(TEXT("No TileConversion found"), EMessageType::Common, Hex->Location);
		}
	}
	return EHandleType::Unhandled;
}
EHandleType UGatherComponent::ParseUnallocateHex(UHexTile* Hex)
{
	if (Hex)
	{
		for (auto& HexSlot : AllocatedTileSlots)
		{
			if (HexSlot)
			{
				if (HexSlot->Allocated == Hex)
				{
					HexSlot->Unallocate();
					return EHandleType::Handled;
				}
			}
		}
	}
	return EHandleType::Unhandled;

	if (Hex && ParentStructure)
	{
		for (auto& TileConversion : TileConversions)
		{
			if (Hex->HexTileType == TileConversion.TileTypeID)
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
	return EHandleType::Unhandled;
}



/******************** OnProgressComplete *************************/
void UGatherComponent::OnProgressComplete()
{
	Super::OnProgressComplete();

	/*
	OnGathered();
	
	CollectPetals();
	ConvertPetals();
	*/
}

void UGatherComponent::UpdateMaxTiles(UAllocationSlot* AllocationSlot, UAllocatable* Allocatable)
{
	int32 MaxNumberOfTiles = 0;
	for (auto& Slot : AllocatedPersonSlots)
	{
		if (Slot && Slot->Allocated)
		{
			UPerson* Person = Cast<UPerson>(Slot->Allocated);
			if (Person)
			{
				MaxNumberOfTiles += 3;
			}
		}
	}
	int32 DiffCount = MaxNumberOfTiles - AllocatedTileSlots.Num();
	if (DiffCount > 0)
	{
		// Add Tile slots
		for (int32 i = 0; i < DiffCount; i++)
		{
			UHexSlot* HexSlot = NewObject<UHexSlot>(this);
			if (HexSlot)
			{
				HexSlot->OnAllocatedDelegate.AddDynamic(this, &UGatherComponent::IncludeHex);
				HexSlot->OnUnallocatedDelegate.AddDynamic(this, &UGatherComponent::ExcludeHex);
				AllocatedTileSlots.Add(HexSlot);
			}
		}
	}
	else if (DiffCount < 0)
	{
		// Remove Tile slots
		for (int32 i = DiffCount; i < 0; i++)
		{
			UHexSlot* LastHexSlot = AllocatedTileSlots[AllocatedTileSlots.Num() - 1];
			if (LastHexSlot)
			{
				LastHexSlot->Unallocate();
				LastHexSlot->OnAllocatedDelegate.RemoveDynamic(this, &UGatherComponent::IncludeHex);
				LastHexSlot->OnUnallocatedDelegate.RemoveDynamic(this, &UGatherComponent::ExcludeHex);
				AllocatedTileSlots.RemoveAt(AllocatedTileSlots.Num() - 1);
				// Delete LastHexSlot ??
			}
		}
		/*
		for (int i = GatheredResources.Num() - 1; i >= 0; i--)
		{}
		*/
	}

	// Sort. Put free slots last
	AllocatedPersonSlots.Sort([this](const UPersonSlot& PersonSlot1, const UPersonSlot& PersonSlot2) {
		return (PersonSlot1.Allocated != nullptr);
	});
	AllocatedTileSlots.Sort([this](const UHexSlot& HexSlot1, const UHexSlot& HexSlot2) {
		return (HexSlot1.Allocated != nullptr);
	});
}

void UGatherComponent::UpdateGatheredResources()
{
	GatheredResources.Empty();
	for (auto& Entry : GatheredResourcesByHex)
	{
		if (Entry.Key)
		{
			GatheredResources.Append(Entry.Value);
		}
	}
	// Sort array by free first
	GatheredResources.Sort([this](const UResource& Resource1, const UResource& Resource2) {
		return (Resource1.AllocatedTo == nullptr);
	});
	OnProductionChangedDelegate.Broadcast(GatheredResources);
}

void UGatherComponent::ProcessBaseData()
{
	for (auto& Entry : BaseData.AllocationSlots)
	{
		for (int32 i = 0; i < Entry.Value; i++)
		{
			UPersonSlot* PersonSlot = NewObject<UPersonSlot>(this);
			//UAllocationSlot* PersonSlot = NewObject<UAllocationSlot>(this);
			if (PersonSlot)
			{
				PersonSlot->AllowedAllocationClass = UPerson::StaticClass();
				PersonSlot->AllowedAllocationID = TEXT("Laborer");
				//~~ Bind RequestAllocatable on slot to RequestAllocatable for processing ~~//
				PersonSlot->OnRequestAllocatable.BindUObject(this, &UGatherComponent::RequestAllocatable); 
				//~~ Listen for allocation state change ~~//
				PersonSlot->OnAllocatedChange.AddDynamic(this, &UGatherComponent::UpdateMaxTiles); 
				AllocatedPersonSlots.Add(PersonSlot);
			}
		}
	}
}

UAllocatable* UGatherComponent::RequestAllocatable(UClass* AllocatableClass, FString AllocatableID)
{
	if (ParentStructure)
	{
		return ParentStructure->RequestAllocatable(AllocatableClass, AllocatableID);
	}
	return nullptr;
}

void UGatherComponent::OnEnterEditMode()
{
	Super::OnEnterEditMode();

	if (ParentStructure)
	{
		for (auto& Hex : ParentStructure->HexesInRange)
		{
			if (Hex)
			{
				if (Hex->AllocatedTo && !AllocatedTileSlots.Contains(Hex->AllocatedTo))
				{
					continue;
				}
				if (Hex->AttachedBuilding)
				{
					continue;
				}
				if (!IsHexWorkable(Hex)) {
					continue;
				}
				FOnHexClickedDelegate* Delegate = Hex->BindToOnHexClicked(this, 0);
				if (Delegate)
				{
					Delegate->BindUObject(this, &UGatherComponent::ParseToggleAllocateHex, false);

					//Hex->UnbindToHexClicked(this);

					/*
					FOnHexClickedDelegate* Delegate = Hex->BindToOnHexClicked(this, 0);
					if (Delegate)
					{
						Delegate->BindUObject(this, &APOTLStructure::ToggleAllocateHex, false);
					}
					*/

					/*
					FOnHexUnallocateDelegate* UnallocateDelegate = ParentStructure->BindToOnHexUnallocate(this, 0);
					if (UnallocateDelegate)
					{
						UnallocateDelegate->BindUObject(this, &UGatherComponent::ParseUnallocateHex);
					}
					*/
				}
			}
		}
	}
}

void UGatherComponent::OnLeaveEditMode()
{
	Super::OnLeaveEditMode();

}

void UGatherComponent::Init()
{
	Super::Init();

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

	ProcessBaseData();

	/*
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
		//~~ Update Production when structure allocated hexes change ~~//
		//ParentStructure->OnAllocatedHexesChangedDelegate.AddDynamic(this, &UGatherComponent::CalcPetalProduction);
	}
	*/

	//CalcPetalProduction();

	// ValidateRequirements every second
	GetWorld()->GetTimerManager().SetTimer(GatherCheckTimer, this, &UGatherComponent::ValidateRequirements, 1.f, true);
}


void UGatherComponent::BeginPlay()
{
	Super::BeginPlay();

}
