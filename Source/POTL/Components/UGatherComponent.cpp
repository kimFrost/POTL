// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "UObjects/UResource.h"
#include "UObjects/UHexTile.h"
#include "POTLGameMode.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "POTLUtilFunctionLibrary.h"
#include "UGatherComponent.h"



// Sets default values for this component's properties
UGatherComponent::UGatherComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	GatherRange = 3;

	//TaskLength = 5.f;

}


/******************** OnProduction *************************/
void UGatherComponent::OnProduction_Implementation()
{
	//MissingResources = RequiredResources;
	CheckGather();
}


void UGatherComponent::CheckGather()
{
	//TODO: Better validation Logic for attachedTo

	if (ParentStructure && ParentStructure->AttachedTo)
	{
		UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
		if (GameInstance)
		{
			
		}
	}
	else
	{
		bIsWorking = false;
	}
}


/******************** OnProgressComplete *************************/
void UGatherComponent::OnProgressComplete()
{
	Super::OnProgressComplete();

	OnProduction();

	
	if (ParentStructure)
	{
		UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
		if (GameInstance)
		{
			// Add resource to storage
			for (auto& ProductionItem : Production)
			{
				for (int i = 0; i < ProductionItem.Value; i++)
				{
					UResource* ProducedResource = GameInstance->CreateResource(ProductionItem.Key);
					if (ProducedResource)
					{
						ParentStructure->StoreResource(ProducedResource);
					}
				}
				//ParentStructure->AddResource(ProductionItem.Key, ProductionItem.Value);
			}

			if (ParentStructure->AttachedTo)
			{

			}
		}
	}
}


void UGatherComponent::Init()
{
	Super::Init();

	if (!ParentStructure || (ParentStructure && !ParentStructure->AttachedTo))
	{
		bIsOn = false;
	}


	UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
	if (GameInstance)
	{
		if (GatherRange > 0)
		{
			TArray<FVector> Cubes = UPOTLUtilFunctionLibrary::GetCubesInRange(ParentStructure->CubeCoord, GatherRange, false);
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

	// CheckGather every second
	GetWorld()->GetTimerManager().SetTimer(GatherCheckTimer, this, &UGatherComponent::CheckGather, 1.f, true);
}


void UGatherComponent::BeginPlay()
{
	Super::BeginPlay();

}
