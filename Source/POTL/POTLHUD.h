// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "POTLHexDecal.h"
#include "POTLHUD.generated.h"

/**
 * 
 */
UCLASS()
class POTL_API APOTLHUD : public AHUD
{
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties
	APOTLHUD(const FObjectInitializer &ObjectInitializer);
	
	//UFUNCTION(BlueprintCallable, Category = "Map")
	//APOTLStructure* GetNearestStructure(FVector Location, TSubclassOf<APOTLStructure> StructureClass);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
	TArray<APOTLHexDecal*> FreeDecalsTest;
	//ADecalActor


	//~~ BeginPlay ~~//
	virtual void BeginPlay() override;

	//~~ Tock ~~//
	virtual void Tick(float DeltaSeconds) override;


};
