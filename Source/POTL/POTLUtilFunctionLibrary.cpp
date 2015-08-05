// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLUtilFunctionLibrary.h"



// Sets default values
UPOTLUtilFunctionLibrary::UPOTLUtilFunctionLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}



FString UPOTLUtilFunctionLibrary::FindThisFunction(FString ReturnString)
{




	return ReturnString;
}


TArray<FVector> UPOTLUtilFunctionLibrary::GetHexesWithFloodFill(FVector StartPosition, TArray<FVector> Obstacles, int const Range)
{
	struct Fridge
	{
		TArray<FVector> CubeCoords;
	};

	TArray<FVector> VisitedCubeCoords;
	TArray<FVector> ReachableCubeCoords;
	//TArray<Fridge> Fridges;
	FVector Fridges[100][6];
	
	TArray<FVector> CubeDirections;

	CubeDirections.Add({ 1, 0, -1 });
	CubeDirections.Add({ 1, -1, 0 });
	CubeDirections.Add({ 0, -1, 1 });
	CubeDirections.Add({ -1, 0, 1 });
	CubeDirections.Add({ -1, 1, 0 });
	CubeDirections.Add({ 0, 1, -1 });

	// Add StartPosition to cubecoords already visited 
	//VisitedCubeCoords.Add(StartPosition);
	//Fridges[0] = FVector[];

	//Fridge fridge = Fridge{0, 0, 0};
	//Fridges.Add(fridge);

	Fridges[0][0] = FVector{ StartPosition };


	for (int32 k = 1; k < Range; k++)
	{
		//Fridges[0][0] = FVector[];
		
		//FVector asdasasd[] = Fridges[0];

		for (int32 k = 1; k < Range; k++)
		{

		}
	}



	//auto array = new double[5][5];
	//int **ary = new int*[];



	return VisitedCubeCoords;
}