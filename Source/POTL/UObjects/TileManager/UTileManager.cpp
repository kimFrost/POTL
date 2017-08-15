// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLGameInstance.h"
#include "UObjects/UHexTile.h"
#include "UTileManager.h"



// Constructor
UTileManager::UTileManager(AIsland* Island)
{
	HexWidth = 255.0f;
	HexHeight = HexWidth / FMath::Sqrt(3) * 2.f;
	GridXCount = 200; // Temp. Needs to be calc in point creation.
	GridYCount = 200; // Temp. Needs to be calc in point creation.
	HexGridReady = false;

	WorldActor = Island;
}

// Deconstructor
UTileManager::~UTileManager()
{

}


