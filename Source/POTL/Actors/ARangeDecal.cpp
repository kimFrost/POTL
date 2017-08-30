// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "UObjects/UHexTile.h"
#include "ARangeDecal.h"




void ARangeDecal::DrawHexes(const TArray<UHexTile*>& Hexes)
{
	OnDrawHexes(Hexes);
}

void ARangeDecal::OnDrawHexes_Implementation(const TArray<UHexTile*>& Hexes)
{}


