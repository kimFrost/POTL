// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "UObjects/UHexTile.h"
#include "POTLDataHolder.h"
#include "ARangeDecal.h"




void ARangeDecal::DrawHexes(const TArray<UHexTile*>& Hexes)
{
	OnDrawHexes(Hexes);
}

void ARangeDecal::DrawStructureInfo(const FST_Structure& StructureInfo)
{
	OnDrawStructureInfo(StructureInfo);
}


void ARangeDecal::OnClear_Implementation()
{}

void ARangeDecal::OnDrawHexes_Implementation(const TArray<UHexTile*>& Hexes)
{}

void ARangeDecal::OnDrawHex_Implementation(const UHexTile* Hex, const FLinearColor Color)
{}

void ARangeDecal::OnDrawStructureInfo_Implementation(const FST_Structure& StructureInfo)
{}

