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
	PrimaryComponentTick.bCanEverTick = false;

	ProviderRange = 5;
}



void UProviderComponent::ToggleProvider()
{
	if (bIsWorking)
	{
		DeactivateProvider();
	}
	else
	{
		ActivateProvider();
	}
}

void UProviderComponent::ActivateProvider()
{
	if (!bIsWorking && ParentStructure)
	{
		for (auto& Hex : ParentStructure->HexesInRange)
		{
			if (Hex) {
				Hex->AddProvider(this);
			}
		}
		bIsWorking = true;
	}
}

void UProviderComponent::DeactivateProvider()
{
	if (bIsWorking && ParentStructure)
	{
		for (auto& Hex : ParentStructure->HexesInRange)
		{
			if (Hex) {
				Hex->RemoveProvider(this);
			}
		}
		bIsWorking = false;
	}
}

void UProviderComponent::ValidateRequirements()
{
	if (!ParentStructure)
	{
		DeactivateProvider();
	}
	else if (bRequireAttached)
	{
		if (ParentStructure) // && ParentStructure->AttachedTo
		{
			//?? Validate tile on for required resource??

			ActivateProvider();
		}
		else
		{
			DeactivateProvider();
		}
	}
}

//void UProviderComponent::SetProduction(const TArray<FST_ResourceQuantity>& ResourceList)
void UProviderComponent::SetProduction(const TArray<UResource*>& ResourceList)
{
	for (auto& Resource : ResourceList)
	{
		if (Resource)
		{

		}
	}


	Resources = ResourceList;
	Provides.Empty();

	for (int32 i = 0; i < ResourceList.Num(); i++)
	{
		UResource* Resource = ResourceList[i];
		if (Resource)
		{
			if (Provides.Contains(Resource->ResourceId))
			{
				Provides[Resource->ResourceId] += 1;
			}
			else
			{
				Provides.Add(Resource->ResourceId, 1);
			}
		}
	}

	if (ParentStructure)
	{
		for (auto& Hex : ParentStructure->HexesInRange)
		{
			if (Hex) {
				Hex->UpdateResources();
			}
		}
	}
}

void UProviderComponent::ActivateComponent()
{
	Super::ActivateComponent();
	ActivateProvider();
}

void UProviderComponent::DeactivateComponent()
{
	Super::DeactivateComponent();
	DeactivateProvider();
}

void UProviderComponent::Init()
{
	Super::Init();

	if (!ParentStructure) // || (ParentStructure && !ParentStructure->AttachedTo)
	{
		bIsOn = false;
	}

	// Store hexes in range
	HexesInRange.Empty();
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

	// ValidateRequirements on initialization
	//ValidateRequirements(); // Might need to use use ActivateProvider instead.
	ActivateComponent();

	// ValidateRequirements every second
	//GetWorld()->GetTimerManager().SetTimer(ProvideCheckTimer, this, &UProviderComponent::ValidateRequirements, 1.f, true);
}


void UProviderComponent::BeginPlay()
{
	Super::BeginPlay();

}
