// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "AResourceSpot.h"


// Sets default values
AResourceSpot::AResourceSpot(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Hex = nullptr;
}




void AResourceSpot::BeginPlay()
{
	Super::BeginPlay();

}

void AResourceSpot::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

