// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "POTLGameMode.h"
#include "FactoryComponent.h"
#include "GatherComponent.h"
#include "Components/UStorageComponent.h"
#include "Kismet/GameplayStatics.h"
#include "POTLStructure.h"


// Sets default values
APOTLStructure::APOTLStructure(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GameInstance = nullptr;
	EmitTo = nullptr;
	AttachedTo = nullptr;

	IsRoot = true;
	TreeId = TEXT("");
	HexIndex = -1;
	CubeCoord = { -1, -1, -1 };
	BroadcastRange = 0;
	StructureBaseData = FST_Structure{};
	StructureRowName = TEXT("");
	IsPlaceholder = false;
	BlockPathing = true;
	IsUnderConstruction = true;
	NumOfLivingSpaces = 0;
	InRangeOfEmitTo = false;
	Root = this;

	//GameInstance = Cast<UPOTLGameInstance>(GetGameInstance()); //~~ <== Will crash. The game instance is not ready at this point ~~//


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


	/*
	switch (Type)
	{
	case EResourceList::Free:
		if (FreeResources.Contains(Id))
		{
			if (FreeResources[Id] + Quantity < 0) 
			{

			}
			else
			{
				FreeResources[Id] += Quantity;
			}
			Added = true;
		}
		else
		{
			FreeResources.Add(Id, Quantity);
			Added = true;
		}
		break;
	case EResourceList::Upkeep:
		if (ResourceUpkeep.Contains(Id))
		{
			if (ResourceUpkeep[Id] + Quantity < 0)
			{

			}
			else
			{
				ResourceUpkeep[Id] += Quantity;
			}
			Added = true;
		}
		else
		{
			ResourceUpkeep.Add(Id, Quantity);
			Added = true;
		}
		break;
	default:
		break;
	}
	*/
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
			if (StorageComponent)
			{
				if (StorageComponent->StoreResource(Resource))
				{
					return true;
				}
			}
		}
	}
	if (AttachedTo)
	{
		return AttachedTo->StoreResource(Resource);
	}
	return false;
}


/******************** GetResourcesAsList *************************/
TArray<FST_Resource> APOTLStructure::GetResourcesAsList(EResourceList Type)
{
	TArray<FST_Resource> List;
	switch (Type)
	{
	case EResourceList::Free:
		for (auto& FreeResource : FreeResources)
		{
			FST_Resource Resource;
			Resource.Id = FreeResource.Key;
			Resource.Quantity = FreeResource.Value;
			List.Add(Resource);
		}
		break;
	case EResourceList::Upkeep:
		for (auto& Upkeep : ResourceUpkeep)
		{
			FST_Resource Resource;
			Resource.Id = Upkeep.Key;
			Resource.Quantity = Upkeep.Value;
			List.Add(Resource);
		}
		break;
	case EResourceList::Allocations:
		for (auto& AllocatedResource : AllocatedResources)
		{
			FST_ResourceAllocation& Allocation = AllocatedResource.Value;
			FST_Resource Resource;
			Resource.Id = Allocation.ResourceKey;
			Resource.Quantity = Allocation.Quantity;
			List.Add(Resource);
		}
		break;
	default:
		break;
	}
	return List;
}



/******************** GetAllocationTotal *************************/
int32 APOTLStructure::GetAllocationTotal(FString Type)
{
	int32 Total = 0;
	for (auto& AllocatedResource : AllocatedResources)
	{
		if (Type.Len() == 0 || AllocatedResource.Value.ResourceKey == Type)
		{
			if (AllocatedResource.Value.Type == EAllocationType::FactoryBilling) {
				Total = Total - AllocatedResource.Value.Quantity;
			}
			else {
				Total = Total + AllocatedResource.Value.Quantity;
			}
		}
	}
	return Total;
}



/******************** GetResourceAlteration *************************/
TArray<FST_ResourceAlteration> APOTLStructure::GetResourceAlteration()
{
	TArray<FST_ResourceAlteration> List;
	TMap<FString, FST_ResourceAlteration> TMapList; //~~ Make complete list of resource from FreeResources and allocations ~~//
	for (auto& FreeResource : FreeResources)
	{
		FST_ResourceAlteration ResourceAlteration;
		ResourceAlteration.Id = FreeResource.Key;
		ResourceAlteration.Storage = FreeResource.Value;
		//ResourceAlteration.Alteration = GetAllocationTotal(FreeResource.Key);
		TMapList.Add(FreeResource.Key, ResourceAlteration);
	}
	for (auto& AllocatedResource : AllocatedResources)
	{
		//~~ Add alteration if allocation key isn't in free resources ~~//
		if (AllocatedResource.Value.From == this || AllocatedResource.Value.To == this) //~~~ If either outgoing or incomming, then create an alteration struct  ~~//
		{
			if (!TMapList.Contains(AllocatedResource.Value.ResourceKey))
			{
				FST_ResourceAlteration ResourceAlteration;
				ResourceAlteration.Id = AllocatedResource.Value.ResourceKey;
				TMapList.Add(AllocatedResource.Value.ResourceKey, ResourceAlteration);
			}
		}
		//~~ Outgoing resources from root ~~//
		if (AllocatedResource.Value.From == this) 
		{
			if (AllocatedResource.Value.Type == EAllocationType::Decay)
			{
				TMapList[AllocatedResource.Value.ResourceKey].Decay -= AllocatedResource.Value.Quantity;
				TMapList[AllocatedResource.Value.ResourceKey].Storage += AllocatedResource.Value.Quantity;
			}
			else if (AllocatedResource.Value.Type == EAllocationType::FactoryBilling)
			{
				TMapList[AllocatedResource.Value.ResourceKey].Alteration -= AllocatedResource.Value.Quantity;
				TMapList[AllocatedResource.Value.ResourceKey].Storage += AllocatedResource.Value.Quantity;
			}
		}
		//~~ Incomming resources to root ~~//
		if (AllocatedResource.Value.To == this) 
		{ 
			if (AllocatedResource.Value.Type == EAllocationType::ProductionDecay)
			{
				TMapList[AllocatedResource.Value.ResourceKey].Alteration += AllocatedResource.Value.Quantity;
				TMapList[AllocatedResource.Value.ResourceKey].Decay -= AllocatedResource.Value.Quantity;
			}
			else
			{
				TMapList[AllocatedResource.Value.ResourceKey].Alteration += AllocatedResource.Value.Quantity;
			}
		}
	}
	/*
	if (TMapList.Contains(AllocatedResource.Value.ResourceKey))
	{
		if (AllocatedResource.Value.Type == EAllocationType::FactoryBilling)
		{
			TMapList[AllocatedResource.Value.ResourceKey].Storage = TMapList[AllocatedResource.Value.ResourceKey].Storage + TMapList[AllocatedResource.Value.ResourceKey].Alteration;
		}
	}
	else
	{
		FST_ResourceAlteration ResourceAlteration;
		ResourceAlteration.Id = AllocatedResource.Value.ResourceKey;
		ResourceAlteration.Alteration = GetAllocationTotal(AllocatedResource.Value.ResourceKey);
		TMapList.Add(AllocatedResource.Value.ResourceKey, ResourceAlteration);
	}
	*/
	for (auto& TMapItem : TMapList)
	{
		List.Add(TMapItem.Value);
	}
	//~~ Sort the list by sort sequence ~~//
	/*
	List.Sort([](const FString& One, const FString& Two) {
		return One > Two;
	});
	List.Sort([](const FString& A, const FString& B) {
		return A.Len() < B.Len();
	});
	*/

	//GameInstance->DATA_Recipes

	List.Sort([](const FST_ResourceAlteration& A, const FST_ResourceAlteration& B) {
		return A.Id.Len() < B.Id.Len();
		//return A.Id.Len() < B.Id.Len();
	});

	//List.Sort();
	return List;
}



/******************** GetAllocationsAsList *************************/
TArray<FST_ResourceAllocation> APOTLStructure::GetAllocationsAsList(FString Type)
{
	TArray<FST_ResourceAllocation> List;
	for (auto& AllocatedResource : AllocatedResources)
	{
		if (Type.Len() == 0 || AllocatedResource.Value.ResourceKey == Type)
		{
			List.Add(AllocatedResource.Value);
		}
	}
	return List;
}



/******************** CalculateUpkeep *************************/
void APOTLStructure::CalculateUpkeep(bool Broadcast)
{
	GEngine->AddOnScreenDebugMessage(100, 15.0f, FColor::Magenta, "CalculateUpkeep()");
	//~~ Resolve children ~~//
	if (Broadcast)
	{
		for (int32 i = 0; i < BroadcastTo.Num(); i++)
		{
			BroadcastTo[i]->CalculateUpkeep(Broadcast);
		}
	}
	//~~ Resolve self / The function logic ~~//
	if (!IsUnderConstruction)
	{

	}
}



/******************** ResolveUpkeep *************************/
void APOTLStructure::ReverseAllocations(bool Broadcast)
{
	//~~ Resolve children ~~//
	/*
	if (Broadcast)
	{
		for (int32 i = 0; i < BroadcastTo.Num(); i++)
		{
			BroadcastTo[i]->ReverseAllocations(Broadcast);
		}
	}
	*/
	//~~ Resolve self / The function logic ~~//
	for (auto& AllocatedResource : AllocatedResources)
	{
		FST_ResourceAllocation& Allocation = AllocatedResource.Value;
		if (Allocation.From == this)
		{
			if (Allocation.Type == EAllocationType::FactoryBilling || Allocation.Type == EAllocationType::Decay)
			{
				AddResource(Allocation.ResourceKey, Allocation.Quantity); //, EResourceList::Free // Re-add resource that was allocated from this/root
			}
			/*
			else if (Allocation.Type == EAllocationType::ProductionDecay)
			{

			}
			*/
		}
	}
	AllocatedResources.Empty();
}



/******************** ResolveUpkeep *************************/
void APOTLStructure::ResolveUpkeep(bool Broadcast)
{
	GEngine->AddOnScreenDebugMessage(100, 15.0f, FColor::Magenta, "ResolveUpkeep()");
	//~~ Resolve children ~~//
	if (Broadcast)
	{
		for (int32 i = 0; i < BroadcastTo.Num(); i++)
		{
			BroadcastTo[i]->ResolveUpkeep(Broadcast);
		}
	}
	//~~ Resolve self / The function logic ~~//
	//~~ Resolve upkeep ~~//
	//RequestResources(this, ResourceUpkeep, 0, 0, EAllocationType::RequestDirect, false, true); //~~ Do self have the required resources ~~//
}



/******************** ProcessFactories *************************/
void APOTLStructure::ProcessFactories(bool Broadcast)
{
	if (!IsPlaceholder)
	{
		GEngine->AddOnScreenDebugMessage(100, 15.0f, FColor::Magenta, "ProcessFactories()");
		//~~ Resolve children ~~//
		if (Broadcast)
		{
			for (int32 i = 0; i < BroadcastTo.Num(); i++)
			{
				BroadcastTo[i]->ProcessFactories(Broadcast);
			}
		}
		//~~ Resolve self / The function logic ~~//
		if (GameInstance)
		{
			for (UFactoryComponent* Factory : Factories)
			{
				if (Factory)
				{
					if (Factory->Quantity > 0)
					{
						int32 Sequence = Factory->ProcessInvoice(GameInstance->DATA_Recipes);
						//RequestResources(this, Factory->Requirements, 0, Sequence, EAllocationType::RequestDirect, false, true);
						// Store all requests in root
						//this->Root->RequestResources(this, Factory, Factory->Requirements, Factory->Invoice, Sequence, 0, EAllocationType::RequestDirect, false, true);
						if (Sequence != -1)
						{
							this->Root->RequestResources(this, Factory, Factory->Requirements, Factory->Invoice, Sequence, 0, EAllocationType::FactoryBilling, false, true);
						}
					}
				}
			}
		}
	}
}



/******************** ProcessGatherers *************************/
void APOTLStructure::ProcessGatherers(bool Broadcast)
{
	if (!IsPlaceholder)
	{
		if (Broadcast)
		{
			for (int32 i = 0; i < BroadcastTo.Num(); i++)
			{
				BroadcastTo[i]->ProcessGatherers(Broadcast);
			}
		}
		if (GameInstance)
		{
			for (UGatherComponent* Gatherer : Gatherers)
			{
				if (Gatherer)
				{

				}
			}
		}
	}
}



/******************** MakeTreeAllocations *************************/
void APOTLStructure::MakeTreeAllocations() //~~ Should only for be called on root structures
{
	if (!IsPlaceholder)
	{
		ResourceRequests.Empty();

		// Temp (BEGIN)
		ReverseAllocations(true);
		// How to empty/reverse stored allocations?
		// Temp (END)


		CalculateUpkeep(true);
		ProcessGatherers(true);
		ProcessFactories(true);
		ProcessResourceRequests();
		//MakeDecayAllocations();
		//CalculateDecay();
		ProcessDecay();

		//~~ Make a flow map of resources based on allocations stored in this structure ~~//
		//TMap<APOTLStructure*, TArray<FST_ResourceAllocation>> ResourceFlowMap;
		ResourceFlowMap.Empty();
		for (auto& AllocatedResource : AllocatedResources)
		{
			FST_ResourceAllocation& Allocation = AllocatedResource.Value;
			APOTLStructure* OtherThanSelf;
			//UGameplayStatics::Equ
			if (Allocation.From != this)
			{
				OtherThanSelf = Allocation.From;
			}
			else
			{
				OtherThanSelf = Allocation.To;
			}
			if (OtherThanSelf)
			{
				if (ResourceFlowMap.Contains(OtherThanSelf))
				{
					ResourceFlowMap[OtherThanSelf].Add(Allocation);
				}
				else
				{
					TArray<FST_ResourceAllocation> List;
					List.Add(Allocation);
					ResourceFlowMap.Add(OtherThanSelf, List);
				}
			}
		}
	}
	//~~ Broadcast city updated through Game Instance ~~//
	GameInstance->OnCityUpdated.Broadcast(this);
}



/******************** ResolveTree *************************/
void APOTLStructure::ResolveTree() //~~ Should only for be called on root structures
{
	if (!IsPlaceholder)
	{
		DecayQueueBackup = DecayQueue; // Update DecayQueueBackup with current data
		ResolveAllocations(EAllocationType::All, true); //~~ Resolve all other allocations ~~//
	}
	/*
	FTimerHandle UniqueHandle;
	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &APOTLStructure::ResolveUpkeep, true);
	GetWorldTimerManager().SetTimer(UniqueHandle, RespawnDelegate, 5.0f, false);
	*/
}



/******************** ResolveAllocations *************************/
void APOTLStructure::ResolveAllocations(EAllocationType Type, bool Broadcast)
{
	//FString TypeAsString;
	//const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EAllocationType"), true);
	//TypeAsString = EnumPtr->GetEnumName(Type);
	GEngine->AddOnScreenDebugMessage(100, 15.0f, FColor::Magenta, "ResolveAllocations()");
	//~~ Resolve children ~~//
	if (Broadcast)
	{
		for (int32 i = 0; i < BroadcastTo.Num(); i++)
		{
			BroadcastTo[i]->ResolveAllocations(Type, Broadcast);
		}
	}
	//~~ Resolve self / The function logic ~~//
	for (auto& AllocatedResource : AllocatedResources)
	{
		FST_ResourceAllocation& Allocation = AllocatedResource.Value;
		if (Type == EAllocationType::All || Allocation.Type == Type)
		{
			if (Allocation.Type == EAllocationType::FactoryBilling || 
				Allocation.Type == EAllocationType::Decay || 
				Allocation.Type == EAllocationType::ProductionDecay)
			{
				//~~ The resources are allready removed from freesources, so the allocation just needs to be removed ~~//
				AllocatedResources.Remove(AllocatedResource.Key);
			}
			else
			{
				Allocation.To->AddResource(Allocation.ResourceKey, Allocation.Quantity); // , EResourceList::Free;
				AllocatedResources.Remove(AllocatedResource.Key);
			}
		}
	}
	if (AllocatedResources.Num() > 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, "ERROR: AllocatedResources where not empty, when it was emptied");
	}
	AllocatedResources.Empty();
}



/******************** ProcessResourceRequests *************************/
void APOTLStructure::ProcessResourceRequests()
{
	int32 i;
	int32 HighestSequence = -1;
	TMap<int32, TArray<FST_ResourceRequest>> SortedResourceRequests;
	//~~ Get highest sequence from all of the requests ~~//
	for (i = 0; i < ResourceRequests.Num(); i++)
	{
		FST_ResourceRequest& ResourceRequest = ResourceRequests[i];
		if (ResourceRequest.Sequence > HighestSequence)
		{
			HighestSequence = ResourceRequest.Sequence;
		}
	}
	//~~ Init SortedResourceRequests. Make empty arrays ~~//
	for (i = 0; i <= HighestSequence; i++)
	{
		TArray<FST_ResourceRequest> List;
		SortedResourceRequests.Add(i, List);
	}
	//~~ Add resource request to sorted tmap ~~//
	for (i = 0; i < ResourceRequests.Num(); i++)
	{
		FST_ResourceRequest& ResourceRequest = ResourceRequests[i];
		int32 Sequence = ResourceRequest.Sequence;
		if (SortedResourceRequests.Contains(Sequence))
		{
			SortedResourceRequests[Sequence].Add(ResourceRequest);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, "ERROR: SortedResourceRequests could not recivce request with a sequence not found ");
		}
	}
	//~~ Allocate and process the requests ~~//
	for (i = 0; i <= HighestSequence; i++) //~~ Handle the request from sequence zero and up ~~//
	{
		TArray<FST_ResourceRequest>& RequestList = SortedResourceRequests[i];
		for (int32 ii = 0; ii < RequestList.Num(); ii++) // Loop through sequence row list. Ex 0[{R}, {R}, {R}]
		{
			FST_ResourceRequest& ResourceRequest = RequestList[ii];

			if (ResourceRequest.Request.Num() > 1)
			{
				FString Sasdsa;
			}

			if (HasResourcesAvailable(ResourceRequest.Request, true, i)) //~~ If self has the resources required. (i) is the sequence, not (ii), it is just a list index for the nested array. ~~//
			{
				TArray<int32> AllocationIndexes;
				//~~ Requirements ~~//
				for (auto& ReqResource : ResourceRequest.Request) //~~ Loop each resource in request ~~//
				{
					//? AllocateResource with group id for grouping multiple allocations?
					if (FreeResources.Contains(ReqResource.Key))
					{
						int32 AvailableQuantity = FreeResources[ReqResource.Key];
						if (AvailableQuantity > ReqResource.Value)
						{
							AvailableQuantity = ReqResource.Value;
						}
						//int32 AllocationIndex = AllocateResource(ResourceRequest.From, ReqResource.Key, AvailableQuantity, EAllocationType::FactoryBilling, ii, true, -1);
						int32 AllocationIndex = AllocateResource(ResourceRequest.From, ReqResource.Key, AvailableQuantity, EAllocationType::FactoryBilling, i, true, -1);
						AllocationIndexes.Add(AllocationIndex);
						ReqResource.Value = ReqResource.Value - AvailableQuantity;
					}
					if (ReqResource.Value > 0) //~~ If freeresources couldn't meet the requiement of the resource quantity, then make reallocations ~~//
					{
						for (auto& AllocatedResource : AllocatedResources)
						{
							FST_ResourceAllocation& Allocation = AllocatedResource.Value;
							if (Allocation.ResourceKey == ReqResource.Key && 
								Allocation.Sequence < ResourceRequest.Sequence && 
								Allocation.To == this && 
								Allocation.Type == EAllocationType::FactoryProduction && 
								Allocation.Quantity > 0)
							{	
								//!! BUG BEGIN
								if (ReqResource.Value < Allocation.Quantity) //~~ If request if less than allocation quantity, then split allocation ~~//
								{
									//Allocation.Quantity = Allocation.Quantity - ReqResource.Value; //~~ Subtract resource request quantity from the allocation, and then let it be ~~//
									//int32 NewAllocationAmount = Allocation.Quantity - ReqResource.Value;
									int32 NewAllocationAmount = ReqResource.Value;
									Allocation.Quantity = Allocation.Quantity - ReqResource.Value; //~~ Subtract resource request quantity from the allocation, and then let it be ~~//
									//Split logic //~~ Make new allocation with ~~//
									//int32 AllocationIndex = Allocation.From->AllocateResource(ResourceRequest.From, ReqResource.Key, ReqResource.Value, EAllocationType::FactoryBilling, ii, false, -1);
									int32 AllocationIndex = Allocation.From->AllocateResource(ResourceRequest.From, ReqResource.Key, NewAllocationAmount, EAllocationType::FactoryBilling, Allocation.Sequence, false, -1);
									AllocationIndexes.Add(AllocationIndex);
									ReqResource.Value = 0;
								}
								else if (ReqResource.Value >= Allocation.Quantity) //~~ If request is more or equal to allocation quantity, then just change allocation to target ~~//
								{
									Allocation.To = ResourceRequest.From;
									Allocation.Type = EAllocationType::FactoryBilling; //! Maybe it needs to be a unique type, like FactoryProductionReallocation or something.
									//ReqResource.Value = ReqResource.Value - AvailableQuantity;
									//ReqResource.Value = ReqResource.Value - Allocation.Quantity;
									FString TestValue = FString::FromInt(Allocation.Quantity);
									ReqResource.Value = ReqResource.Value - Allocation.Quantity;
									// 3 ? = 4 - 4
									// ? = 4 - 2;
								}
								//!! BUG END
							}
 							if (ReqResource.Value == 0)
							{
								break;
							}
						}
						if (ReqResource.Value > 0)
						{
							FString Bug;
						}
					}
				}
				//~~ Payoff ~~//
				for (auto& Resource : ResourceRequest.Payoff) //~~ Loop each resource in payoff ~~//
				{
					//~~ Add resource payoff to the keeper of the factory, and then allocate it to root afterwards ~~//
					//ResourceRequest.From->AddResource(Resource.Key, Resource.Value, EResourceList::Free); //~~ Add Resources and then consume them with AllocateResource(true) ~~// //!~~ No need for adding them, and then consuming them anymore. AllocateResource can not be called with consume: false ~~//
					//int32 AllocationIndex = ResourceRequest.From->AllocateResource(this, Resource.Key, Resource.Value, EAllocationType::FactoryProduction, ii, false, -1); // Maybe ii + 1 ? I think not. Are checked if sequence is lower, not lower or equal
					int32 AllocationIndex = ResourceRequest.From->AllocateResource(this, Resource.Key, Resource.Value, EAllocationType::FactoryProduction, i, false, -1); // Maybe ii + 1 ? I think not. Are checked if sequence is lower, not lower or equal
					if (ResourceRequest.Factory)
					{
						ResourceRequest.Factory->AllocationIndex = AllocationIndex;
					}
					AllocationIndexes.Add(AllocationIndex);
				}
				ResourceRequest.RequestMet = true;
			}
			else
			{ 

			}
		}
	}
}


/******************** HasResourcesAvailable *************************/
bool APOTLStructure::HasResourcesAvailable(TMap<FString, int32>& Request, bool IncludeAllocations, int32 Sequence)
{
	bool RequestMet = true;
	TMap<FString, int32> ResourceAvailable = FreeResources; //~~ Copy resources ~~//
	//~~ Append all allocated resources  with lower sequence to available resources ~~//

	if (IncludeAllocations) 
	{
		for (auto& AllocatedResource : AllocatedResources)
		{
			FST_ResourceAllocation& Allocation = AllocatedResource.Value;
			if (Allocation.Sequence < Sequence && Allocation.Type == EAllocationType::FactoryProduction && Allocation.To == this && Allocation.Quantity > 0) //~~ If allocation has a lower sequence than the check and is allocated to this structure ~~//
			{
				if (ResourceAvailable.Contains(Allocation.ResourceKey))		ResourceAvailable[Allocation.ResourceKey] = ResourceAvailable[Allocation.ResourceKey] + Allocation.Quantity;
				else														ResourceAvailable.Add(Allocation.ResourceKey, Allocation.Quantity);
			}
		}
	}

	

	for (auto& ResourceRequest : Request)
	{
		if (ResourceAvailable.Contains(ResourceRequest.Key))
		{
			int32 Remaining = ResourceRequest.Value;
			if (Remaining > ResourceAvailable[ResourceRequest.Key]) //~~ If request is larger than the resource pool ~~//
			{
				RequestMet = false;
				break;
			}
		}
		else
		{
			RequestMet = false;
			break;
		}
	}
	return RequestMet;
}


/******************** ProcessDecay *************************/
void APOTLStructure::ProcessDecay()
{
	// Wheat 4(+5)
	//			[0],[1],[2]
	// Wheat	+5,  4,  0

	// Wheat 4(+1) (+5-4)
	// Flour 0(+2)
	//			[0],[1],[2]
	// Wheat	+1,  4-4=0,  0
	// Flour	+2,  0,  0

	//? Production always adds to index zero, and billing always takes from last indexes. Consume from back through the array;

	//? Decay check will be mad after resource request are calculated. It is the only way that I know the amount of consumtion.

	//? Then make resource allocations based on MaxAge check

	//?? Reverse decay ??//
	//!! allocation have allready been reverse, but what about the queue? !!//
	//!! Backup/copy at start of each turn? !!//


	//DecayQueue
	// ID#1 [4,0,2,0,2]
	// ID#2 [2]
	// ID#3 [0,2]


	//~~ Overwrite decay queue with a copy of the backup ~~//
	DecayQueue = DecayQueueBackup;

	FString ADASdasd;

	//~~ Move freeResources up the decay queue ~~//
	for (auto& Decay : DecayQueue)
	{
		//FString ResourceKey = Decay.Key;
		TArray<int32>& Queue = Decay.Value;
		Queue.Insert(0, 0);
		/*
		for (int32 i = Queue.Num() - 1; i >= 0; i--)
		{

		}
		*/
	}

	

	//~~ Add production to decay queue ~~//
	for (auto& AllocatedResource : AllocatedResources)
	{
		FST_ResourceAllocation& Allocation = AllocatedResource.Value;
		if (Allocation.Type == EAllocationType::FactoryProduction)
		{
			static const FString ContextString(TEXT("GENERAL"));
			FST_Resource* ResourceData = GameInstance->DATA_Resources->FindRow<FST_Resource>(*Allocation.ResourceKey, ContextString);
			if (ResourceData->MaxAge == 0)
			{
				//int32 AllocationIndex = AllocateResource(nullptr, Allocation.ResourceKey, Allocation.Quantity, EAllocationType::Decay, -1, false, -1);
				//Allocation.Type = EAllocationType::Decay; //!! Almost right. But missing the decay information for displaying to the user (+1,-1). 
				Allocation.Type = EAllocationType::ProductionDecay;
			}
			if (DecayQueue.Contains(Allocation.ResourceKey))
			{
				TArray<int32>& Queue = DecayQueue[Allocation.ResourceKey];
				if (Queue.IsValidIndex(0))
				{
					Queue[0] += Allocation.Quantity;
				}
				else
				{
					Queue.Add(Allocation.Quantity);
				}
			}
			else {
				TArray<int32> Queue;
				Queue.Add(Allocation.Quantity);
				DecayQueue.Add(AllocatedResource.Value.ResourceKey, Queue);
			}
		}
		//~~ If bill and from root, then remove from decay que end ~~//
		else if (Allocation.Type == EAllocationType::FactoryBilling && Allocation.From == this)
		{
			if (DecayQueue.Contains(Allocation.ResourceKey))
			{
				TArray<int32>& Queue = DecayQueue[Allocation.ResourceKey];
				if (Queue.Num() > 0)
				{
					Queue[Queue.Num() - 1] -= Allocation.Quantity;
				}
			}
		}
	}


	/*
	//~~ Debug print ~~//
	int32 Count = 100;
	for (auto& Decay : DecayQueue)
	{
		Count++;
		FString ResourceKey = Decay.Key;
		TArray<int32>& Queue = Decay.Value;
		FString PrintString = ResourceKey + " : ";
		for (int32 i = 0; i < Queue.Num(); i++)
		{
			int32 QueuItem = Queue[i];
			PrintString += FString::FromInt(QueuItem) + ", ";
		}
		GEngine->AddOnScreenDebugMessage(Count, 60.0f, FColor::Yellow, PrintString);
	}
	*/


	//~~ Make decay allocations based on maxage and decay queue ~~//
	for (auto& Decay : DecayQueue)
	{
		FString ResourceKey = Decay.Key;
		TArray<int32>& Queue = Decay.Value;
		static const FString ContextString(TEXT("GENERAL"));
		FST_Resource* ResourceData = GameInstance->DATA_Resources->FindRow<FST_Resource>(*ResourceKey, ContextString);
		if (ResourceData && ResourceData->MaxAge != -1)
		{
			for (int32 i = Queue.Num() - 1; i >= 0; i--)
			{
				if (ResourceData->MaxAge == 0)
				{
					Queue.RemoveAt(i);
				}
				else if (i >= ResourceData->MaxAge)
				{
					int32 AllocationIndex = AllocateResource(nullptr, ResourceKey, Queue[i], EAllocationType::Decay, -1, true, -1);
					Queue.RemoveAt(i);
				}
			}
		}
	}

	/*
	//~~ Debug print ~~//
	Count = 200;
	for (auto& Decay : DecayQueue)
	{
		Count++;
		FString ResourceKey = Decay.Key;
		TArray<int32>& Queue = Decay.Value;
		FString PrintString = ResourceKey + " : ";
		for (int32 i = 0; i < Queue.Num(); i++)
		{
			int32 QueuItem = Queue[i];
			PrintString += FString::FromInt(QueuItem) + ", ";
		}
		GEngine->AddOnScreenDebugMessage(Count, 60.0f, FColor::Blue, PrintString);
	}
	*/
	

	// WP 3 - Production x 2
	// Wheat 1 - Production x 1

}


/******************** GetFreeKey *************************/
int32 APOTLStructure::MakeAllocationKey()
{
	int32 Key;
	Key = FMath::RandRange(0, 100000000);
	if (AllocatedResources.Contains(Key)) //~~ If key is already taken, then get new ~~//
	{
		Key = MakeAllocationKey();
	}
	return Key;
}



/******************** AllocateResource *************************/
int32 APOTLStructure::AllocateResource(APOTLStructure* To, FString ResourceKey, int32 Quantity, EAllocationType Type, int32 Sequence, bool Consume, int32 Key)
{
	if (Key >= 0)
	{
		FST_ResourceAllocation Allocation;
		Allocation.To = To;
		Allocation.From = this;
		Allocation.ResourceKey = ResourceKey;
		Allocation.Type = Type;
		Allocation.Quantity = Quantity;
		Allocation.Sequence = Sequence;
		//~~ Remove resources from FreeResources on this, not the caller ~~//
		if (Consume)
		{
			if (FreeResources.Contains(ResourceKey) && FreeResources[ResourceKey] >= Quantity)
			{
				FreeResources[ResourceKey] = FreeResources[ResourceKey] - Quantity;
				if (FreeResources[ResourceKey] == 0)
				{
					FreeResources.Remove(ResourceKey);
				}
			}
			else
			{
				bool Bug = true;
				return -1; //~~ If FreeResources doesn't have the resource. Just for safe handling ~~//
			}
		}
		//~~ Add the allocation to root for keeping ~~//
		if (this->Root->AllocatedResources.Contains(Key))
		{
			//~~ Overwrite existing allocation ~~//
			this->Root->AllocatedResources[Key] = Allocation;
		}
		else
		{
			this->Root->AllocatedResources.Add(Key, Allocation);
		}
	}
	else
	{
		//~~ Call itself to allocate the resource with the right keyindex ~~//
		Key = MakeAllocationKey();
		AllocateResource(To, ResourceKey, Quantity, Type, Sequence, Consume, Key);
	}
	return Key;
}



/******************** ResolveTree *************************/
TArray<int32> APOTLStructure::AllocateResources(APOTLStructure* To, TMap<FString, int32>& Resources, EAllocationType Type, int32 Sequence, bool Consume, int32 Key)
{
	TArray<int32> KeyIndexes;
	for (auto& Resource : Resources)
	{
		int32 KeyIndex = AllocateResource(To, Resource.Key, Resource.Value, Type, Sequence, Consume, -1);
		KeyIndexes.Add(KeyIndex);
	}
	return KeyIndexes;
}


/******************** RequestResources *************************/
void APOTLStructure::RequestResources(APOTLStructure* RequestFrom, UFactoryComponent* Factory, TMap<FString, int32>& Request, TMap<FString, int32>& Payoff, int32 Sequence, int32 Steps, EAllocationType Type, bool Consume, bool Bubble)
{
	FST_ResourceRequest ResourceRequest;
	ResourceRequest.From = RequestFrom;
	ResourceRequest.Factory = Factory;
	ResourceRequest.Request = Request;
	ResourceRequest.Payoff = Payoff;
	ResourceRequest.Sequence = Sequence;
	ResourceRequest.Steps = Steps;
	ResourceRequest.Type = Type;
	ResourceRequest.Consume = Consume;
	ResourceRequest.Bubble = Bubble;
	for (auto& Item : Request)
	{
		ResourceRequest.TestRequestQuantity += Item.Value;
	}
	for (auto& Item : Payoff)
	{
		ResourceRequest.TestPayoffQuantity += Item.Value;
	}
	ResourceRequests.Add(ResourceRequest);
}


/******************** GetStructureResourceFlow *************************/
TArray<FST_ResourceAllocation> APOTLStructure::GetStructureResourceFlow(APOTLStructure* Structure)
{
	TArray<FST_ResourceAllocation> ReturnList;
	if (ResourceFlowMap.Contains(Structure))
	{
		ReturnList = ResourceFlowMap[Structure];
	}
	return ReturnList;
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


/******************** ProcessBaseData *************************/
void APOTLStructure::ProcessBaseData()
{
	//StructureBaseData
	for (auto& Factory : StructureBaseData.Factories)
	{
		//FST_ResourceAllocation& Allocation = AllocatedResource.Value;
		UFactoryComponent* FactoryComponent = NewObject<UFactoryComponent>(this);
		FactoryComponent->RegisterComponent();
		FactoryComponent->Recipe = Factory.Recipe;
		FactoryComponent->MaxQuantity = Factory.MaxQuantity;
		Factories.Add(FactoryComponent);
	}
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



void APOTLStructure::OnTimeUpdate(float Time, float TimeProgressed)
{
	// Add to labor progress
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

