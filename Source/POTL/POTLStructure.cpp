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
	if (GameInstance != nullptr)
	{

	}
	*/

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
	case EResourceList::Storage:
		if (FreeResources.Contains(Id))
		{
			FreeResources[Id] += Quantity;
			Added = true;
		}
		else
		{
			FreeResources.Add(Id, Quantity);
			Added = true;
		}
		break;
	case EResourceList::Requirements:
		if (ResourceRequirements.Contains(Id))
		{
			ResourceRequirements[Id] += Quantity;
			Added = true;
		}
		else
		{
			ResourceRequirements.Add(Id, Quantity);
			Added = true;
		}
		break;
	case EResourceList::Alterations:
		if (ResourceAlterations.Contains(Id))
		{
			ResourceAlterations[Id] += Quantity;
			Added = true;
		}
		else
		{
			ResourceAlterations.Add(Id, Quantity);
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
	case EResourceList::Storage:
		for (auto& ResourceRequest : Resources)
		{
			FST_Resource Resource;
			Resource.Id = ResourceRequest.Key;
			Resource.Quantity = ResourceRequest.Value;
			List.Add(Resource);
		}
		break;
	case EResourceList::Requirements:
		for (auto& ResourceRequest : ResourceRequirements)
		{
			FST_Resource Resource;
			Resource.Id = ResourceRequest.Key;
			Resource.Quantity = ResourceRequest.Value;
			List.Add(Resource);
		}
		break;
	case EResourceList::Alterations:
		for (auto& ResourceRequest : ResourceAlterations)
		{
			FST_Resource Resource;
			Resource.Id = ResourceRequest.Key;
			Resource.Quantity = ResourceRequest.Value;
			List.Add(Resource);
		}
		break;
	default:
		break;
	}
	return List;
}


/******************** CalculateUpkeep *************************/
void APOTLStructure::CalculateUpkeep()
{
	//~~ Resolve children ~~//
	for (int32 i = 0; i < BroadcastTo.Num(); i++)
	{
		BroadcastTo[i]->CalculateUpkeep();
	}
	//~~ Resolve self / The function logic ~~//

}

/******************** ResolveUpkeep *************************/
void APOTLStructure::ResolveUpkeep(bool Broadcast)
{
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
	bool Fulfilled = RequestResources(false, this, ResourceUpkeep, ResourceAlterations, 0); //~~ Do self have the required resources ~~//
	if (EmitTo != nullptr)
	{
		if (!Fulfilled)		EmitTo->RequestResources(true, this, ResourceUpkeep, ResourceAlterations, 0); //~~ Else get resources from emitTo parent ~~//
	}
}

/******************** ResolveFactories *************************/
void APOTLStructure::ResolveFactories(bool Broadcast)
{
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
			Factory.ProcessInvoice(GameInstance->RecipeTable); //~~ Calculate requirements ~~//
			bool Fulfilled = RequestResources(false, this, Factory.Requirements, Factory.Allocations, 0);
			if (!Fulfilled)		EmitTo->RequestResources(true, this, Factory.Requirements, Factory.Allocations, 0);

			//~~ Resources are not being drained from the source ???

			//Factory.Resolve(ResourceAlterations, this, GameInstance->RecipeTable, AllocatedResources); //~~ Resolve factory and get the results/production ~~//
			Factory.Resolve(ResourceAlterations, this, GameInstance->RecipeTable, AllocatedResources); //~~ Resolve factory and get the results/production ~~//
			
			//Factory.Resolve(TMap<FString, int32>& StructureFreeResources, UDataTable* RecipeTable, const TMap<FString, int32>& Production)

			// Use parameters for production and leftovers
			// Then make resource allocations from here
			//AllocateResource(this, Key, Value, EAllocationType::FactoryLeftover);
			//AllocateResource(this, Key, Value, EAllocationType::FactoryProduction);

		}
	}
}


/******************** ResolveTree *************************/
void APOTLStructure::ResolveTree()
{
	ResolveUpkeep(true);
	ResolveAllocations(EAllocationType::RequestDirect, true); //~~ Resolve allocations type direct ~~//
	ResolveFactories(true); 
	ResolveAllocations(EAllocationType::None, true); //~~ Resolve all other allocations ~~//
}


/******************** ResolveAllocations *************************/
void APOTLStructure::ResolveAllocations(EAllocationType Type, bool Broadcast)
{
	//~~ Resolve children ~~//
	if (Broadcast)
	{
		for (int32 i = 0; i < BroadcastTo.Num(); i++)
		{
			BroadcastTo[i]->ResolveAllocations(Type, Broadcast);
		}
	}
	//~~ Resolve self / The function logic ~~//
	for (int32 i = 0; i < AllocatedResources.Num(); i++)
	{
		if (Type == EAllocationType::None)
		{

		}
		else
		{

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
bool APOTLStructure::RequestResources(bool Bubble, APOTLStructure* RequestFrom, TMap<FString, int32>& Request, TMap<FString, int32>& Allocations, int32 Steps)
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
			if (ResourceRequest.Value > FreeResources[ResourceRequest.Key]) //~~ If request is larger than the resource pool ~~//
			{
				//!! Use AllocateResource instead. But Allocations parameters properly need to be changed

				//AllocateResource(this, ResourceRequest.Key, ResourceRequest.Value, EAllocationType::RequestDirect);

				//bool Success = RequestFrom->AddResource(ResourceRequest.Key, Resources[ResourceRequest.Key], EResourceList::Alterations);
				if (Allocations.Contains(ResourceRequest.Key))	Allocations[ResourceRequest.Key] += FreeResources[ResourceRequest.Key];
				else											Allocations.Add(ResourceRequest.Key, FreeResources[ResourceRequest.Key]);
				ResourceRequest.Value -= FreeResources[ResourceRequest.Key];
				FreeResources[ResourceRequest.Key] = 0;
				//RequestedResources.Add(ResourceRequest.Key, ResourceRequest.Value);
				FreeResources.Remove(ResourceRequest.Key); //~~ Remove the empty resource ~~//
				RequestFulfilled = false;

			}
			else if (ResourceRequest.Value <= FreeResources[ResourceRequest.Key]) //~~ If request is less or equal to the resource pool ~~//
			{
				AllocateResource(this, ResourceRequest.Key, ResourceRequest.Value, EAllocationType::RequestDirect);

				//bool Success = RequestFrom->AddResource(ResourceRequest.Key, ResourceRequest.Value, EResourceList::Alterations);
				if (Allocations.Contains(ResourceRequest.Key))	Allocations[ResourceRequest.Key] += FreeResources[ResourceRequest.Key];
				else											Allocations.Add(ResourceRequest.Key, FreeResources[ResourceRequest.Key]);
				FreeResources[ResourceRequest.Key] = FreeResources[ResourceRequest.Key] - ResourceRequest.Value;
				ResourceRequest.Value = 0;
			}
		}
	}

	//~~ if Bubble then then RequestResources on parent/emitTo. ~~//
	//~~ and resource request haven't been met. ~~//
	if (Bubble &&
	!RequestFulfilled &&
	EmitTo != nullptr)
	{
		//TMap<FName, FST_Resource> ResourcesFromParent = EmitTo->RequestResources(Bubble, this, RequestedResources, Steps);
		//bool Fulfilled = EmitTo->RequestResources(Bubble, this, RequestedResources, Steps);
		bool Fulfilled = EmitTo->RequestResources(Bubble, this, Request, Allocations, Steps);
		//~~ Combine requested resources ~~//
	}


	/*
	//~~ TEMP Move alterations to resources. Should go through a provider/production chain with a resolver ~~//
	for (auto& RequestedResource : RequestedResources)
	{

	}
	*/

	//return RequestedResources;
	return RequestFulfilled; //~~ True / false ~~//
}


//~~ Called when the game starts or when spawned ~~//
void APOTLStructure::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UPOTLGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		// Add test resources
		FreeResources.Add(TEXT("Wood"), 50.f);
		FreeResources.Add(TEXT("Stone"), 50.f);

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

}

//~~ Called every frame ~~//
void APOTLStructure::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

