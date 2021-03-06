// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObjects/UHexTile.h"
#include "POTLDataHolder.h"
#include <string>
#include "Components/UProviderComponent.h"
#include "POTLUtilFunctionLibrary.h"



// Sets default values
UPOTLUtilFunctionLibrary::UPOTLUtilFunctionLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}


/******************** GetObjReferenceCount *************************/
int32 UPOTLUtilFunctionLibrary::GetObjReferenceCount(UObject* Obj, TArray<UObject*>* OutReferredToObjects = nullptr)
{
	if (!Obj || !Obj->IsValidLowLevelFast())
	{
		return -1;
	}

	TArray<UObject*> ReferredToObjects;				//req outer, ignore archetype, recursive, ignore transient
	FReferenceFinder ObjectReferenceCollector(ReferredToObjects, Obj, false, true, true, false);
	ObjectReferenceCollector.FindReferences(Obj);

	if (OutReferredToObjects)
	{
		OutReferredToObjects->Append(ReferredToObjects);
	}
	//return OutReferredToObjects.Num();
	return 0;
}

FVector UPOTLUtilFunctionLibrary::RotateCube(FVector CubeCoord, int32 Direction, FVector CenterCube)
{
	Direction = Direction % 6;
	FVector RotatedCube = CubeCoord;
	FVector CubeLocal = CubeCoord - CenterCube;
	switch (Direction)
	{
		case 0:
		{
			break;
		}
		case 1:
		{
			RotatedCube = FVector{ -CubeLocal.Z, -CubeLocal.X, -CubeLocal.Y } + CenterCube;
			break;
		}
		case 2:
		{
			RotatedCube = FVector{ CubeLocal.Y, CubeLocal.Z, CubeLocal.X } + CenterCube;
			break;
		}
		case 3:
		{
			RotatedCube = FVector{ -CubeLocal.X, -CubeLocal.Y, -CubeLocal.Z } + CenterCube;
			break;
		}
		case 4:
		{
			RotatedCube = FVector{ CubeLocal.Z, CubeLocal.X, CubeLocal.Y } + CenterCube;
			break;
		}
		case 5:
		{
			RotatedCube = FVector{ -CubeLocal.Y, -CubeLocal.Z, -CubeLocal.X } + CenterCube;
			break;
		}
	}
	return RotatedCube;
}
FVector UPOTLUtilFunctionLibrary::DirectionToCube(int32 Direction)
{
	Direction = Direction % 6;
	switch (Direction)
	{
		case 0:
		{
			return FVector{ 1,0,-1 };
		}
		case 1:
		{
			return FVector{ 1,-1,0 };
		}
		case 2:
		{
			return FVector{ 0,-1,1 };
		}
		case 3:
		{
			return FVector{ -1,0,1 };
		}
		case 4:
		{
			return FVector{ -1,1,0 };
		}
		case 5:
		{
			return FVector{ 0,1,-1 };
		}
	}
	return FVector();
}
TArray<FVector> UPOTLUtilFunctionLibrary::RotateCubes(TArray<FVector> CubeCoords, int32 Direction, FVector CenterCube)
{
	Direction = Direction % 6;
	TArray<FVector> RotatedCubes;
	for (int32 i = 0; i < CubeCoords.Num(); i++)
	{
		FVector CubeCoord = CubeCoords[i];
		FVector CubeLocal = CubeCoord - CenterCube;
		switch(Direction)
		{
			case 0:
			{
				RotatedCubes.Add(CubeCoord);
				break;
			}
			case 1:
			{
				CubeCoord = FVector{ -CubeLocal.Z, -CubeLocal.X, -CubeLocal.Y } + CenterCube;
				RotatedCubes.Add(CubeCoord);
				break;
			}
			case 2:
			{
				CubeCoord = FVector{ CubeLocal.Y, CubeLocal.Z, CubeLocal.X } + CenterCube;
				RotatedCubes.Add(CubeCoord);
				break;
			}
			case 3:
			{
				CubeCoord = FVector{ -CubeLocal.X, -CubeLocal.Y, -CubeLocal.Z } + CenterCube;
				RotatedCubes.Add(CubeCoord);
				break;
			}
			case 4:
			{
				CubeCoord = FVector{ CubeLocal.Z, CubeLocal.X, CubeLocal.Y } + CenterCube;
				RotatedCubes.Add(CubeCoord);
				break;
			}
			case 5:
			{
				CubeCoord = FVector{ -CubeLocal.Y, -CubeLocal.Z, -CubeLocal.X } + CenterCube;
				RotatedCubes.Add(CubeCoord);
				break;
			}
		}
	}
	return RotatedCubes;
}
FVector UPOTLUtilFunctionLibrary::RoundCube(FVector Cube)
{	
	FVector RoundedCube;
	int32 Rx = FMath::RoundToInt(Cube.X);
	int32 Ry = FMath::RoundToInt(Cube.Y);
	int32 Rz = FMath::RoundToInt(Cube.Z);

	float XDiff = FMath::Abs(Rx - Cube.X);
	float YDiff = FMath::Abs(Ry - Cube.Y);
	float ZDiff = FMath::Abs(Rz - Cube.Z);

	if (XDiff > YDiff && XDiff > ZDiff)
	{
		Rx = -Ry - Rz;
	}
	else if (YDiff > ZDiff)
	{
		Ry = -Rx - Rz;
	}
	else
	{
		Rz = -Rx - Ry;
	}

	RoundedCube = FVector{ (float)(Rx), (float)(Ry), (float)(Rz) };
	return RoundedCube;
}
FVector UPOTLUtilFunctionLibrary::LocationToCube(int32 GridXCount, float HexWidth, float HexHeight, FVector Location)
{
	Location = Location - FVector{ HexWidth / 2, HexHeight / 2, 0 };

	float Q = (Location.X * FMath::Sqrt(3) / 3 - Location.Y / 3) / (HexHeight / 2);
	float R = Location.Y * 2 / 3 / (HexHeight / 2);

	//float Temp = FMath::FloorToFloat(Location.X + FMath::Sqrt(3) * Location.Y + 1);
	//float Q = FMath::FloorToFloat((FMath::FloorToFloat(2 * Location.X + 1) + Temp) / 3);
	//float R = FMath::FloorToFloat((Temp + FMath::FloorToFloat(-Location.X + FMath::Sqrt(3) * Location.Y + 1)) / 3);

	LogMsg("Q: " + FString::FromInt(Q) + ", R: " + FString::FromInt(R), 15.0f, FColor::Yellow, 1);
	
	return RoundCube(AxialToCube(Q, R));
	//return RoundCube(ConvertOffsetToCube(FVector2D{ Q, R }));
}
int32 UPOTLUtilFunctionLibrary::GetGridIndex(int32 GridWidth, int32 Column, int32 Row, bool NoWrap)
{
	int32 index;
	float insideGrid;
	// -1 == Outside grid || 0 == Inside grid
	insideGrid = FMath::FloorToInt((Column + 1) / (GridWidth + 2)) * -1;
	index = ((Row * GridWidth) + Column + Row) * (1 + (2 * insideGrid));
	return index;
}
int32 UPOTLUtilFunctionLibrary::GetHexIndex(FVector2D OffsetCoord, int32 GridXCount)
{
	int32 GridWidth = (GridXCount - (GridXCount % 2)) / 2 - 1;
	int32 Index = GetGridIndex(GridWidth, FMath::FloorToInt(OffsetCoord.X), FMath::FloorToInt(OffsetCoord.Y), true);
	Index = Index - (FMath::FloorToInt(OffsetCoord.Y) / 2 * ((GridXCount + 1) % 2));
	return Index;
}
TArray<int32> UPOTLUtilFunctionLibrary::CubesToHexIndexes(TArray<FVector> Cubes, int32 GridXCount)
{
	TArray<int32> HexIndexes;
	for (int32 i = 0; i < Cubes.Num(); i++)
	{
		FVector& Cube = Cubes[i];
		FVector2D Axial = ConvertCubeToOffset(Cube);
		int32 HexIndex = GetHexIndex(Axial, GridXCount);
		HexIndexes.Add(HexIndex);
	}
	return HexIndexes;
}
void UPOTLUtilFunctionLibrary::LogMsg(FString Msg = "", float Duration = 5.0f, FColor DebugColor = FColor::Green, int32 GroupIndex = -1)
{
	GEngine->AddOnScreenDebugMessage(GroupIndex, Duration, DebugColor, Msg);
}

/******************** AMIT *************************/
int32 UPOTLUtilFunctionLibrary::GetCubeDistance(FVector CubeCoordsFrom, FVector CubeCoordsTo)
{
	int32 Distance;
	FVector DistanceVector = CubeCoordsFrom - CubeCoordsTo;
	Distance = DistanceVector.GetAbsMax();
	//float X = FMath::Max(DistanceVector.X);
	return Distance;
}
TArray<FVector> UPOTLUtilFunctionLibrary::GetCubesInRange(FVector CubeCoordsFrom, int32 Range, bool IncludeFrom)
{
	TArray<FVector> Cubes;
	if (Range > 0)
	{
		int32 NegativeRange = Range * -1;
		for (int32 Dx = NegativeRange; Dx <= Range; Dx++)
		{
			int32 From = FMath::FloorToInt(FVector(NegativeRange, (Dx * -1) - Range, -99999999).GetMax());
			int32 To = FMath::FloorToInt(FVector(Range, (Dx * -1) + Range, 99999999).GetMin());
			for (int32 Dy = From; Dy <= To; Dy++)
			{
				int32 Dz = (Dx * -1) - Dy;
				Cubes.Add(FVector(Dx + CubeCoordsFrom.X, Dy + CubeCoordsFrom.Y, Dz + CubeCoordsFrom.Z));
			}
		}
	}
	if (!IncludeFrom)
	{
		Cubes.Remove(CubeCoordsFrom);
	}
	return Cubes;
}
TArray<FVector> UPOTLUtilFunctionLibrary::GetCubeRing(FVector CubeCoordsCenter, int32 Radius)
{
	TArray<FVector> Cubes = TArray<FVector>();

	FVector Cube = CubeCoordsCenter + (DirectionToCube(0) * Radius); // Start which is not center, but position zero
	for (int32 i = 0; i < 6; i++)
	{
		for (int32 r = 0; r < Radius; r++)
		{
			Cubes.Add(Cube);
			Cube = Cube + DirectionToCube(i + 2); // +2 to make rotation match.
		}
	}
	return Cubes;
}
TArray<FVector> UPOTLUtilFunctionLibrary::GetCubeSpiralInRange(FVector CubeCoordsFrom, int32 Range, bool IncludeFrom)
{
	TArray<FVector> Cubes = TArray<FVector>();
	for (int32 k = 1; k <= Range; k++)
	{
		Cubes.Append(GetCubeRing(CubeCoordsFrom, k));
	}
	if (IncludeFrom)
	{
		Cubes.Add(CubeCoordsFrom);
	}
	return Cubes;
}
FVector UPOTLUtilFunctionLibrary::ConvertOffsetToCube(FVector2D OffsetCoords)
{
	FVector CubeCoords;
	//CubeCoords.X = OffsetCoords.X - ((FMath::FloorToInt(OffsetCoords.Y) - (FMath::FloorToInt(OffsetCoords.Y) % 2)) / 2);
	//CubeCoords.Y = (CubeCoords.X * -1) - OffsetCoords.Y;
	//CubeCoords.Z = OffsetCoords.Y;
	CubeCoords.X = OffsetCoords.X - (OffsetCoords.Y - (FMath::FloorToInt(OffsetCoords.Y) % 2)) / 2;
	CubeCoords.Y = -CubeCoords.X - OffsetCoords.Y;
	CubeCoords.Z = OffsetCoords.Y;
	return CubeCoords;
}
FVector2D UPOTLUtilFunctionLibrary::ConvertCubeToOffset(FVector CubeCoords)
{
	int32 CubeCoordsZFloored = FMath::FloorToInt(CubeCoords.Z);
	FVector2D OffsetCoords;
	OffsetCoords.X = (FMath::FloorToInt(CubeCoords.X) + ((CubeCoordsZFloored - CubeCoordsZFloored % 2) / 2));
	OffsetCoords.Y = CubeCoords.Z;
	return OffsetCoords;
}
FVector UPOTLUtilFunctionLibrary::AxialToCube(float Q, float R)
{
	FVector CubeCoords;
	CubeCoords.X = Q;
	CubeCoords.Y = (Q * -1) - R;
	CubeCoords.Z = R;
	return CubeCoords;
}
FVector UPOTLUtilFunctionLibrary::CubeToRelativeLocation(FVector CubeCoords)
{
	//TODO get numbers from game instance instead
	float X = CubeCoords.X * 127.5f + CubeCoords.Y * -127.5f;
	float Y = CubeCoords.Z * (294.f / 4 * 3);
	return FVector(X, Y, 0);
}

void UPOTLUtilFunctionLibrary::ActorExits(AActor* Actor, TEnumAsByte<EBoolGateEnum>& Branches)
{
	if (Actor)
	{
		Branches = EBoolGateEnum::Exits;
	}
	else
	{
		Branches = EBoolGateEnum::Null;
	}
}
TArray<int32> UPOTLUtilFunctionLibrary::HexesToHexIndexes(const TArray<UHexTile*>& Hexes)
{
	TArray<int32> HexIndexes;
	for (int32 i = 0; i < Hexes.Num(); i++)
	{
		UHexTile* Hex = Hexes[i];
		if (IsValid(Hex))
		{
			HexIndexes.Add(Hexes[i]->HexIndex);
		}
	}
	return HexIndexes;
}
TArray<UHexTile*> UPOTLUtilFunctionLibrary::SubtractHexes(const TArray<UHexTile*>& Hexes, const TArray<UHexTile*>& Subtraction)
{
	TArray<UHexTile*> Result;
	TArray<int32> HexIndexes = HexesToHexIndexes(Hexes);
	TArray<int32> SubtractionIndexes = HexesToHexIndexes(Subtraction);
	int32 i;
	for (i = 0; i < SubtractionIndexes.Num(); i++)
	{
		HexIndexes.Remove(SubtractionIndexes[i]);
	}
	for (i = 0; i < Hexes.Num(); i++) 
	{ 
		UHexTile* Hex = Hexes[i];
		if (IsValid(Hex) && HexIndexes.Contains(Hex->HexIndex))
		{
			Result.Add(Hex);
		}
	}
	return Result;
}
TArray<UHexTile*> UPOTLUtilFunctionLibrary::IntersectHexes(const TArray<UHexTile*>& Hexes, const TArray<UHexTile*>& Intersection)
{
	TArray<UHexTile*> Result;
	TArray<int32> HexIndexes = HexesToHexIndexes(Hexes);
	TArray<int32> IntersectionIndexes = HexesToHexIndexes(Intersection);
	int32 i;
	for (i = HexIndexes.Num() - 1; i >= 0; i--)
	{
		if (!IntersectionIndexes.Contains(HexIndexes[i]))
		{
			HexIndexes.RemoveAt(i);
		}
	}
	for (i = 0; i < Hexes.Num(); i++)
	{
		UHexTile* Hex = Hexes[i];
		if (IsValid(Hex) && HexIndexes.Contains(Hex->HexIndex))
		{
			Result.Add(Hex);
		}
	}
	return Result;
}
TArray<UHexTile*> UPOTLUtilFunctionLibrary::GetAdjacentHexesToHex(const UHexTile* Hex)
{
	TArray<UHexTile*> AdjacentHexes;
	if (Hex)
	{
		for (auto& HexNeighbor: Hex->HexNeighbors) 
		{
			if (HexNeighbor)
			{
				AdjacentHexes.Add(HexNeighbor);
			}
		}
	}
	return AdjacentHexes;
}
TArray<UHexTile*> UPOTLUtilFunctionLibrary::GetAdjacentHexesToHexes(const TArray<UHexTile*>& Hexes)
{
	TArray<UHexTile*> AdjacentHexes;
	for (auto& Hex : Hexes)
	{
		if (Hex)
		{
			TArray<UHexTile*> NeighborHexes = GetAdjacentHexesToHex(Hex);
			for (auto& NeighborHex : NeighborHexes)
			{
				AdjacentHexes.AddUnique(NeighborHex);
			}
		}
	}
	return AdjacentHexes;
}





/** -- MAP - PATHING --------------------*/


/******************** GetHexesWithFloodFill *************************/
TArray<FVector> UPOTLUtilFunctionLibrary::GetHexesWithFloodFill(FVector StartPosition, const TArray<FVector>& Obstacles, int32 Range)
{
	struct Fridge
	{
		TArray<FVector> CubeCoords;
	};

	TArray<FVector> VisitedCubeCoords;
	TArray<Fridge> Fridges;

	TArray<FVector> CubeDirections;

	CubeDirections.Add({ 1, 0, -1 });
	CubeDirections.Add({ 1, -1, 0 });
	CubeDirections.Add({ 0, -1, 1 });
	CubeDirections.Add({ -1, 0, 1 });
	CubeDirections.Add({ -1, 1, 0 });
	CubeDirections.Add({ 0, 1, -1 });

	// Add StartPosition to cubecoords already visited 
	VisitedCubeCoords.Add(StartPosition);

	Fridge fridge;
	fridge.CubeCoords.Add(StartPosition);
	Fridges.Add(fridge);

	for (int32 k = 1; k <= Range; k++)
	{
		Fridge fridge;
		Fridges.Add(fridge);

		fridge = Fridges[k - 1];

		int arrayLength = fridge.CubeCoords.Num();
		for (int32 m = 0; m < arrayLength; m++)
		{
			FVector CubeCoord = fridge.CubeCoords[m];
			for (int32 l = 0; l < CubeDirections.Num(); l++)
			{
				FVector CubeDirection = CubeDirections[l];
				FVector CombinedVector = CubeCoord + CubeDirection;
				bool ExistsInVisited = VisitedCubeCoords.Contains(CombinedVector);
				bool ExistsInObstacles = Obstacles.Contains(CombinedVector);
				if (ExistsInVisited || ExistsInObstacles)
				{

				}
				else 
				{
					VisitedCubeCoords.Add(CombinedVector);
					Fridges[k].CubeCoords.Add(CombinedVector);
				}
			}
		}
	}
	return VisitedCubeCoords;
}

/*
TArray<FST_ResourceQuantity> UPOTLUtilFunctionLibrary::ConvertResourceList(const TMap<FString, int>& ResourceList)
{
	return TArray<FST_ResourceQuantity>();
}
*/

TArray<UProviderComponent*> UPOTLUtilFunctionLibrary::FilterProvidersByResourceList(const TArray<UProviderComponent*>& Providers, const TArray<FST_ResourceQuantity>& ResourceList)
{
	TArray<UProviderComponent*> FilterProviders = TArray<UProviderComponent*>();
	for (auto& Provider : Providers)
	{
		if (Provider)
		{
			bool bValid = true;
			for (auto& Resource : ResourceList)
			{
				if (!Provider->Provides.Contains(Resource.ResourceId) || Provider->Provides[Resource.ResourceId] < Resource.Quantity)
				{
					bValid = false;
					break;
				}
			}
			if (bValid)
			{
				FilterProviders.Add(Provider);
			}
		}
	}
	return FilterProviders;
}

void UPOTLUtilFunctionLibrary::FilterTileArrayByResources(const TArray<UHexTile*>& TargetArray, const TMap<FString, int>& Resources, TArray<UHexTile*>& FilteredArray)
{
	FilteredArray.Empty();

	for (auto& Hex : TargetArray)
	{
		for (auto& Entry : Resources)
		{
			if (!Hex->Resources.Contains(Entry.Key) || Hex->Resources[Entry.Key] < Entry.Value)
			{
				break;
			}
		}
		FilteredArray.Add(Hex);
	}

	//TSubclassOf<class AActor> FilterClass
	//TargetArray.FilterByPredicate

	/*
	for (auto It = TargetArray.CreateConstIterator(); It; It++)
	{
		AActor* TargetElement = (*It);
		if (TargetElement && TargetElement->IsA(FilterClass))
		{
			FilteredArray.Add(TargetElement);
		}
	}
	*/
}


void UPOTLUtilFunctionLibrary::MergeResourceLists(const TMap<FString, int>& From, TMap<FString, int>& To)
{
	for (auto & Line : From)
	{
		if (To.Contains(Line.Key))
		{
			To[Line.Key] += Line.Value;
		}
		else
		{
			To.Add(Line.Key, Line.Value);
		}
	}
}

//static void ParseRecordSetForState(const FP4RecordSet& InRecords, TMap<FString, EPerforceState::Type>& OutResults)

//const TMap & TargetMap
// typedef TMap<const UTexture*, const UTexture*> FPaperRenderSceneProxyTextureOverrideMap;


// NOT WORKING
//UPARAM(ref) TMap<FString, int> From, UPARAM(ref) TMap<FString, int> To