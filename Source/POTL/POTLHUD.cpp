// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "Kismet/GameplayStatics.h"
#include "POTLHUD.h"



// Sets default values
APOTLHUD::APOTLHUD(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{

}



/******************** ClearHighlightedHexes *************************/
void APOTLHUD::ClearHighlightedHexes(EHighlightType Type)
{
	if (Type == EHighlightType::Type1)
	{
		for (int32 i = 0; i < DecalsType1.Num(); i++)
		{
			AHexDecal* Decal = DecalsType1[i];
			Decal->SetActorHiddenInGame(true);
		}
		FreeDecals.Append(DecalsType1);
		DecalsType1.Empty();
	}
}



/******************** HighlightHex *************************/
void APOTLHUD::HighlightHex(UPARAM(ref) FST_Hex& Hex, EHighlightType Type, bool Clear)
{
	//TArray<AHexDecal*> DecalsRef;
	if (Type == EHighlightType::Type1)
	{
		if (Clear)
		{
			for (int32 i = 0; i < DecalsType1.Num(); i++)
			{
				AHexDecal* Decal = DecalsType1[i];
				Decal->SetActorHiddenInGame(true);
				//SetActorEnableCollision(false);
				//Decal->SetActorTickEnabled(false);
				FreeDecals.Add(Decal);
			}
			DecalsType1.Empty();
		}
		if (FreeDecals.IsValidIndex(0))
		{
			AHexDecal* Decal = FreeDecals[0];
			Decal->ChangeMaterial(Type);
			Decal->SetActorLocation(Hex.Location);
			Decal->SetActorHiddenInGame(false);
			DecalsType1.Add(Decal);
			FreeDecals.RemoveAt(0);
		}
	}
}


/******************** HighlightHexes *************************/
void APOTLHUD::HighlightHexes(UPARAM(ref) TArray<FST_Hex>& Hexes, EHighlightType Type, bool Clear)
{
	if (Clear)
	{
		ClearHighlightedHexes(Type);
	}
	for (int32 i = 0; i < Hexes.Num(); i++)
	{
		FST_Hex& Hex = Hexes[i];
		HighlightHex(Hex, Type, false);
	}
}



/******************** BeginPlay *************************/
void APOTLHUD::BeginPlay()
{
	Super::BeginPlay();


}


/******************** Tick *************************/
void APOTLHUD::Tick(float DeltaTime)
{

}