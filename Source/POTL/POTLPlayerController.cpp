// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "Kismet/GameplayStatics.h"
#include "POTLHUD.h"
#include "POTLUtilFunctionLibrary.h"
#include "POTLPlayerController.h"





// Sets default values
APOTLPlayerController::APOTLPlayerController(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
	GameInstance = nullptr;
	ActiveToolType = EToolType::Select;
}