// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLStructure.h"
#include "POTLGameMode.h"
#include "POTLGameInstance.h"
#include "UStructureComponent.h"


// Sets default values for this component's properties
UStructureComponent::UStructureComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	ParentStructure = nullptr;
	bCanToggle = true;
	bIsOn = true;
	Progress = 0.f;
	TaskLength = 0.f;
}


/******************** ToggleOn *************************/
bool UStructureComponent::ToggleOn()
{

	return bIsOn;
}


/******************** OnTimeUpdate *************************/
void UStructureComponent::OnTimeUpdate(float Time, float TimeProgressed)
{
	if (bIsOn)
	{
		if (TaskLength > 0)
		{
			Progress += TimeProgressed;
			if (Progress >= TaskLength)
			{
				OnProgressComplete();
				Progress = 0.f;
				//OnProgressComplete.Broadcast();
			}
		}
	}
}


/******************** OnProgressComplete *************************/
void UStructureComponent::OnProgressComplete()
{

}


// Called when the game starts
void UStructureComponent::BeginPlay()
{
	Super::BeginPlay();

	APOTLStructure* Structure = Cast<APOTLStructure>(GetOwner());
	if (Structure)
	{
		ParentStructure = Structure;

		// Bind to time update
		APOTLGameMode* GameMode = Cast<APOTLGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			GameMode->OnTimeUpdated.AddDynamic(this, &UStructureComponent::OnTimeUpdate);
		}
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

