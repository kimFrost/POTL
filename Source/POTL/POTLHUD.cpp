// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "HexDecal.h"
#include "Kismet/GameplayStatics.h"
#include "POTLHUD.h"



// Sets default values
APOTLHUD::APOTLHUD(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
	
}



/******************** SpawnDecals *************************/
void APOTLHUD::SpawnDecals()
{
	UWorld* const World = GetWorld();
	if (World)
	{
		for (int32 i = 0; i < 200; i++)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			FVector SpawnLocation = FVector(0, 0, 0);
			//FRotator SpawnRotation = FRotator(0, 0, 0);
			FRotator SpawnRotation = FRotator(0.f, 90.f, 0.f);
			FTransform SpawnTM(SpawnRotation, SpawnLocation, FVector(1.f, 1.f, 1.f));
			AHexDecal* Decal = World->SpawnActor<AHexDecal>(AHexDecal::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
			if (Decal)
			{
				Decal->SetActorScale3D(FVector(500.f, 142.f, 124.f));
				FreeDecals.Add(Decal);
			}
		}
	}
}


/******************** ClearHighlightedHexes *************************/
void APOTLHUD::ClearHighlightedHexes(EHighlightType Type)
{
	//!! REWRITE THIS FUNCTION TO USE THE REFERENCES STORES IN THE HEXES !!//
	/*
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
	*/
}


/******************** ClearDecals *************************/
void APOTLHUD::ClearDecals(UPARAM(ref) TArray<AHexDecal*>& Decals)
{
	for (int32 i = 0; i < Decals.Num(); i++)
	{
		AHexDecal* Decal = Decals[i];
		if (Decal)
		{	
			Decal->SetActorHiddenInGame(true);
		}
	}
	FreeDecals.Append(Decals);
	Decals.Empty();
}


/******************** HighlightHex *************************/
AHexDecal* APOTLHUD::HighlightHex(UPARAM(ref) FST_Hex& Hex, EHighlightType Type)
{
	if (FreeDecals.IsValidIndex(0))
	{
		AHexDecal* Decal = FreeDecals[0];
		if (Decal)
		{
			Decal->ChangeMaterial(Type);
			Decal->SetActorLocation(Hex.Location);
			Decal->SetActorHiddenInGame(false);
			FreeDecals.RemoveAt(0);
		}
		return Decal;
	}
	//~~ Spawn more decals if none available ~~//
	else if (FreeDecals.Num() == 0)
	{
		SpawnDecals();
		return HighlightHex(Hex, Type);
	}
	//~~ Else no know solution ~~//
	else {
		return nullptr;
	}
}


/******************** HighlightHexes *************************/
TArray<AHexDecal*> APOTLHUD::HighlightHexes(UPARAM(ref) TArray<FST_Hex>& Hexes, EHighlightType Type)
{
	TArray<AHexDecal*> Decals;
	for (int32 i = 0; i < Hexes.Num(); i++)
	{
		FST_Hex& Hex = Hexes[i];
		AHexDecal* Decal = HighlightHex(Hex, Type);
		Decals.Add(Decal);
	}
	return Decals;
}



/******************** BeginPlay *************************/
void APOTLHUD::BeginPlay()
{
	Super::BeginPlay();

	SpawnDecals();
}


/******************** Tick *************************/
void APOTLHUD::Tick(float DeltaTime)
{

}