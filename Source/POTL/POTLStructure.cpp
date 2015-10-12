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
	FST_Resource TestResource;
	TestResource.Id = FName(TEXT("Wood"));
	TestResource.Quantity = 100.f;
	//Resources.Add(TestResource);
	Resources.Add(FName(TEXT("Wood")), TestResource);

	// Add test resources
	TestResource = FST_Resource{};
	TestResource.Id = FName(TEXT("Stone"));
	TestResource.Quantity = 50.f;
	Resources.Add(FName(TEXT("Stone")), TestResource);


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
bool APOTLStructure::AddResource(FST_Resource Resource, EResourceList Type)
{
	bool Added = false;
	switch (Type)
	{
	case EResourceList::Storage:
		if (Resources.Contains(Resource.Id))
		{
			Resources[Resource.Id].Quantity += Resource.Quantity; // No check for storage and negative size yet ?
			Added = true;
		}
		else
		{
			Resources.Add(Resource.Id, Resource);
			Added = true;
		}
		break;
	case EResourceList::Requirements:
		if (ResourceRequirements.Contains(Resource.Id))
		{
			ResourceRequirements[Resource.Id].Quantity += Resource.Quantity; // No check for storage and negative size yet ?
			Added = true;
		}
		else
		{
			ResourceRequirements.Add(Resource.Id, Resource);
			Added = true;
		}
		break;
	case EResourceList::Alterations:
		if (ResourceAlterations.Contains(Resource.Id))
		{
			ResourceAlterations[Resource.Id].Quantity += Resource.Quantity; // No check for storage and negative size yet ?
			Added = true;
		}
		else
		{
			ResourceAlterations.Add(Resource.Id, Resource);
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
			List.Add(ResourceRequest.Value);
		}
		break;
	case EResourceList::Requirements:
		for (auto& ResourceRequest : ResourceRequirements)
		{
			List.Add(ResourceRequest.Value);
		}
		break;
	case EResourceList::Alterations:
		for (auto& ResourceRequest : ResourceAlterations)
		{
			List.Add(ResourceRequest.Value);
		}
		break;
	default:
		break;
	}
	return List;
}


/******************** ResolveTree *************************/
void APOTLStructure::ResolveTree(bool Bubble)
{
	// Resolve children
	for (int32 i = 0; i < BroadcastTo.Num(); i++)
	{
		BroadcastTo[i]->ResolveTree(Bubble);
	}
	// Resolve self
	// Request resources from parent/emitTo

	/*
	TMap<FName, FST_Resource> TestResourcesRequest;
	FST_Resource TestResourceRequest;
	TestResourceRequest.Id = FName(TEXT("Stone"));
	TestResourceRequest.Quantity = 1.f;
	TestResourcesRequest.Add(FName(TEXT("Stone")), TestResourceRequest);
	*/

	if (EmitTo != nullptr)
	{
		TMap<FName, FST_Resource> RequestedResources = EmitTo->RequestResources(true, this, ResourceRequirements, 0);
		// Store RequestedResources in self?

	}

	// Broadcast resources to children/broadcastTo

}


/******************** RequestResources *************************/
TMap<FName, FST_Resource> APOTLStructure::RequestResources(bool Bubble, APOTLStructure* RequestFrom, TMap<FName, FST_Resource>& Request, int32 Steps)
{
	TMap<FName, FST_Resource> RequestedResources;
	bool RequestFulfilled = true;
	Steps++; // Increase steps, resulting in more resource loss from many reroutes

	// Should the emitTo structor require manpower to transport resources?? 

	// Handle request and Try to meet the resource request with own storage // Or parent
	for (auto& ResourceRequest : Request)
	{
		//ResourceRequest.Key
		//ResourceRequest.Value
		//ResourceRequest.Remove(ResourceRequest.Key)
		if (Resources.Contains(ResourceRequest.Key))
		{
			if (ResourceRequest.Value.Quantity > Resources[ResourceRequest.Key].Quantity)
			{
				bool Success = RequestFrom->AddResource(ResourceRequest.Value, EResourceList::Storage); // May send too large value ?
				ResourceRequest.Value.Quantity = ResourceRequest.Value.Quantity - Resources[ResourceRequest.Key].Quantity;
				Resources[ResourceRequest.Key].Quantity = 0;
				RequestedResources.Add(ResourceRequest.Key, ResourceRequest.Value);
				Resources.Remove(ResourceRequest.Key); // Remove the empty resource
				
				// Use resource alterations list instead

				// Should resource requirement list be emptied and then refilled by the structure again? // Mayby yes. Then the calculation is propperly correct each time. // 
				// Then the structure and turnBegin will calculate their resource requirements

				//RequestFrom->Resources.Add // Add requested resource to structures resource alterations ?
			}
			else if (ResourceRequest.Value.Quantity <= Resources[ResourceRequest.Key].Quantity)
			{
				bool Success = RequestFrom->AddResource(ResourceRequest.Value, EResourceList::Storage); 
				RequestedResources.Add(ResourceRequest.Key, ResourceRequest.Value);
				Resources[ResourceRequest.Key].Quantity = Resources[ResourceRequest.Key].Quantity - ResourceRequest.Value.Quantity;
				ResourceRequest.Value.Quantity = 0;
			}
		}
	}



	// if Bubble then then RequestResources on parent/emitTo.
	// and resource request haven't been met.
	if (Bubble &&
	RequestFulfilled &&
	EmitTo != nullptr)
	{
		TMap<FName, FST_Resource> ResourcesFromParent = EmitTo->RequestResources(Bubble, this, RequestedResources, Steps);
		// Combine requested resources

	}

	// Store RequestedResources in self?
	for (auto& RequestedResource : RequestedResources)
	{

	}

	return RequestedResources;
}


// Called when the game starts or when spawned
void APOTLStructure::BeginPlay()
{
	Super::BeginPlay();

	/*********** BINDINGS **************/
	//UPOTLGameInstance::OnTurnSwitched.AddDynamic(this, &APOTLStructure::RequestResources);
	//UPOTLGameInstance::OnTurnSwitched.Add(this, &APOTLStructure::RequestResources);

}

// Called every frame
void APOTLStructure::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

