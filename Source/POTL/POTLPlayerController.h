// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "Kismet/GameplayStatics.h"
#include "POTLHUD.h"
#include "POTLPlayerController.generated.h"



//~~~~~ ENUMS ~~~~~//







//~~~~~ CLASS ~~~~~//
UCLASS()
class POTL_API APOTLPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	
public:

	// Sets default values for this actor's properties
	APOTLPlayerController(const FObjectInitializer &ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	UPOTLGameInstance* GameInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool")
	EToolType ActiveToolType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool")
	bool BuildingAllowed;



	//~~ BeginPlay ~~//
	virtual void BeginPlay() override;

	//~~ Tock ~~//
	virtual void Tick(float DeltaSeconds) override;

};
