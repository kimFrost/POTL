// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "POTLGameMode.h"
#include "Components/UStorageComponent.h"
#include "Components/UResidentsComponent.h"
#include "Components/UConstructionComponent.h"
#include "UObjects/UHexTile.h"
#include "Kismet/GameplayStatics.h"
#include "POTLStructure.h"


// Sets default values
APOTLStructure::APOTLStructure(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GameInstance = nullptr;
	AttachedTo = nullptr;

	HexIndex = -1;
	Hex = nullptr;
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


/*****************************************************************************************************/
/******************************************* RESOURCES ***********************************************/
/*****************************************************************************************************/


/******************** AddResource *************************/
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


/******************** StoreResource *************************/
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


/******************** AddWealth *************************/
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


/******************** SubtractWealth *************************/
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



/*****************************************************************************************************/
/****************************************** CONSTRUCTION *********************************************/
/*****************************************************************************************************/


/******************** UpdateConstrunction *************************/
void APOTLStructure::UpdateConstrunction_Implementation()
{
	if (IsUnderConstruction)
	{

	}
	else
	{

	}
}


/******************** Init *************************/
void APOTLStructure::Init()
{
	//StructureBaseData.ConstructionTime

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

	OnInit();
}


/******************** RemoveStructure *************************/
void APOTLStructure::RemoveStructure()
{
	//~~ Remove self from hexes ~~//
	for (auto& OccupiedHex : OccupiedHexes)
	{
		if (OccupiedHex)
		{
			OccupiedHex->AttachedBuilding = nullptr;
		}
	}
	if (Hex)
	{
		Hex->AttachedBuilding = nullptr;
	}
	//~~ Clear all timers ~~//
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	//~~ DESTROY ~~//
	Destroy();
}


/******************** ProcessBaseData *************************/
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


/******************** CompleteConstruction *************************/
void APOTLStructure::CompleteConstruction()
{
	IsUnderConstruction = false;
	OnConstructionComplete();
	Init(); // Re-init structure
};


/*****************************************************************************************************/
/********************************************** MAP **************************************************/
/*****************************************************************************************************/



/******************** GetNearestStructure *************************/
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

