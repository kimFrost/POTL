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

	// Add test resources
	//FST_Resource TestResource;
	//TestResource.Id = FName(TEXT("Wood"));
	//TestResource.Quantity = 100.f;
	Resources.Add(FName(TEXT("Wood")), 100.f);

	// Add test resources
	//TestResource = FST_Resource{};
	//TestResource.Id = FName(TEXT("Stone"));
	//TestResource.Quantity = 50.f;
	Resources.Add(FName(TEXT("Stone")), 50.f);


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

//AVehicle(const class FPostConstructInitializeProperties& PCIP, FString Path, FString Name);


/*****************************************************************************************************/
/******************************************* RESOURCES ***********************************************/
/*****************************************************************************************************/


/******************** AddResource *************************/
bool APOTLStructure::AddResource(FName Id, int32 Quantity, EResourceList Type)
{
	bool Added = false;
	switch (Type)
	{
	case EResourceList::Storage:
		if (Resources.Contains(Id))
		{
			Resources[Id] += Quantity;
			Added = true;
		}
		else
		{
			Resources.Add(Id, Quantity);
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


/******************** ResolveTree *************************/
void APOTLStructure::ResolveTree()
{
	//~~ Resolve children ~~//
	for (int32 i = 0; i < BroadcastTo.Num(); i++)
	{
		BroadcastTo[i]->ResolveTree();
	}
	//~~ Resolve self ~~//
	//~~ Request resources from self, then parent/emitTo ~~//
	if (EmitTo != nullptr) 
	{
		bool Fulfilled = RequestResources(false, this, ResourceRequirements, 0); //~~ Do self have the required resources ~~//
		if (!Fulfilled)
		{
			EmitTo->RequestResources(true, this, ResourceRequirements, 0); //~~ Else get resources from emitTo parent ~~//
		}
	}
	//~~ Broadcast resources to children/broadcastTo ? ~~//
}


/******************** RequestResources *************************/
bool APOTLStructure::RequestResources(bool Bubble, APOTLStructure* RequestFrom, TMap<FName, int32>& Request, int32 Steps)
{
	TMap<FName, int32> RequestedResources; //~~ FName Id, Int32 Quantity ~~//
	bool RequestFulfilled = true;
	Steps++; //~~ Increase steps, resulting in more resource loss from many reroutes ~~//

	//~~ Should the emitTo structor require manpower to transport resources?? ~~//

	//~~ Handle request and Try to meet the resource request with own storage. If not then request parent of current ~~//
	for (auto& ResourceRequest : Request)
	{
		if (Resources.Contains(ResourceRequest.Key))
		{
			if (ResourceRequest.Value > Resources[ResourceRequest.Key]) //~~ If request is larger than the resource pool ~~//
			{
				bool Success = RequestFrom->AddResource(ResourceRequest.Key, Resources[ResourceRequest.Key], EResourceList::Alterations);
				ResourceRequest.Value -= Resources[ResourceRequest.Key];
				Resources[ResourceRequest.Key] = 0;
				//RequestedResources.Add(ResourceRequest.Key, ResourceRequest.Value);
				Resources.Remove(ResourceRequest.Key); //~~ Remove the empty resource ~~//
				RequestFulfilled = false;
				//TODO: Use resource alterations list instead

				//~~ Should resource requirement list be emptied and then refilled by the structure again? // Mayby yes. Then the calculation is propperly correct each time. ~~// 
				//~~ Then the structure on turnBegin will calculate their resource requirements ~~//

				//RequestFrom->Resources.Add // Add requested resource to structures resource alterations ?
			}
			else if (ResourceRequest.Value <= Resources[ResourceRequest.Key]) //~~ If request is less or equal to the resource pool ~~//
			{
				bool Success = RequestFrom->AddResource(ResourceRequest.Key, ResourceRequest.Value, EResourceList::Alterations);
				//RequestedResources.Add(ResourceRequest.Key, ResourceRequest.Value);
				Resources[ResourceRequest.Key] = Resources[ResourceRequest.Key] - ResourceRequest.Value;
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
		bool Fulfilled = EmitTo->RequestResources(Bubble, this, Request, Steps);
		//~~ Combine requested resources ~~//
		 
	}

	/*
	//~~ Store RequestedResources in self? ~~//
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

	/*********** BINDINGS **************/
	//UPOTLGameInstance::OnTurnSwitched.AddDynamic(this, &APOTLStructure::RequestResources);
	//UPOTLGameInstance::OnTurnSwitched.Add(this, &APOTLStructure::RequestResources);

}

//~~ Called every frame ~~//
void APOTLStructure::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

