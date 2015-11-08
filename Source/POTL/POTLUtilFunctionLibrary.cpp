// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "POTLDataHolder.h"
#include <string>
#include "POTLUtilFunctionLibrary.h"



// Sets default values
UPOTLUtilFunctionLibrary::UPOTLUtilFunctionLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}


/*
FST_Hex UPOTLUtilFunctionLibrary::CubeToHex(FVector CubeCoord, const TArray<FST_Hex>& Hexes, int32 GridXCount)
{
	FST_Hex Hex;
	
	bool Found = false;
	FVector2D OffsetCoords = ConvertCubeToOffset(CubeCoord);
	int32 Index = GetHexIndex(OffsetCoords, GridXCount);
	if (HexIndexValid(Hexes, Index))
	{
		GetHex(Hexes, Index, Found, Hex); //const TArray<FST_Hex> Hexes, const int32 Index, bool &Found, FST_Hex &Hex
	}
	
	return Hex;
}
*/

/*
TArray<FST_Hex> UPOTLUtilFunctionLibrary::CubesToHexes(TArray<FVector> CubeCoords, const TArray<FST_Hex>& Hexes, int32 GridXCount)
{
	TArray<FST_Hex> ConvertedHexes;
	FST_Hex Hex;
	for (int32 i = 0; i < CubeCoords.Num(); i++)
	{
		FVector CubeCoord = CubeCoords[i];
		Hex = CubeToHex(CubeCoord, Hexes, GridXCount);
		ConvertedHexes.Add(Hex);
	}
	return ConvertedHexes;
}
*/

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

/*
FST_Hex UPOTLUtilFunctionLibrary::LocationToHex(int32 GridXCount, float HexWidth, float HexHeight, FVector Location, const TArray<FST_Hex>& Hexes)
{
	float Size = HexHeight / 2;
	float S = HexWidth / FMath::Sqrt(3) * (3 / 2);
	int32 Odd = FMath::FloorToInt(Location.Y / Size) % 2; // Is Odd == 1 || Even == 0
	int32 IndexX = (HexWidth - (HexWidth / 2 * Odd)) / HexWidth;
	int32 IndexY = FMath::FloorToInt(Location.Y / S);
	int32 HexIndex = -1;

	FVector HexBaseLocation = FVector{ IndexX * HexWidth + (HexWidth / 2 * Odd), IndexY / S, 0 };
	FVector HexRelativeLocation = Location - HexBaseLocation;

	FVector YLeft = FVector{ (float)(HexRelativeLocation.X), (HexRelativeLocation.X * (100 / 90 * 50 / 100)) - (HexHeight - S), 0 } +HexBaseLocation;
	FVector YRight = FVector{ (float)(HexRelativeLocation.X), (HexRelativeLocation.X * (100 / 90 * -50 / 100)) + (HexHeight - S), 0 } +HexBaseLocation;

	bool LocYSmallerThanCalYRight = (Location.Y < YLeft.Y);
	bool LocYSmallerThanCalYLeft = (Location.Y < YRight.Y);

	if (LocYSmallerThanCalYRight)
	{
		IndexX = IndexX + (IndexY % 2);
	}
	else if (LocYSmallerThanCalYLeft)
	{
		IndexX = IndexX - ((IndexY + 1) % 2);
	}
	IndexY = IndexY - 1;

	HexIndex = GetGridIndex((GridXCount - (GridXCount % 2)) / 2 - 1, IndexX, IndexY, true);

	bool Found = false;
	FST_Hex Hex;
	GetHex(Hexes, HexIndex, Found, Hex);
	return Hex;
}
*/


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


void UPOTLUtilFunctionLibrary::LogMsg(FString Msg = "", float Duration = 5.0f, FColor DebugColor = FColor::Green, int32 GroupIndex = -1)
{
	GEngine->AddOnScreenDebugMessage(GroupIndex, Duration, DebugColor, Msg);
}

/*
bool UPOTLUtilFunctionLibrary::PointIndexValid(const TArray<FST_Point>& Points, int32 Index)
{
	bool valid = (Index < Points.Num()) && (Index >= 0);
	return valid;
}
*/

/*
void UPOTLUtilFunctionLibrary::GetPoint(const TArray<FST_Point>& Points, const int32 Index, bool &Found, FST_Point &Point)
{;
	Found = PointIndexValid(Points, Index);
	if (Found) {
		Point = Points[Index];
	}
}
*/

/*
bool UPOTLUtilFunctionLibrary::HexIndexValid(const TArray<FST_Hex>& Hexes, int32 Index)
{
	bool valid = (Index < Hexes.Num()) && (Index >= 0);
	return valid;
}
*/

/*
void UPOTLUtilFunctionLibrary::GetHex(const TArray<FST_Hex>& Hexes, const int32 Index, bool &Found, FST_Hex &Hex)
{
	Found = HexIndexValid(Hexes, Index);
	if (Found) {
		Hex = Hexes[Index];
	}
}
*/



/**--- AMIT ------------------------*/

int32 UPOTLUtilFunctionLibrary::GetCubeDistance(FVector CubeCoordsFrom, FVector CubeCoordsTo)
{
	int32 Distance;
	FVector DistanceVector = CubeCoordsFrom - CubeCoordsTo;
	Distance = DistanceVector.GetAbsMax();
	//float X = FMath::Max(DistanceVector.X);
	return Distance;
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


/** -- MAP - PATHING --------------------*/

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


