// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLStructure.h"
#include "POTLGameInstance.h"



// Sets default values
UPOTLGameInstance::UPOTLGameInstance(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{


}


TArray<FST_ConstructLocation> UPOTLGameInstance::GetConstructLocations(APOTLStructure* Structure, bool IncludeChildren)
{
	TArray<FST_ConstructLocation> ConstructLocations;

	TArray<int32> VisitedHexIndexes;
	struct Frontier
	{
		TArray<FST_Hex> Hexes;
	};
	TArray<Frontier> Frontiers;

	// Add Start Hex to VisitedHexes
	VisitedHexIndexes.Add(Structure->Hex.HexIndex);

	Frontier frontier;
	frontier.Hexes.Add(Structure->Hex);
	Frontiers.Add(frontier);

	for (int32 k = 1; k <= Structure->BroadcastRange; k++)
	{
		Frontier frontier;
		Frontiers.Add(frontier);

		frontier = Frontiers[k - 1];

		for (int32 m = 0; m < frontier.Hexes.Num(); m++)
		{
			FST_Hex& Hex = frontier.Hexes[m];
			for (int32 l = 0; l < Hex.HexNeighborIndexes.Num(); l++)
			{
				int32& Index = Hex.HexNeighborIndexes[l];
				if (Index != -1 && Hexes.IsValidIndex(Index))
				{
					FST_Hex& NeighborHex = Hexes[Index];
					if (VisitedHexIndexes.Contains(Index) || IsHexBuildable(NeighborHex))
					{

					}
					else {
						VisitedHexIndexes.Add(Index);
						Frontiers[k].Hexes.Add(Hex);
						FST_ConstructLocation ConstructLocation;
						//ConstructLocation.EmitTo.Add(Structure); // Don't know if it should be a hex or structure reference to, for it to be the best solution.
						ConstructLocation.EmitTo.Add(Hex);
						ConstructLocations.Add(ConstructLocation);
					}
				}
			}
		}

	}

	if (IncludeChildren) {
		for (int32 i = 0; i < Structure->BroadcastTo.Num(); i++)
		{
			//TArray<FST_ConstructLocation> ChildrenConstructLocations = GetConstructLocations(Structure->BroadcastTo[i], true);
		}
	}

	return ConstructLocations;
}


bool UPOTLGameInstance::IsHexBuildable(FST_Hex& Hex)
{
	if (Hex.AttachedBuilding == NULL)
	{
		return true;
	}
	else {
		return false;
	}
}
