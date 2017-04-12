// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "Components/UStorageComponent.h"
#include "POTLGameInstance.h"
#include "POTLUtilFunctionLibrary.h"
#include "ULaborPoint.h"




ULaborPoint::ULaborPoint()
{
	
}


ULaborPoint::~ULaborPoint()
{
	
}

APOTLStructure* ULaborPoint::GetOwner()
{


	return nullptr;
}


void ULaborPoint::Init()
{
	// Get resource data
	UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
	if (GameInstance)
	{
		//GameInstance->DATA_Resources->
	}

	// Create resource throught game instance instead
}
