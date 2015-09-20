// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <string>
#include "POTLGameInstance.h"
#include "POTLUtilFunctionLibrary.h"



// Sets default values
UPOTLUtilFunctionLibrary::UPOTLUtilFunctionLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}



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

	Log("Q: " + FString::FromInt(Q) + ", R: " + FString::FromInt(R), 15.0f, FColor::Yellow, 1);
	
	return RoundCube(AxialToCube(Q, R));
	//return RoundCube(ConvertOffsetToCube(FVector2D{ Q, R }));
}

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


void UPOTLUtilFunctionLibrary::Log(FString Msg = "", float Duration = 5.0f, FColor DebugColor = FColor::Green, int32 GroupIndex = -1)
{
	GEngine->AddOnScreenDebugMessage(GroupIndex, Duration, DebugColor, Msg);
}


bool UPOTLUtilFunctionLibrary::PointIndexValid(const TArray<FST_Point>& Points, int32 Index)
{
	bool valid = (Index < Points.Num()) && (Index >= 0);
	return valid;
}

void UPOTLUtilFunctionLibrary::GetPoint(const TArray<FST_Point>& Points, const int32 Index, bool &Found, FST_Point &Point)
{;
	Found = PointIndexValid(Points, Index);
	if (Found) {
		Point = Points[Index];
	}
}

bool UPOTLUtilFunctionLibrary::HexIndexValid(const TArray<FST_Hex>& Hexes, int32 Index)
{
	bool valid = (Index < Hexes.Num()) && (Index >= 0);
	return valid;
}

void UPOTLUtilFunctionLibrary::GetHex(const TArray<FST_Hex>& Hexes, const int32 Index, bool &Found, FST_Hex &Hex)
{
	Found = HexIndexValid(Hexes, Index);
	if (Found) {
		Hex = Hexes[Index];
	}
}




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


/** Map - Creation */

TArray<FST_Point> UPOTLUtilFunctionLibrary::TraceLandscape(AActor* Landscape, int32 GridXCount, int32 GridYCount, float HexWidth, ECollisionChannel CollisionChannel)
{
	//UGameplayStatics::
	TArray<FST_Point> Points;
	
	if (Landscape != NULL)
	{
		FVector ActorLocation = Landscape->GetActorLocation();
		// Get player controller at index 0
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(Landscape->GetWorld(), 0);
		if (PlayerController)
		{
			//UWorld *World = Landscape->GetWorld();
			//const FName TraceTag("MyTraceTag");
			//World->DebugDrawTraceTag = TraceTag;

			//Landscape->GetWorld()->DebugDrawTraceTag = TraceTag;

			FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, PlayerController);
			RV_TraceParams.bTraceComplex = true;
			RV_TraceParams.bTraceAsyncScene = true;
			RV_TraceParams.bReturnPhysicalMaterial = false;
			//RV_TraceParams.TraceTag = TraceTag;

			//ECollisionChannel CollisionChannel = ECC_Pawn;

			//Re-initialize hit info
			FHitResult RV_Hit(ForceInit);

			for (int32 Row = 0; Row <= GridYCount; Row++)
			{
				for (int32 Column = 0; Column <= GridXCount; Column++)
				{
					float X = Column * (HexWidth / 2);
					float Y = (Column + Row + 1) % 2 * 74 + (Row * 255) - (34 * Row); // FindMe
					FVector LineTraceFrom = ActorLocation + FVector{ X, Y, 3000 } +FVector{ 1.f, 1.f, 0.f };
					FVector LineTraceTo = ActorLocation + FVector{ X, Y, -3000 } +FVector{ 1.f, 1.f, 0.f };

					PlayerController->GetWorld()->LineTraceSingleByChannel(RV_Hit, LineTraceFrom, LineTraceTo, CollisionChannel, RV_TraceParams);
					//if (RV_Hit.GetActor() != NULL)
					if (RV_Hit.bBlockingHit)
					{
						FST_Point Point;
						Point.Location = RV_Hit.Location;
						Point.Row = Row;
						Point.Column = Column;
						Point.Exits = true;
						Points.Add(Point);
					}

					//DrawDebugLine(Landscape->GetWorld(), LineTraceFrom, LineTraceTo, FColor(255, 0, 0), true, -1, 0, 15.0f);

				}
			}
		}
	}
	return Points;
}

TArray<FST_Hex> UPOTLUtilFunctionLibrary::CreateHexes(AActor* Landscape, const TArray<FST_Point>& Points, int32 GridXCount, float HexWidth, ECollisionChannel CollisionChannel)
{
	TArray<FST_Hex> Hexes;
	
	if (Landscape != NULL) {
		// Get player controller at index 0
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(Landscape->GetWorld(), 0);

		if (PlayerController)
		{
			FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, PlayerController);
			RV_TraceParams.bTraceComplex = true;
			RV_TraceParams.bTraceAsyncScene = true;
			RV_TraceParams.bReturnPhysicalMaterial = false;

			//const FName TraceTag("Persistent");
			//Landscape->GetWorld()->DebugDrawTraceTag = TraceTag;
			//RV_TraceParams.TraceTag = TraceTag;

			//ECollisionChannel CollisionChannel = ECC_Pawn;
			// LandscapesObjectChannel

			//Re-initialize hit info
			FHitResult RV_Hit(ForceInit);

			float X = HexWidth / 2;
			float Y = HexWidth / FMath::Sqrt(3) * 2 / 4;

			for (int32 i = 0; i < Points.Num(); i++)
			{
				FST_Point Point = Points[i];
				FVector LineTraceFrom = Point.Location + FVector{ X, Y, 3000 };
				FVector LineTraceTo = Point.Location + FVector{ X, Y, -3000 };

				int32 Creator = ((Point.Row + 1) % 2) + ((Point.Column + 2) % 2);
				if (Creator == 1)
				{
					Point.IsCreator = true;
					PlayerController->GetWorld()->LineTraceSingleByChannel(RV_Hit, LineTraceFrom, LineTraceTo, CollisionChannel, RV_TraceParams);
					//if (RV_Hit.GetActor() != NULL)
					if (RV_Hit.bBlockingHit)
					{
						FST_Hex Hex;
						Hex.Location = RV_Hit.Location;
						Hex.HexOffsetCoords = FVector2D{ (float)FMath::FloorToInt((float)Point.Column / 2), (float)FMath::FloorToInt((float)Point.Row) };
						Hex.HexCubeCoords = ConvertOffsetToCube(Hex.HexOffsetCoords);

						//Log(FString::FromInt(Point.Column) + ", " + FString::FromInt(Point.Row), 15.0f, FColor::Yellow, -1);

						// Points Ref
						int PointIndex;
						Hex.Point0 = Point;
						PointIndex = GetGridIndex(GridXCount, Point.Column + 1, Point.Row, true);
						if (PointIndexValid(Points, PointIndex))
						{
							Hex.Point1 = Points[PointIndex];
						}
						PointIndex = GetGridIndex(GridXCount, Point.Column + 2, Point.Row, true);
						if (PointIndexValid(Points, PointIndex))
						{
							Hex.Point2 = Points[PointIndex];
						}
						PointIndex = GetGridIndex(GridXCount, Point.Column + 2, Point.Row + 1, true);
						if (PointIndexValid(Points, PointIndex))
						{
							Hex.Point3 = Points[PointIndex];
						}
						PointIndex = GetGridIndex(GridXCount, Point.Column + 1, Point.Row + 1, true);
						if (PointIndexValid(Points, PointIndex))
						{
							Hex.Point4 = Points[PointIndex];
						}
						PointIndex = GetGridIndex(GridXCount, Point.Column, Point.Row + 1, true);
						if (PointIndexValid(Points, PointIndex))
						{
							Hex.Point5 = Points[PointIndex];
						}

						/*
						Hex.Point1 = Points[GetGridIndex(GridXCount, Point.Column + 1, Point.Row, true)];
						Hex.Point2 = Points[GetGridIndex(GridXCount, Point.Column + 2, Point.Row, true)];
						Hex.Point3 = Points[GetGridIndex(GridXCount, Point.Column + 2, Point.Row + 1, true)];
						Hex.Point4 = Points[GetGridIndex(GridXCount, Point.Column + 1, Point.Row + 1, true)];
						Hex.Point5 = Points[GetGridIndex(GridXCount, Point.Column, Point.Row + 1, true)];
						*/

						//DrawDebugLine(Landscape->GetWorld(), Point.Location + FVector{ 0, 0, -2000 }, Point.Location + FVector{ 0, 0, 2000 }, FColor(255, 0, 0), true, -1, 0, 15.0f);

						Hexes.Add(Hex);
					}
				}
			}
		}
	}
	return Hexes;
}

TArray<FST_Hex> UPOTLUtilFunctionLibrary::CleanHexes(TArray<FST_Hex> Hexes)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "CleanHexes");
	TArray<FST_Hex> ValidHexes;
	int32 Count = 0;
	for (int32 i = 0; i < Hexes.Num(); i++)
	{
		FST_Hex Hex = Hexes[i];
		bool Remove = (!Hex.Point0.Exits || !Hex.Point1.Exits || !Hex.Point2.Exits || !Hex.Point3.Exits || !Hex.Point4.Exits || !Hex.Point5.Exits);
		if (Remove)
		{
			Count++;
			Hex.Remove = true;
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Remove Hex");
		}
		else 
		{
			FString Msg = "Add Hex : ";
			Msg += Hex.Remove ? "true" : "false";
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, Msg);
			ValidHexes.Add(Hex);
		}
	}
	FString CountMsg = FString::FromInt(Count);

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, CountMsg);
	return ValidHexes;
}

TArray<FST_Hex> UPOTLUtilFunctionLibrary::EnrichHexes(TArray<FST_Hex> Hexes, int32 GridXCount)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "EnrichHexes");
	TArray<FST_Hex> EnrichedHexes;
	int32 Count = 0;
	for (int32 i = 0; i < Hexes.Num(); i++)
	{
		FST_Hex Hex = Hexes[i];
		// Set HexIndex
		Hex.HexIndex = i;

		TArray<FVector> CubeDirections;
		CubeDirections.Add({ 0, 1, -1 });
		CubeDirections.Add({ 1, 0, -1 });
		CubeDirections.Add({ 1, -1, 0 });
		CubeDirections.Add({ 0, -1, 1 });
		CubeDirections.Add({ -1, 0, 1 });
		CubeDirections.Add({ -1, 1, 0 });

		FVector CubeCoord = Hex.HexCubeCoords;
		for (int32 ii = 0; ii < CubeDirections.Num(); ii++)
		{
			FVector CubeDirection = CubeDirections[ii];
			FVector CombinedVector = CubeCoord + CubeDirection;
			FVector2D OffsetCoords = ConvertCubeToOffset(CombinedVector);
			int32 HexDirectionIndex = GetHexIndex(OffsetCoords, GridXCount);
			if (Hexes.IsValidIndex(HexDirectionIndex))
			{
				Hex.HexNeighborIndexes[ii] = HexDirectionIndex;
			}
		}

		EnrichedHexes.Add(Hex);
	}
	return EnrichedHexes;
}

TArray<FST_Hex> UPOTLUtilFunctionLibrary::CalcHexesRot(TArray<FST_Hex> Hexes, float HexWidth)
{
	TArray<FST_Hex> CalcedHexes;
	float HexRealHeight = HexWidth / FMath::Sqrt(3) * 2;
	for (int32 i = 0; i < Hexes.Num(); i++)
	{
		FST_Hex Hex = Hexes[i];
		FRotator Rotation;

		float Angle;
		float xDiff;
		float yDiff;

		/*
		FVector FirstPoint = (Hex.Point2.Location - Hex.Location).GetSafeNormal();
		FVector SecondPoint = (Hex.Point0.Location - Hex.Location).GetSafeNormal();

		// float AimAtAngle = ((acosf(FVector::DotProduct(PlayerDirection, MouseDirection))) * (180 / 3.1415926));
		// float AimAtAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(PlayerDirection, MouseDirection)));

		float DotProduct = FVector::DotProduct(FirstPoint, SecondPoint);
		float Radians = acosf(DotProduct);
		Angle = FMath::RadiansToDegrees(Radians);
		*/

		xDiff = Hex.Point2.Location.X - Hex.Point0.Location.X;
		yDiff = Hex.Point2.Location.Z - Hex.Point0.Location.Z;
		Angle = FMath::Atan2(yDiff, xDiff) * (180 / 3.141592);
		Rotation.Pitch = Angle;

		//Rotation.Pitch = (HexWidth / (Hex.Point2.Location.Z - Hex.Point0.Location.Z)) / 100 * 45 / 100;
		//Rotation.Pitch = 45.0f;

		Rotation.Yaw = 0.0f;

		xDiff = Hex.Point4.Location.Y - Hex.Point1.Location.Y;
		yDiff = Hex.Point4.Location.Z - Hex.Point1.Location.Z;
		Angle = FMath::Atan2(yDiff, xDiff) * (180 / 3.141592) * -1;
		Rotation.Roll = Angle;

		//Rotation.Roll = (HexRealHeight / (Hex.Point1.Location.Z - Hex.Point4.Location.Z)) / 100 * 45 / 100;
		//Rotation.Roll = 0.0f;
		Hex.Rotation = Rotation;

		CalcedHexes.Add(Hex);
		//Log(FString::SanitizeFloat(Rotation.Pitch), 15.0f, FColor::Yellow, -1);
	}
	return CalcedHexes;
	//return Hexes;
}



/** -- PATCHING --------------------*/

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


