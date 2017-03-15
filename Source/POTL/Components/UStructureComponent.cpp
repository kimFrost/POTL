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
	bIsOn = false;
}


/******************** ToggleOn *************************/
bool UStructureComponent::ToggleOn()
{
	bIsOn = !bIsOn;
	return bIsOn;
}


/******************** Init *************************/
void UStructureComponent::Init()
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
	}
	else
	{
		// Failed to find parent structure
		bIsOn = false;
	}

	OnInit();
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

