// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLStructure.h"
#include "UStructureComponent.h"


// Sets default values for this component's properties
UStructureComponent::UStructureComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	ParentStructure = nullptr;
	bCanToggle = true;
	bIsOn = true;
	Progress = 0.f;
}


bool UStructureComponent::ToggleOn()
{

	return bIsOn;
}

// Called when the game starts
void UStructureComponent::BeginPlay()
{
	Super::BeginPlay();

	APOTLStructure* Structure = Cast<APOTLStructure>(GetOwner());
	if (Structure)
	{
		ParentStructure = Structure;
	}
	else 
	{
		bIsOn = false;
		// Failed to find parent structure
	}

}


// Called every frame
void UStructureComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

