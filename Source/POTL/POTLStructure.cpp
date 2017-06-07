// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "POTLGameMode.h"
#include "Components/UStorageComponent.h"
#include "Components/UResidentsComponent.h"
#include "Components/UConstructionComponent.h"
#include "Components/UGatherComponent.h"
#include "UObjects/UHexTile.h"
#include "Kismet/GameplayStatics.h"
#include "POTLPlayerController.h"
#include "POTLStructure.h"


// Sets default values
APOTLStructure::APOTLStructure(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GameInstance = nullptr;
	AttachedTo = nullptr;

	HexIndex = -1;
	BaseHex = nullptr;
	CubeCoord = { -1, -1, -1 };
	StructureBaseData = FST_Structure{};
	StructureRowName = TEXT("");
	IsPlaceholder = false;
	bIsInitialized = false;
	BlockPathing = true;
	IsUnderConstruction = true;

	ConstructionComponent = CreateDefaultSubobject<UConstructionComponent>(TEXT("ConstructionComponent"));
	if (ConstructionComponent)
	{
		AddOwnedComponent(ConstructionComponent);
	}

	/*
	#define    COLLISION_PLAYERMOVEMENT    ECollisionChannel::ECC_GameTraceChannel1
	#define    COLLISION_TETHERBALL        ECollisionChannel::ECC_GameTraceChannel2
	#define    COLLISION_PLAYERPAWN        ECollisionChannel::ECC_GameTraceChannel3
	*/

	/*
	CollisionComp->BodyInstance.SetCollisionProfileName("Custom");
	CollisionComp->SetCollisionObjectType(COLLISION_TETHERBALL);
	CollisionComp->SetNotifyRigidBodyCollision(true);
	CollisionComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(COLLISION_PLAYERMOVEMENT, ECollisionResponse::ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(COLLISION_TETHERBALL, ECollisionResponse::ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(COLLISION_PLAYERPAWN, ECollisionResponse::ECR_Ignore);
	CollisionComp->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics, true);
	*/
}

//AVehicle(const class FPostConstructInitializeProperties& PCIP, FString Path, FString Name);


/******************** UTIL *************************/

void APOTLStructure::Select()
{
	if (!bSelected)
	{
		bSelected = true;
		OnSelected();
		GameInstance = Cast<UPOTLGameInstance>(GetGameInstance());
		if (GameInstance)
		{
			GameInstance->OnStructureSelectedDelegate.Broadcast(this);
		}
	}
}
void APOTLStructure::Deselect()
{
	if (bSelected)
	{
		LeaveEditMode();
		bSelected = false;
		OnDeselected();
		GameInstance = Cast<UPOTLGameInstance>(GetGameInstance());
		if (GameInstance)
		{
			GameInstance->OnStructureDeselectedDelegate.Broadcast(this);
		}
	}
}
void APOTLStructure::EnterEditMode()
{
	if (!bInEditMode)
	{
		//~~ EnterEditMode for all UStructureComponents ~~//
		/*
		TArray<UActorComponent*> StructureComponents = GetComponentsByClass(UStructureComponent::StaticClass());
		for (auto& Component : StructureComponents)
		{
			UStructureComponent* StructureComponent = Cast<UStructureComponent>(Component);
			if (StructureComponent)
			{
				StructureComponent->EnterEditMode();
			}
		}
		*/
		//TODO: Move allocated and in range decal handling to function
		for (auto& Hex : HexesInRange)
		{
			if (Hex)
			{
				if (Hex->AllocatedTo)
				{
					if (Hex->AllocatedTo != this)
					{
						continue;
					}
				}
				if (Hex->AttachedBuilding)
				{
					continue;
				}

				// If tile is workable with current components
				UActorComponent* ChildComponent = GetComponentByClass(UGatherComponent::StaticClass());
				if (ChildComponent)
				{
					UGatherComponent* GatherComponent = Cast<UGatherComponent>(ChildComponent);
					if (GatherComponent)
					{
						if (!GatherComponent->IsHexWorkable(Hex)) {
							continue;
						}
					}
				}

				Hex->ShowDecal(EDecalType::ValidBuild);

				FOnHexClickedDelegate* Delegate = Hex->BindToOnHexClicked(this, 0);
				if (Delegate)
				{
					Delegate->BindUObject(this, &APOTLStructure::ToggleAllocateHex, false);
				}
				//Hex->BindToOnHexClicked(0, &APOTLStructure::ToggleAllocateHex);

				//Delegate.BindUObject(this, &APOTLStructure::ToggleAllocateHex, false);
				//Delegate.BindUObject(this, &APOTLStructure::ToggleAllocateHex);
				//Delegate.BindUFunction(this, "SomeFunctionThatReturnsEHandleType");
				//Delegate.IsBoundToObject(this)

				
				//Hex->OnHexToggleAllocate.RemoveDynamic(this, &APOTLStructure::ToggleAllocateHex);
				//Hex->OnHexToggleAllocate.AddDynamic(this, &APOTLStructure::ToggleAllocateHex);
			}
		}
		for (auto& Hex : AllocatedHexes)
		{
			if (Hex)
			{
				Hex->ShowDecal(EDecalType::Allocated);
			}
		}
		/*
		//~~ Set edit mode in player controller ~~//
		APOTLPlayerController* PlayerController = Cast<APOTLPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PlayerController)
		{
			PlayerController->EditStructure(this);
		}
		*/
		bInEditMode = true;
		OnEnterEditMode();
	}
}
void APOTLStructure::LeaveEditMode()
{
	if (bInEditMode)
	{
		for (auto& Hex : HexesInRange)
		{
			if (Hex)
			{
				Hex->HideDecal();
				Hex->UnbindToHexClicked(this);
				//Hex->OnHexToggleAllocate.RemoveDynamic(this, &APOTLStructure::ToggleAllocateHex);
			}
		}
		bInEditMode = false;
		OnLeaveEditMode();
	}
}
void APOTLStructure::UpdateInRangeLists(bool bUpdateOthers)
{
	StructuresInRange.Empty();
	RootStructuresInRange.Empty();
	for (auto& Hex : HexesInRange)
	{
		if (Hex && Hex->AttachedBuilding && Hex->AttachedBuilding != this)
		{
			APOTLStructure* Structure = Hex->AttachedBuilding;
			if (!StructuresInRange.Contains(Structure))
			{
				StructuresInRange.Add(Structure);
			}
			if (!Structure->AttachedTo && !RootStructuresInRange.Contains(Structure))
			{
				RootStructuresInRange.Add(Structure);
			}
			if (bUpdateOthers)
			{
				Structure->UpdateInRangeLists(false);
			}
		}
	}
	//~~ Sort in range structure lists by distance ~~//
	StructuresInRange.Sort([this](const APOTLStructure& Structure1, const APOTLStructure& Structure2) {
		return (Structure1.GetActorLocation() - this->GetActorLocation()).Size() < (Structure2.GetActorLocation() - this->GetActorLocation()).Size();
	});
	RootStructuresInRange.Sort([this](const APOTLStructure& Structure1, const APOTLStructure& Structure2) {
		return (Structure1.GetActorLocation() - this->GetActorLocation()).Size() < (Structure2.GetActorLocation() - this->GetActorLocation()).Size();
	});
}
FOnHexAllocateDelegate* APOTLStructure::BindToOnHexAllocate(UObject* Listener, int Priority)
{
	if (Listener)
	{
		UnbindToOnHexAllocate(Listener);
		FOnHexAllocateDelegate* Delegate = new FOnHexAllocateDelegate();
		OnHexAllocateDelegates.Add(Listener, Delegate);
		return Delegate;
	}
	return nullptr;
}
void APOTLStructure::UnbindToOnHexAllocate(UObject* Listener)
{
	if (Listener && OnHexAllocateDelegates.Contains(Listener))
	{
		FOnHexAllocateDelegate* Delegate = OnHexAllocateDelegates[Listener];
		Delegate->Unbind();
		delete Delegate;
		OnHexAllocateDelegates.Remove(Listener);
	}
}
FOnHexUnallocateDelegate* APOTLStructure::BindToOnHexUnallocate(UObject* Listener, int Priority)
{
	if (Listener)
	{
		UnbindToOnHexUnallocate(Listener);
		FOnHexUnallocateDelegate* Delegate = new FOnHexUnallocateDelegate();
		OnHexUnallocateDelegates.Add(Listener, Delegate);
		return Delegate;
	}
	return nullptr;
}
void APOTLStructure::UnbindToOnHexUnallocate(UObject* Listener)
{
	if (Listener && OnHexUnallocateDelegates.Contains(Listener))
	{
		FOnHexUnallocateDelegate* Delegate = OnHexUnallocateDelegates[Listener];
		Delegate->Unbind();
		delete Delegate;
		OnHexUnallocateDelegates.Remove(Listener);
	}
}
EHandleType APOTLStructure::ToggleAllocateHex(UHexTile* Hex, bool bUpdate)
{
	if (Hex)
	{
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
			OnAllocatedHexesChangedDelegate.Broadcast();
			OnAllocatedHexesChanged();


			//TODO: Move allocated and in range decal handling to function
			/*
			for (auto& Hex : HexesInRange)
			{
				if (Hex)
				{
					if (!Hex->AllocatedTo || (Hex->AllocatedTo && Hex->AllocatedTo == this))
					{
						Hex->ShowDecal(EDecalType::ValidBuild);
						//Hex->OnHexToggleAllocate.RemoveDynamic(this, &APOTLStructure::ToggleAllocateHex);
						//Hex->OnHexToggleAllocate.AddDynamic(this, &APOTLStructure::ToggleAllocateHex);
					}
				}
			}
			for (auto& Hex : AllocatedHexes)
			{
				if (Hex)
				{
					Hex->ShowDecal(EDecalType::Allocated);
				}
			}
			*/

			//~~ Update AllocatedHexes in all structure components ~~//
			TArray<UActorComponent*> StructureComponents = GetComponentsByClass(UStructureComponent::StaticClass());
			for (auto& Component : StructureComponents)
			{
				UStructureComponent* StructureComponent = Cast<UStructureComponent>(Component);
				if (StructureComponent)
				{
					StructureComponent->AllocatedHexes = AllocatedHexes;
				}
			}
		}
	}

	return EHandleType::Unhandled;
}

bool APOTLStructure::AllocateHex(UHexTile* Hex)
{
	if (!AllocatedHexes.Contains(Hex))
	{
		bool bSuccesfullyUnallocated = true;
		if (Hex->AllocatedTo)
		{
			bSuccesfullyUnallocated = Hex->AllocatedTo->UnallocateHex(Hex);
		}
		if (bSuccesfullyUnallocated)
		{
			for (auto& Delegate : OnHexAllocateDelegates)
			{
				if (Delegate.Key && Delegate.Value && Delegate.Value->IsBound())
				{
					EHandleType Response = Delegate.Value->Execute(Hex);
					if (Response == EHandleType::HandledBreak)
					{
						return false;
					}
				}
			}

			Hex->ShowDecal(EDecalType::Allocated);

			Hex->AllocatedTo = this;
			AllocatedHexes.Add(Hex);
			OnHexAllocated(Hex);
			return true;
		}
	}
	return false;
}

bool APOTLStructure::UnallocateHex(UHexTile* Hex)
{
	if (AllocatedHexes.Contains(Hex))
	{
		for (auto& Delegate : OnHexUnallocateDelegates)
		{
			if (Delegate.Key && Delegate.Value && Delegate.Value->IsBound())
			{
				EHandleType Response = Delegate.Value->Execute(Hex);
				if (Response == EHandleType::HandledBreak)
				{
					return false;
				}
			}
		}

		Hex->ShowDecal(EDecalType::ValidBuild);

		Hex->AllocatedTo = nullptr;
		AllocatedHexes.Remove(Hex);
		OnHexUnallocated(Hex);
		return true;
	}
	return false;
}

/******************** RESOURCES *************************/

int APOTLStructure::AddResource(FString ResourceId, int32 Quantity)
{
	int Leftovers = Quantity;
	
	// Check Attachments for storage with AddResource
	if (AttachedTo)
	{
		Leftovers = AttachedTo->AddResource(ResourceId, Quantity);
	}
	else {
		// distribute to self and attachments?
		for (auto& Attachment : AttachedStructures)
		{
			if (IsValid(Attachment))
			{

			}
		}
	}

	// Check for storage component in self
	if (Leftovers > 0)
	{
		TArray<UActorComponent*> StorageComponents = GetComponentsByClass(UStorageComponent::StaticClass());
		for (auto& Component : StorageComponents)
		{
			UStorageComponent* StorageComponent = Cast<UStorageComponent>(Component);
			if (StorageComponent)
			{
				Leftovers = StorageComponent->AddResource(ResourceId, Leftovers);
				if (Leftovers > 0)
				{

				}
				else
				{
					//Added = true;
					break;
				}
			}
		}
	}
	return Leftovers;
}
bool APOTLStructure::StoreResource(UResource* Resource)
{
	if (Resource)
	{
		TArray<UActorComponent*> StorageComponents = GetComponentsByClass(UStorageComponent::StaticClass());
		for (auto& Component : StorageComponents)
		{
			UStorageComponent* StorageComponent = Cast<UStorageComponent>(Component);
			if (StorageComponent && StorageComponent->bAllowFlowIn)
			{
				if (StorageComponent->StoreResource(Resource))
				{
					return true;
				}
			}
		}
	}
	if (AttachedTo && AttachedTo != this)
	{
		return AttachedTo->StoreResource(Resource);
	}
	else if (AttachedTo == this)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("WARNING: AttachTo link to self!"));
	}
	return false;
}
void APOTLStructure::AddWealth(float Amount)
{
	UActorComponent* ChildComponent = GetComponentByClass(UResidentsComponent::StaticClass());
	if (ChildComponent)
	{
		UResidentsComponent* ResidentsComponent = Cast<UResidentsComponent>(ChildComponent);
		if (ResidentsComponent)
		{

		}
	}
	else
	{

	}
}
void APOTLStructure::SubtractWealth(float Amount)
{
	UActorComponent* ChildComponent = GetComponentByClass(UResidentsComponent::StaticClass());
	if (ChildComponent)
	{
		UResidentsComponent* ResidentsComponent = Cast<UResidentsComponent>(ChildComponent);
		if (ResidentsComponent)
		{

		}
	}
	else
	{

	}
}

/******************** CONSTRUCTION *************************/

void APOTLStructure::Init()
{
	//StructureBaseData.ConstructionTime

	//~~ Get hexes in range ~~//
	if (BaseHex)
	{
		if (AttachedTo)
		{
			HexesInRange = AttachedTo->HexesInRange;
		}
		else
		{
			GameInstance = Cast<UPOTLGameInstance>(GetGameInstance());
			if (GameInstance)
			{
				TArray<FVector> Cubes = UPOTLUtilFunctionLibrary::GetCubesInRange(CubeCoord, 5, false);
				for (int32 i = 0; i < Cubes.Num(); i++)
				{
					FVector2D OffsetCoords = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(Cubes[i]);
					int32 HexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(OffsetCoords, GameInstance->GridXCount);
					if (GameInstance->Hexes.IsValidIndex(HexIndex))
					{
						HexesInRange.Add(GameInstance->Hexes[HexIndex]);
					}
				}
			}
		}
	}

	if (IsPlaceholder)
	{
		// Show area resource information and new connections to other structures
		for (auto& Hex : HexesInRange)
		{
			if (Hex)
			{
				Hex->ShowDecal(EDecalType::ValidBuild);

				// Spawn AResourceSpot (Hex)

				//Hex->ShowResourceInformation();

				if (Hex->AttachedBuilding)
				{
					// Show new Connection information
					// StructuresInRange.Add(Hex->AttachedBuilding)
				}
			}
		}
		OnShowResourceInformation();
		// On Remove/Destroy clean up resource- and connection information.
	}
	else
	{
		if (IsUnderConstruction)
		{
			if (ConstructionComponent)
			{
				ConstructionComponent->OnComplete.AddDynamic(this, &APOTLStructure::CompleteConstruction);
				ConstructionComponent->Init();
			}
		}
		else
		{
			ProcessBaseData();
			// Initialize all UStructureComponents
			TArray<UActorComponent*> StructureComponents = GetComponentsByClass(UStructureComponent::StaticClass());
			for (auto& Component : StructureComponents)
			{
				UStructureComponent* StructureComponent = Cast<UStructureComponent>(Component);
				if (StructureComponent)
				{
					StructureComponent->Init();
				}
			}
			bIsInitialized = true;
		}
		UpdateInRangeLists(true);
		OnInit();
	}
}
void APOTLStructure::RemoveStructure()
{
	if (IsPlaceholder)
	{
		for (auto& Hex : HexesInRange)
		{
			if (Hex)
			{
				Hex->HideDecal();
			}
		}
	}
	//~~ Remove self from hexes ~~//
	for (auto& OccupiedHex : OccupiedHexes)
	{
		if (OccupiedHex && OccupiedHex->AttachedBuilding == this)
		{
			OccupiedHex->AttachedBuilding = nullptr;
		}
	}
	if (BaseHex && BaseHex->AttachedBuilding == this)
	{
		BaseHex->AttachedBuilding = nullptr;
	}
	//~~ Clear all timers ~~//
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	OnRemoveStructure();
	//~~ DESTROY ~~//
	Destroy();
}
void APOTLStructure::ProcessBaseData()
{
	/*
	for (auto& Factory : StructureBaseData.Factories)
	{
		//FST_ResourceAllocation& Allocation = AllocatedResource.Value;
		//UFactoryComponent* FactoryComponent = NewObject<UFactoryComponent>(this);
		//FactoryComponent->RegisterComponent();
		//FactoryComponent->Recipe = Factory.Recipe;
		//FactoryComponent->MaxQuantity = Factory.MaxQuantity;
		//Factories.Add(FactoryComponent);
	}
	*/
}
void APOTLStructure::AttachToStructure(APOTLStructure* Structure)
{
	if (IsValid(Structure))
	{
		AttachedTo = Structure;
		Structure->AttachedStructures.Add(this);
	}
}
void APOTLStructure::DetachFromStructure()
{
	if (AttachedTo)
	{
		AttachedTo->AttachedStructures.Remove(this);
		AttachedTo = nullptr;
	}
}
void APOTLStructure::CompleteConstruction()
{
	IsUnderConstruction = false;
	OnConstructionComplete();
	Init(); // Re-init structure
}
UResource* APOTLStructure::RequestResource(FString ResourceId, bool bBubble = true)
{
	// Search self
	TArray<UActorComponent*> StorageComponents = GetComponentsByClass(UStorageComponent::StaticClass());
	for (auto& Component : StorageComponents)
	{
		UStorageComponent* StorageComponent = Cast<UStorageComponent>(Component);
		if (StorageComponent)
		{
			UResource* Resource = StorageComponent->RequestResource(this, ResourceId);
			if (Resource)
			{
				return Resource;
			}
		}
	}
	if (bBubble)
	{
		// Search others in range
		for (auto& Structure : RootStructuresInRange)
		{
			if (Structure)
			{
				UResource* Resource = Structure->RequestResource(ResourceId, false);
				if (Resource)
				{
					return Resource;
				}
			}
		}
	}
	return nullptr;
}
UResource* APOTLStructure::RequestResourceByTag(FString TagId, bool bBubble = true)
{
	// Search self
	TArray<UActorComponent*> StorageComponents = GetComponentsByClass(UStorageComponent::StaticClass());
	for (auto& Component : StorageComponents)
	{
		UStorageComponent* StorageComponent = Cast<UStorageComponent>(Component);
		if (StorageComponent)
		{
			UResource* Resource = StorageComponent->RequestResourceByTag(this, TagId);
			if (Resource)
			{
				return Resource;
			}
		}
	}
	if (bBubble)
	{
		// Search others in range
		for (auto& Structure : RootStructuresInRange)
		{
			if (Structure)
			{
				UResource* Resource = Structure->RequestResourceByTag(TagId, false);
				if (Resource)
				{
					return Resource;
				}
			}
		}
	}
	return nullptr;
}
bool APOTLStructure::RequestLabor(int Amount)
{
	TArray<UActorComponent*> ChildComponents = GetComponentsByClass(UResidentsComponent::StaticClass());
	for (auto& Component : ChildComponents)
	{
		UResidentsComponent* ResidentsComponent = Cast<UResidentsComponent>(Component);
		if (ResidentsComponent)
		{
			if (ResidentsComponent->RequestLabor(Amount))
			{
				return true;
			}
		}
	}
	if (AttachedTo)
	{
		return AttachedTo->RequestLabor(Amount);
	}
	return false;
}

bool APOTLStructure::StoreLabor(int Amount)
{
	TArray<UActorComponent*> ChildComponents = GetComponentsByClass(UResidentsComponent::StaticClass());
	for (auto& Component : ChildComponents)
	{
		UResidentsComponent* ResidentsComponent = Cast<UResidentsComponent>(Component);
		if (ResidentsComponent)
		{
			if (ResidentsComponent->StoreLabor(Amount))
			{
				return true;
			}
		}
	}
	if (AttachedTo)
	{
		return AttachedTo->StoreLabor(Amount);
	}
	return false;
}


/******************** MAP *************************/
APOTLStructure* APOTLStructure::GetNearestStructure()
{
	APOTLStructure* NearestStructure = nullptr;
	//TSubclassOf<AActor> ClassToFind;
	//Array<AActor*> FoundActors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassToFind, FoundActors);

	for (TActorIterator<APOTLStructure> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		//ClientMessage(ActorItr->GetName());
		//ClientMessage(ActorItr->GetActorLocation().ToString());
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, ActorItr->GetName());
	}

	return NearestStructure;
}



FOnStructureClickedDelegate* APOTLStructure::BindToOnStructureClicked(UObject* Listener, int Priority)
{
	if (Listener)
	{
		UnbindToStructureClicked(Listener);
		FOnStructureClickedDelegate* Delegate = new FOnStructureClickedDelegate();
		OnStrucureClickedDelegates.Add(Listener, Delegate);
		return Delegate;
	}
	else
	{
		return nullptr;
	}
}
void APOTLStructure::UnbindToStructureClicked(UObject* Listener)
{
	if (Listener && OnStrucureClickedDelegates.Contains(Listener))
	{
		FOnStructureClickedDelegate* Delegate = OnStrucureClickedDelegates[Listener];
		Delegate->Unbind();
		delete Delegate;
		OnStrucureClickedDelegates.Remove(Listener);
	}
}
void APOTLStructure::ClickStructure()
{
	// Loop function array and execute. They must have a EHandleType response, which can break the loop. 
	// Function to add function to list with prioty index,
	// or  BindUFunction??
	bool Handled = false;
	for (auto& Delegate: OnStrucureClickedDelegates)
	{
		if (Delegate.Key && Delegate.Value && Delegate.Value->IsBound())
		{
			EHandleType Response = Delegate.Value->Execute(this);
			if (Response == EHandleType::HandledBreak)
			{
				break;
			}
			else if (Response == EHandleType::Handled)
			{
				Handled = true;
			}
		}
	}
	if (!Handled)
	{
		Select();
	}

	/*
	FOnStructureClickedDelegate Delegate = NearestStructure->BindToOnStructureClicked(0);
	Delegate.BindRaw(this, &APOTLStructure::SomeFunctionThatReturnsEHandleType);
	Delegate.BindUFunction(this, "SomeFunctionThatReturnsEHandleType");
	*/

	/*
	if (OnStructureClickedDelegate.IsBound())
	{
		EHandleType Response = OnStructureClickedDelegate.Execute();
		if (Response == EHandleType::Handled)
		{
			return;
		}
	}
	*/
}

void APOTLStructure::UpdateConstrunction_Implementation()
{
	if (IsUnderConstruction)
	{

	}
	else
	{

	}
}


void APOTLStructure::OnStructureClicked_Implementation()
{

}
void APOTLStructure::OnTimeUpdate_Implementation(float Time, float TimeProgressed)
{
	/*
	if (!IsPlaceholder)
	{
		if (IsUnderConstruction)
		{
			// Should other "work" the construction? Like working on a station
			ConstructionTimeLeft -= TimeProgressed;
			ProcentConstructed = FMath::Abs(ConstructionTimeLeft - 5) / 5 * 100; // 5 is construction time from data
			if (ConstructionTimeLeft <= 0)
			{
				ProcentConstructed = 100.f;
				CompleteConstruction();
			}
		}
	}
	*/

	// If attached then progress construction

	// Check Construction progress -> Complete construction()
}
void APOTLStructure::OnInit_Implementation()
{

}
void APOTLStructure::OnConstructionComplete_Implementation()
{

}
void APOTLStructure::OnRemoveStructure_Implementation()
{

}
void APOTLStructure::OnShowResourceInformation_Implementation()
{

}
void APOTLStructure::OnSelected_Implementation()
{

}
void APOTLStructure::OnDeselected_Implementation()
{

}
void APOTLStructure::OnEnterEditMode_Implementation()
{

}
void APOTLStructure::OnLeaveEditMode_Implementation()
{

}
void APOTLStructure::OnAllocatedHexesChanged_Implementation()
{

}
void APOTLStructure::OnHexAllocated_Implementation(UHexTile* Hex)
{

}
void APOTLStructure::OnHexUnallocated_Implementation(UHexTile* Hex)
{

}


//~~ Called when the game starts or when spawned ~~//
void APOTLStructure::BeginPlay()
{
	Super::BeginPlay();

	// Bind to time update
	APOTLGameMode* GameMode = Cast<APOTLGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->OnTimeUpdated.AddDynamic(this, &APOTLStructure::OnTimeUpdate);
	}

	GameInstance = Cast<UPOTLGameInstance>(GetGameInstance());
	if (GameInstance)
	{

		//IncludeStorage

		// Add test resources
		//FreeResources.Add(TEXT("Wood"), 9.f);
		//FreeResources.Add(TEXT("Stone"), 15.f);

		// Add test factory for resource process
		//FST_Factory Factory;
		//Factory.Invoice.Add(FName(TEXT("Plank"), 1));
		//Factory.Invoice.Add(TEXT("Plank"), 1);
		//Factories.Add(Factory);

		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Factory.Invoice[FName(TEXT("Plank")]);
	}

	if (IsValid(AttachedTo))
	{
		// Bind to labor 


	}

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FName(TEXT("Plank")).ToString());

	/*********** BINDINGS **************/
	//UPOTLGameInstance::OnTurnSwitched.AddDynamic(this, &APOTLStructure::RequestResources);
	//UPOTLGameInstance::OnTurnSwitched.Add(this, &APOTLStructure::RequestResources);
	//UPOTLGameInstance::OnNewTurn.Add(this, &APOTLStructure::BindToDelegate);
}

//~~ Called every frame ~~//
void APOTLStructure::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

