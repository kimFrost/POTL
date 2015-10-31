// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"


// Sets default values
APOTLStructure::APOTLStructure(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TreeId = FName(TEXT(""));
	HexIndex = -1;
	IsRoot = true;
	BroadcastRange = 0;
	StructureBaseData = FST_Structure{};

	//GameInstance = Cast<UPOTLGameInstance>(GetGameInstance()); //~~ <== Will crash. The game instance is not ready at this point ~~//


	/*
	GameInstance = Cast<UPOTLGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		// Add test resources
		Resources.Add(FName(TEXT("Wood")), 50.f);
		Resources.Add(FName(TEXT("Stone")), 50.f);

		// Add test factory for resource process

		FST_Factory Factory;
		Factory.Requirements.Add(FName(TEXT("Wood")), 2.f);
		Factories.Add(Factory);

		//Id = FName(TEXT(""));
		//Title = "";
		//UnitMass = 1.f;
		//Stackable = true;
		//Burnable = true;
		//StackSize = 64;
		//Quantity = 0;

		//HSSSS = nullptr;
		//HSSSS = FST_Hex{};
	}
	*/
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
	default:
		break;
	}
	return List;
}

/******************** GetResourcesAsList *************************/
void APOTLStructure::OptimizeAllocatedResources()
{
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
	bool Fulfilled = RequestResources(true, this, ResourceUpkeep, 0, EAllocationType::RequestDirect, false); //~~ Do self have the required resources ~~//
}


/******************** ProcessFactories *************************/
void APOTLStructure::ProcessFactories(bool Broadcast)
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
		for (auto& Factory : Factories)
		{
			Factory.ProcessInvoice(GameInstance->RecipeTable); //~~ Calculate requirements ~~//
			bool Fulfilled = RequestResources(false, this, Factory.Requirements, 0, EAllocationType::RequestDirect, false);
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
		for (auto& Factory : Factories)
		{
			TMap<FString, int32> FactoryProduction;
			TMap<FString, int32> FactoryBilling;
			Factory.Resolve(this, FreeResources, GameInstance->RecipeTable, FactoryProduction, FactoryBilling); //~~ Resolve factory and get the results/production ~~//
			RequestResources(false, this, FactoryBilling, 0, EAllocationType::FactoryBilling, true); //~~ RequestResources doens't know how to handle negative values ~~//
			for (auto& Resource : FactoryBilling)
			{
				//AllocateResource(this, Resource.Key, Resource.Value, EAllocationType::FactoryBilling);
			}
			for (auto& Resource : FactoryProduction)
			{
				AllocateResource(this, Resource.Key, Resource.Value, EAllocationType::FactoryProduction);
			}
		}
	}
}


/******************** ResolveTree *************************/
void APOTLStructure::ResolveTree()
{
	GEngine->AddOnScreenDebugMessage(100, 15.0f, FColor::Magenta, "ResolveTree()");
	ResolveUpkeep(true);
	ResolveAllocations(EAllocationType::RequestDirect, true); //~~ Resolve allocations type direct ~~//
	ResolveFactories(true);
	ResolveAllocations(EAllocationType::All, true); //~~ Resolve all other allocations ~~//

	/*
	FTimerHandle UniqueHandle;
	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &APOTLStructure::ResolveUpkeep, true);
	GetWorldTimerManager().SetTimer(UniqueHandle, RespawnDelegate, 5.0f, false);

	UniqueHandle;
	RespawnDelegate = FTimerDelegate::CreateUObject(this, &APOTLStructure::ResolveAllocations, EAllocationType::RequestDirect, true);
	GetWorldTimerManager().SetTimer(UniqueHandle, RespawnDelegate, 10.0f, false);

	UniqueHandle;
	RespawnDelegate = FTimerDelegate::CreateUObject(this, &APOTLStructure::ResolveFactories, true);
	GetWorldTimerManager().SetTimer(UniqueHandle, RespawnDelegate, 15.0f, false);

	UniqueHandle;
	RespawnDelegate = FTimerDelegate::CreateUObject(this, &APOTLStructure::ResolveAllocations, EAllocationType::All, true);
	GetWorldTimerManager().SetTimer(UniqueHandle, RespawnDelegate, 20.0f, false);
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
	//for (int32 i = 0; i < AllocatedResources.Num(); i++)
	for (int32 i = AllocatedResources.Num() - 1; i >= 0; i--)
	{
		FST_ResourceAllocation& Allocation = AllocatedResources[i];
		if (Type == EAllocationType::All)
		{
			Allocation.To->AddResource(Allocation.ResourceKey, Allocation.Quantity, EResourceList::Free);
			AllocatedResources.RemoveAt(i);
		}
		else
		{
			if (Allocation.Type == Type)
			{
				Allocation.To->AddResource(Allocation.ResourceKey, Allocation.Quantity, EResourceList::Free);
				AllocatedResources.RemoveAt(i);
			}
		}
	}
}


/******************** ResolveTree *************************/
int32 APOTLStructure::AllocateResource(APOTLStructure* From, FString ResourceKey, int32 Quantity, EAllocationType Type)
{
	FST_ResourceAllocation Allocation;
	Allocation.From = From;
	Allocation.To = this;
	Allocation.ResourceKey = ResourceKey;
	Allocation.Type = Type;
	Allocation.Quantity = Quantity;
	int32 Index = AllocatedResources.Add(Allocation);
	return Index;
}


/******************** RequestResources *************************/
bool APOTLStructure::RequestResources(bool Bubble, APOTLStructure* RequestFrom, TMap<FString, int32>& Request, int32 Steps, EAllocationType Type, bool Consume)
{
	TMap<FString, int32> RequestedResources; //~~ FString Id, Int32 Quantity ~~//
	bool RequestFulfilled = true;
	Steps++; //~~ Increase steps, resulting in more resource loss from many reroutes ~~//
	//~~ Should the emitTo structor require manpower to transport resources?? ~~//
	//~~ Handle request and Try to meet the resource request with own storage. If not then request parent of current ~~//
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
					AllocateResource(this, ResourceRequest.Key, FreeResources[ResourceRequest.Key], Type);
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
					AllocateResource(this, ResourceRequest.Key, ResourceRequest.Value, Type);
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
		bool Fulfilled = EmitTo->RequestResources(Bubble, RequestFrom, Request, Steps, EAllocationType::RequestDirect, Consume);
	}
	return RequestFulfilled; //~~ True / false ~~//
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
		FreeResources.Add(TEXT("Wood"), 9.f);
		FreeResources.Add(TEXT("Stone"), 15.f);

		// Add test factory for resource process
		FST_Factory Factory;
		//Factory.Invoice.Add(FName(TEXT("Plank"), 1));
		Factory.Invoice.Add(TEXT("Plank"), 1);
		Factories.Add(Factory);

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

