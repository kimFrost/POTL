// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "UObjects/UResource.h"
#include "UObjects/UHexTile.h"
#include "POTLGameMode.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "POTLUtilFunctionLibrary.h"
#include "UProviderComponent.h"



// Sets default values for this component's properties
UProviderComponent::UProviderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ProviderRange = 3;
}



void UProviderComponent::ValidateRequirements()
{
	//TODO: Better validation Logic for attachedTo
	if (!ParentStructure)
	{
		bIsWorking = false;
	}
	else if (bRequireAttached)
	{
		if (ParentStructure && ParentStructure->AttachedTo)
		{
			bIsWorking = true;
		}
		else
		{
			bIsWorking = false;
		}
	}
}



void UProviderComponent::Init()
{
	Super::Init();

	if (!ParentStructure || (ParentStructure && !ParentStructure->AttachedTo))
	{
		bIsOn = false;
	}

	// Store hexes in range
	UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
	if (GameInstance)
	{
		if (ProviderRange > 0)
		{
			TArray<FVector> Cubes = UPOTLUtilFunctionLibrary::GetCubesInRange(ParentStructure->CubeCoord, ProviderRange, false);
			for (int32 i = 0; i < Cubes.Num(); i++)
			{
				FVector2D OffsetCoords = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(Cubes[i]);
				int32 HexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(OffsetCoords, GameInstance->GridXCount);
				if (GameInstance->Hexes.IsValidIndex(HexIndex))
				{
					HexesInRange.Add(GameInstance->Hexes[HexIndex]);
				}
			}
		}
	}

	// ValidateRequirements every second
	GetWorld()->GetTimerManager().SetTimer(ProvideCheckTimer, this, &UProviderComponent::ValidateRequirements, 1.f, true);
}


void UProviderComponent::BeginPlay()
{
	Super::BeginPlay();

}
