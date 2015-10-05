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

	//HSSSS = nullptr;
	//HSSSS = FST_Hex{};
}

//AVehicle(const class FPostConstructInitializeProperties& PCIP, FString Path, FString Name);


/*****************************************************************************************************/
/******************************************* RESOURCES ***********************************************/
/*****************************************************************************************************/

/******************** ResolveTree *************************/
void APOTLStructure::ResolveTree()
{
	// Resolve children
	for (int32 i = 0; i < BroadcastTo.Num(); i++)
	{
		BroadcastTo[i]->ResolveTree(); 
	}
	// Resolve self

	// Request resources from parent/emitTo

	// Broadcast resources to children/broadcastTo

}




// Called when the game starts or when spawned
void APOTLStructure::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APOTLStructure::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

