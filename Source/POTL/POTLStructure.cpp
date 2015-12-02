// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "FactoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "POTLStructure.h"


// Sets default values
APOTLStructure::APOTLStructure(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TreeId = TEXT("");
	HexIndex = -1;
	CubeCoord = { -1, -1, -1 };
	IsRoot = true;
	BroadcastRange = 0;
	StructureBaseData = FST_Structure{};
	StructureRowName = TEXT("");
	IsPlaceholder = false;
	BlockPathing = true;
	IsUnderConstruction = true;
	InRangeOfEmitTo = false;
	Root = this;

	//GameInstance = Cast<UPOTLGameInstance>(GetGameInstance()); //~~ <== Will crash. The game instance is not ready at this point ~~//
}

//AVehicle(const class FPostConstructInitializeProperties& PCIP, FString Path, FString Name);


/*****************************************************************************************************/
/******************************************* RESOURCES ***********************************************/
/*****************************************************************************************************/


/******************** AddResource *************************/
bool APOTLStructure::AddResource(FString Id, int32 Quantity, EResourceList Type)
{
	bool Added = false;
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
	return Added;
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

/******************** GetResourcesAsList *************************/
void APOTLStructure::OptimizeAllocatedResources()
{
	//!! Not needed anymore. Switched from tarray to tmap
	/*
	TMap<FString, FST_ResourceAllocation> TempResourceHolder;
	for (int32 i = 0; i < AllocatedResources.Num(); i++)
	{
		if (TempResourceHolder.Contains(AllocatedResources[i].ResourceKey))		TempResourceHolder[AllocatedResources[i].ResourceKey].Quantity += AllocatedResources[i].Quantity;
		else																	TempResourceHolder.Add(AllocatedResources[i].ResourceKey, AllocatedResources[i]);
	}
	AllocatedResources.Empty();
	for (auto& Resource : TempResourceHolder)
	{
		AllocatedResources.Add(Resource.Value);
	}
	*/
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
					int32 Sequence = Factory->ProcessInvoice(GameInstance->DATA_Recipes);
					//RequestResources(this, Factory->Requirements, 0, Sequence, EAllocationType::RequestDirect, false, true);
					this->Root->RequestResources(this, Factory, Factory->Requirements, Factory->Invoice, Sequence, 0, EAllocationType::RequestDirect, false, true);
				}
			}
		}
	}
}

/******************** ResolveFactories *************************/
void APOTLStructure::ResolveFactories(bool Broadcast)
{
	GEngine->AddOnScreenDebugMessage(100, 15.0f, FColor::Magenta, "ResolveFactories()");
	//~~ Resolve children ~~//
	if (Broadcast)
	{
		for (int32 i = 0; i < BroadcastTo.Num(); i++)
		{
			BroadcastTo[i]->ResolveFactories(Broadcast);
		}
	}
	//~~ Resolve self / The function logic ~~//
	//~~ Resolve factories ~~//
	if (GameInstance)
	{
		for (UFactoryComponent* Factory : Factories)
		{
			if (Factory)
			{
				TMap<FString, int32> FactoryProduction;
				TMap<FString, int32> FactoryBilling;
				Factory->Resolve(this, FreeResources, GameInstance->DATA_Recipes, FactoryProduction, FactoryBilling); //~~ Resolve factory and get the results/production ~~//
				//Factory->Resolve(this, Root->FreeResources, GameInstance->DATA_Recipes, FactoryProduction, FactoryBilling); //~~ Resolve factory and get the results/production ~~//
				RequestResources(this, Factory, FactoryBilling, Factory->Invoice, 0, 100, EAllocationType::FactoryBilling, true, true); //~~ RequestResources doens't know how to handle negative values ~~//
				for (auto& Resource : FactoryProduction)
				{
					AllocateResource(this, Resource.Key, Resource.Value, EAllocationType::FactoryProduction, false, -1);
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
		CalculateUpkeep(true);
		ProcessFactories(true);
		ProcessResourceRequests();
	}
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
	//~~ Init SortedResourceRequests ~~//
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
	}
	//~~ Allocate and process the requests ~~//
	for (i = 0; i <= HighestSequence; i++) //~~ Handle the request from sequence zero and up ~~//
	{
		TArray<FST_ResourceRequest>& RequestList = SortedResourceRequests[i];
		for (int32 ii = 0; ii < RequestList.Num(); ii++)
		{
			FST_ResourceRequest& ResourceRequest = RequestList[ii];
			//~~ If sequene is over zero, then check previous request for their production ~~//
			if (i > 0) 
			{
				FST_ResourceRequest ResourceRequestCopy = ResourceRequest;
				//if (ResourceRequest.Payoff && ResourceRequest.RequestMet)
			}
			else
			{
				if (HasResourcesAvailable(ResourceRequest.Request)) //~~ If self has the resources required ~~//
				{
					for (auto& ResourceRequest : ResourceRequest.Request)
					{
						AllocateResource(this, ResourceRequest.Key, ResourceRequest.Value, EAllocationType::FactoryBilling, false, -1);
					}
				}
			}
		}
	}


	// What about a request gets resources from multiple sources 
}



/******************** HasResourcesAvailable *************************/
bool APOTLStructure::HasResourcesAvailable(TMap<FString, int32>& Request)
{
	bool RequestMet = true;
	for (auto& ResourceRequest : Request)
	{
		if (FreeResources.Contains(ResourceRequest.Key))
		{
			if (ResourceRequest.Value > FreeResources[ResourceRequest.Key]) //~~ If request is larger than the resource pool ~~//
			{
				RequestMet = false;
				break;
			}
		}
	}
	return RequestMet;
}


/******************** ResolveTree *************************/
void APOTLStructure::ResolveTree() //~~ Should only for be called on root structures
{
	if (!IsPlaceholder)
	{
		//ResolveUpkeep(true);
		//ResolveAllocations(EAllocationType::RequestDirect, true); //~~ Resolve allocations type direct ~~//
		//ResolveFactories(true);
		//ResolveAllocations(EAllocationType::All, true); //~~ Resolve all other allocations ~~//
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
	/*
	for (auto& AllocatedResource : AllocatedResources)
	{
		FST_ResourceAllocation& Allocation = AllocatedResource.Value;
		if (Type == EAllocationType::All)
		{
			Allocation.To->AddResource(Allocation.ResourceKey, Allocation.Quantity, EResourceList::Free);
			AllocatedResources.Remove(AllocatedResource.Key);
		}
		else
		{
			if (Allocation.Type == Type)
			{
				Allocation.To->AddResource(Allocation.ResourceKey, Allocation.Quantity, EResourceList::Free);
				AllocatedResources.Remove(AllocatedResource.Key);
			}
		}
	}
	*/
	//~~ Check for sequence and handle it ~~// //!! Will I even need this for resolve. 
	/*
	for (i = 0; i < 8; i++) //~~ 8 is just a random static guess. Should be higher properly ~~//
	{
		for (auto& AllocatedResource : AllocatedResources)
		{
			if (Type == EAllocationType::Sequence)
			{
				FST_ResourceAllocation& Allocation = AllocatedResource.Value;
				if (Allocation.Sequence == i) //~~ Resolve allocation with sequence, if same as static loop integer ~~//
				{
					Allocation.To->AddResource(Allocation.ResourceKey, Allocation.Quantity, EResourceList::Free);
					AllocatedResources.Remove(AllocatedResource.Key);
				}
			}
		}
	}
	*/
}


/******************** ResolveTree *************************/
int32 APOTLStructure::AllocateResource(APOTLStructure* From, FString ResourceKey, int32 Quantity, EAllocationType Type, bool KeyLoop, int32 Key)
{
	if (Key >= 0)
	{
		if (!KeyLoop)
		{
			FST_ResourceAllocation Allocation;
			Allocation.From = From;
			//Allocation.To = this;
			if (this->IsRoot)
			{
				Allocation.To = this; //~~ Always send to root for now ~~//
			}
			else
			{
				Allocation.To = this->Root;
			}
			Allocation.ResourceKey = ResourceKey;
			Allocation.Type = Type;
			Allocation.Quantity = Quantity;
			if (AllocatedResources.Contains(Key))
			{
				//~~ Overwrite existing allocation ~~//
				AllocatedResources[Key] = Allocation;
			}
			else 
			{
				AllocatedResources.Add(Key, Allocation);
			}
		}
		return Key;
	}
	else
	{
		//~~ Generate random key ~~//
		int32 KeyIndex = FMath::RandRange(0, 1000000);
		if (AllocatedResources.Contains(KeyIndex)) //~~ If key is already taken, then get new ~~//
		{
			KeyIndex = AllocateResource(From, ResourceKey, Quantity, Type, true, Key);
		}
		//~~ Call itself to allocate the resource with the right keyindex ~~//
		AllocateResource(From, ResourceKey, Quantity, Type, false, KeyIndex);
		return KeyIndex;
	}
}

/******************** ResolveTree *************************/
TArray<int32> APOTLStructure::AllocateResources(APOTLStructure* From, TMap<FString, int32>& Resources, EAllocationType Type, int32 Key)
{
	TArray<int32> KeyIndexes;
	for (auto& Resource : Resources)
	{
		int32 KeyIndex = AllocateResource(From, Resource.Key, Resource.Value, Type, false, -1);
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
	ResourceRequests.Add(ResourceRequest);

	/*
	TMap<FString, int32> RequestedResources; //~~ FString Id, Int32 Quantity ~~//
	bool RequestFulfilled = true;
	Steps++; //~~ Increase steps, resulting in more resource loss from many reroutes ~~//
	//~~ Should the emitTo structor require manpower to transport resources?? ~~//
	//~~ Handle request and Try to meet the resource request with own storage. If not then request parent of current ~~//½

	//~~ Check allocated with a lower sequence number resources first ~~//

	//~~ Then check FreeResources ~~//
	for (auto& ResourceRequest : Request)
	{
		if (FreeResources.Contains(ResourceRequest.Key))
		{
			if (FreeResources[ResourceRequest.Key] == 0) //~~ if resource pool is 0 ~~//
			{
				FreeResources.Remove(ResourceRequest.Key);
				RequestFulfilled = false;
			}
			else if (ResourceRequest.Value > FreeResources[ResourceRequest.Key]) //~~ If request is larger than the resource pool ~~//
			{
				if (!Consume)
				{
					AllocateResource(this, ResourceRequest.Key, FreeResources[ResourceRequest.Key], Type, false, -1);
				}
				ResourceRequest.Value -= FreeResources[ResourceRequest.Key];
				FreeResources[ResourceRequest.Key] = 0;
				FreeResources.Remove(ResourceRequest.Key); //~~ Remove the empty resource ~~//
				RequestFulfilled = false;
			}
			else if (ResourceRequest.Value <= FreeResources[ResourceRequest.Key]) //~~ If request is less or equal to the resource pool ~~//
			{
				if (!Consume)
				{
					AllocateResource(this, ResourceRequest.Key, ResourceRequest.Value, Type, false, -1);
				}
				FreeResources[ResourceRequest.Key] = FreeResources[ResourceRequest.Key] - ResourceRequest.Value;
				ResourceRequest.Value = 0;
				if (FreeResources[ResourceRequest.Key] == 0)
				{
					FreeResources.Remove(ResourceRequest.Key); 
				}
			}
		}
	}

	//~~ if Bubble then then RequestResources on parent/emitTo. ~~//
	//~~ and resource request haven't been met. ~~//
	if (Bubble &&
	!RequestFulfilled &&
	EmitTo != nullptr)
	{
		EmitTo->RequestResources(Bubble, RequestFrom, Request, Steps, EAllocationType::RequestDirect, Consume);
	}
	*/
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



void APOTLStructure::BindToDelegate(float Turn)
{

}


//~~ Called when the game starts or when spawned ~~//
void APOTLStructure::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UPOTLGameInstance>(GetGameInstance());
	if (GameInstance)
	{
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

