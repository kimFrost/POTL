// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObjects/UHexTile.h"
#include "POTLDataHolder.h"
#include "POTLUtilFunctionLibrary.generated.h"



UENUM(BlueprintType)
enum EBoolGateEnum
{
	Exits,
	Null
};



UCLASS()
class POTL_API UPOTLUtilFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UPOTLUtilFunctionLibrary(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Debug")
	static void LogMsg(FString Msg, float Duration, FColor DebugColor, int32 GroupIndex);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static FVector RotateCube(FVector CubeCoord, int32 Direction, FVector CenterCube);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static TArray<FVector> RotateCubes(TArray<FVector> CubeCoords, int32 Direction, FVector CenterCube);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static FVector RoundCube(FVector Cube);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static FVector LocationToCube(int32 GridXCount, float HexWidth, float HexHeight, FVector Location);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static int32 GetCubeDistance(FVector CubeCoordsFrom, FVector CubeCoordsTo);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static TArray<FVector> GetCubesInRange(FVector CubeCoordsFrom, int32 Range, bool IncludeFrom);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static FVector ConvertOffsetToCube(FVector2D OffsetCoords);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static FVector2D ConvertCubeToOffset(FVector CubeCoords);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static FVector AxialToCube(float Q, float R);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static int32 GetGridIndex(int32 GridWidth, int32 Column, int32 Row, bool NoWrap);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static int32 GetHexIndex(FVector2D OffsetCoord, int32 GridXCount);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static TArray<int32> CubesToHexIndexes(TArray<FVector> Cubes, int32 GridXCount);

	UFUNCTION(BlueprintCallable, Category = "Util", Meta = (ExpandEnumAsExecs = "Branches"))
	static void ActorExits(AActor* Actor, TEnumAsByte<EBoolGateEnum>& Branches);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static TArray<int32> HexesToHexIndexes(const TArray<UHexTile*>& Hexes);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static TArray<UHexTile*> SubtractHexes(const TArray<UHexTile*>& Hexes, const TArray<UHexTile*>& Subtraction);

	UFUNCTION(BlueprintCallable, Category = "Util")
	static TArray<UHexTile*> IntersectHexes(const TArray<UHexTile*>& Hexes, const TArray<UHexTile*>& Intersection);


	/** Map - Pathing */

	UFUNCTION(BlueprintCallable, Category = "Hex")
	static TArray<FVector> GetHexesWithFloodFill(FVector StartPosition, const TArray<FVector>& Obstacles, int32 Range);





};
