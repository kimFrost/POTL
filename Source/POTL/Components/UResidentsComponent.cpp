// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLStructure.h"
#include "POTLGameMode.h"
#include "POTLGameInstance.h"
#include "UResidentsComponent.h"


// Sets default values for this component's properties
UResidentsComponent::UResidentsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UResidentsComponent::BeginPlay()
{
	Super::BeginPlay();


}


// Called every frame
void UResidentsComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

