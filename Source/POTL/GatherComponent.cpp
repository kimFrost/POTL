// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "POTLUtilFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GatherComponent.h"


// Sets default values for this component's properties
UGatherComponent::UGatherComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
	
	GatherRecipe = "";
	GatherRange = 0;
}



// Get available resource from gather hex indexes

// On hex resource provideTo switch, recal prev provideTo without the removed hex index



/******************** Resolve *************************/
void UGatherComponent::GetGatherIndexes()
{
	GatherFromIndexes.Empty();
	if (GameInstance && Structure)
	{
		if (GatherRange > 0)
		{
			TArray<FVector> Cubes = UPOTLUtilFunctionLibrary::GetCubesInRange(Structure->CubeCoord, GatherRange, false); //!! Maybe not right from vector !!//
			for (int32 i = 0; i < Cubes.Num(); i++)
			{
				FVector2D OffsetCoords = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(Cubes[i]);
				int32 HexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(OffsetCoords, GameInstance->GridXCount);
				GatherFromIndexes.Add(HexIndex);
			}
		}
	}
}


/******************** CalcAvaiableResources *************************/
int32 UGatherComponent::CalcAvaiableResources(UDataTable* RecipeTable)
{
	int32 QuantityAvaible = 0;
	if (GameInstance && Structure)
	{
		for (int32 i = 0; i < GatherFromIndexes.Num(); i++)
		{
			if (GameInstance->Hexes.IsValidIndex(GatherFromIndexes[i]))
			{
				FST_Hex& Hex = GameInstance->Hexes[GatherFromIndexes[i]];
				//Hex.Resources.HasLake
				//Hex.Resources.LakeDepth
			}
		}
	}
	return QuantityAvaible;
}


/******************** Resolve *************************/
void UGatherComponent::Resolve()
{
	if (GameInstance && Structure)
	{

	}
}



// Called when the game starts
void UGatherComponent::BeginPlay()
{
	Super::BeginPlay();
	//UGameplayStatics::GetPlayerController()->GetGameInstance()
	GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
	if (GameInstance)
	{

	}
}


// Called every frame
void UGatherComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
	// ...
}

