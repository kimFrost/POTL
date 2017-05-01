// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLStructure.h"
#include "POTLGameMode.h"
#include "POTLGameInstance.h"
#include "UObjects/UHexTile.h"
#include "UStructureComponent.h"


// Sets default values for this component's properties
UStructureComponent::UStructureComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	ParentStructure = nullptr;
	bIsWorking = false;
	bCanToggle = true;
	bIsOn = false;
	bRequireAttached = true;
}


/******************** ToggleOn *************************/
bool UStructureComponent::ToggleOn()
{
	bIsOn = !bIsOn;
	return bIsOn;
}

/*
void UStructureComponent::EnterEditMode()
{
	// Loop hexesInRange and bind to allocate delegate if not allocated to other than this
	for (auto& Hex : HexesInRange)
	{
		if (Hex)
		{
			Hex->OnHexToggleAllocate.AddDynamic(this, &UStructureComponent::ToggleAllocateHex);
		}
	}
}

void UStructureComponent::LeaveEditMode()
{
	for (auto& Hex : HexesInRange)
	{
		if (Hex)
		{
			Hex->OnHexToggleAllocate.RemoveDynamic(this, &UStructureComponent::ToggleAllocateHex);
		}
	}
}

void UStructureComponent::ToggleAllocateHex(UHexTile* Hex)
{
	if (Hex)
	{
		if (AllocatedHexes.Contains(Hex))
		{
			//Hex->UnAllocateTo(this);
			Hex->AllocatedTo = nullptr;
			AllocatedHexes.Remove(Hex);
		}
		else
		{
			if (!Hex->AllocatedTo)
			{
				// Binding?
				//Hex->AllocateTo(this);
				Hex->AllocatedTo = this;
				AllocatedHexes.Add(Hex);
			}
		}
	}
}
*/

/******************** Init *************************/
void UStructureComponent::Init()
{
	if (!bIsInitialized)
	{
		APOTLStructure* Structure = Cast<APOTLStructure>(GetOwner());
		if (Structure)
		{
			ParentStructure = Structure;
			bIsOn = true;

			// Bind to time update
			APOTLGameMode* GameMode = Cast<APOTLGameMode>(GetWorld()->GetAuthGameMode());
			if (GameMode)
			{
				GameMode->OnTimeUpdated.AddDynamic(this, &UStructureComponent::OnTimeUpdate);
			}

			// Store hexes in range and allocated hexes
			if (ParentStructure)
			{
				HexesInRange = ParentStructure->HexesInRange;
				AllocatedHexes = ParentStructure->AllocatedHexes;
			}
		}
		else
		{
			// Failed to find parent structure
			bIsOn = false;
		}

		bIsInitialized = true;
		OnInit();
	}
}


/******************** OnTimeUpdate *************************/
void UStructureComponent::OnTimeUpdate(float Time, float TimeProgressed)
{
	
}


/******************** OnInit *************************/
void UStructureComponent::OnInit_Implementation()
{

}


// Called when the game starts
void UStructureComponent::BeginPlay()
{
	Super::BeginPlay();


}


// Called every frame
void UStructureComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

