// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLStructure.h"
#include "POTLGameMode.h"
#include "POTLGameInstance.h"
#include "UStructureTaskComponent.h"


// Sets default values for this component's properties
UStructureTaskComponent::UStructureTaskComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bIsWorking = false;
	Progress = 0.f;
	TaskLength = 0.f;
}


/******************** Init *************************/
void UStructureTaskComponent::Init()
{
	Super::Init();

	/*
	APOTLStructure* Structure = Cast<APOTLStructure>(GetOwner());
	if (Structure)
	{
		ParentStructure = Structure;
		bIsOn = true;

		// Bind to time update
		APOTLGameMode* GameMode = Cast<APOTLGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			GameMode->OnTimeUpdated.AddDynamic(this, &UStructureTaskComponent::OnTimeUpdate);
		}
	}
	else
	{
		// Failed to find parent structure
		bIsOn = false;
	}
	*/

}


/******************** OnTimeUpdate *************************/
void UStructureTaskComponent::OnTimeUpdate(float Time, float TimeProgressed)
{
	Super::OnTimeUpdate(Time, TimeProgressed);

	if (bIsOn && bIsWorking)
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
void UStructureTaskComponent::OnProgressComplete()
{

}


// Called when the game starts
void UStructureTaskComponent::BeginPlay()
{
	Super::BeginPlay();


}


// Called every frame
void UStructureTaskComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

