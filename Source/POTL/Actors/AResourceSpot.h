// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "UObjects/UResource.h"
#include "UObjects/UHexTile.h"
#include "AResourceSpot.generated.h"




UCLASS()
class POTL_API AResourceSpot : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	AResourceSpot(const FObjectInitializer &ObjectInitializer);

	// Resource ID

	// Hex
	

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
};
