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

	TArray<FST_Resource> TestResourcesRequest;

	if (EmitTo != nullptr)
	{
		EmitTo->RequestResources(true, this, TestResourcesRequest, 0);
	}

	// Broadcast resources to children/broadcastTo

}

TArray<FST_Resource> APOTLStructure::RequestResources(bool Bubble, const APOTLStructure* RequestFrom, const TArray<FST_Resource>& Request, int32 Steps)
{
	TArray<FST_Resource> RequestedResources;
	bool RequestFulfilled = true;
	Steps++; // Increase steps, resulting in more resource loss from many reroutes

	// Should the emitTo structor require manpower to transport resources?? 

	// Handle request and Try to meet the resource request

	// Rewrite from TArray to TMap
	/*
	for (int32 i = 0; i < Request.Num(); i++)
	{
		FST_Resource& RequestResource = Resources[i];
		for (int32 ii = Resources.Num() - 1; ii >= 0; ii--) // Loop the stored resources in reverse
		{
			FST_Resource& Resource = Resources[ii];
			if (RequestResource.Id == Resource.Id)
			{
				if (RequestResource.Quantity > Resource.Quantity)
				{
					RequestResource.Quantity = RequestResource.Quantity - Resource.Quantity;
					Resource.Quantity = 0;
					RequestedResources.Add(Resource);
					RequestFulfilled = false;
					Resources.RemoveAt(ii); // Remove the empty resource
				}
				else if (RequestResource.Quantity <= Resource.Quantity)
				{
					RequestedResources.Add(Resource);
					Resource.Quantity = Resource.Quantity - RequestResource.Quantity;
					RequestResource.Quantity = 0;
				}
			}
		}
	}
	*/


	// Clean resources, if fx is empty, then remove from list


	// if Bubble then then RequestResources on parent/emitTo.
	// and resource request haven't been met.
	if (Bubble &&
	RequestFulfilled &&
	EmitTo != nullptr)
	{
		
		TArray<FST_Resource> ResourcesFromParent = EmitTo->RequestResources(Bubble, this, RequestedResources, Steps);
		//EmitTo->RequestResources(Bubble, this);

		// Combine requested resources

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

