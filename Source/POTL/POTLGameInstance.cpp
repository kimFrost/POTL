// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLDataHolder.h"
#include "UObjects/UHexPoint.h"
#include "UObjects/UHexTile.h"
#include "UObjects/UResourceMap.h"
#include "UObjects/UStorageMap.h"
#include "POTLUtilFunctionLibrary.h"
#include "POTLStructure.h"
#include "Components/UStructureComponent.h"
#include "Components/UStorageComponent.h"
#include "Kismet/GameplayStatics.h"
//#include "Runtime/Foliage/Public/FoliageInstancedStaticMeshComponent.h"
//#include "Components/InstancedStaticMeshComponent.h"
#include "POTLGameInstance.h"



// Sets default values
UPOTLGameInstance::UPOTLGameInstance(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
	HexWidth = 255.0f;
	HexHeight = HexWidth / FMath::Sqrt(3) * 2.f;
	Landscape = nullptr;
	GridXCount = 200; // Temp. Needs to be calc in point creation.
	GridYCount = 200; // Temp. Needs to be calc in point creation.
	HexGridReady = false;

	//~~ Table data ~~//
	DATA_Recipes = nullptr;
	DATA_Structures = nullptr;
	DATA_Resources = nullptr;

	//~~ For setting custom channels from blueprint ~~//
	ChannelLandscape = ECollisionChannel::ECC_WorldStatic;
	ChannelFoliage = ECollisionChannel::ECC_WorldStatic;

}


/******************** ReadTables *************************/
void UPOTLGameInstance::ReadTables()
{
	//auto cls = StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("/Game/Wood_Table_Blueprint")); 
	//UObject* Asset = (UObject*)StaticLoadObject(UBlueprint::StaticClass(), nullptr, TEXT("DataTable'/Game/Resources/Structures'"));
	//~~ Structures ~~//
	UDataTable* StructureTable = (UDataTable*)StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("DataTable'/Game/Resources/Structures.Structures'"));
	if (StructureTable)
	{
		DATA_Structures = StructureTable;
	}
	//~~ Recipes ~~//
	UDataTable* RecipeTable = (UDataTable*)StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("DataTable'/Game/Resources/ResourceRecipies.ResourceRecipies'"));
	if (RecipeTable)
	{
		DATA_Recipes = RecipeTable;
	}
	//~~ Resources ~~//
	UDataTable* ResourceTable = (UDataTable*)StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("DataTable'/Game/Resources/Resources.Resources'"));
	if (ResourceTable)
	{
		DATA_Resources = ResourceTable;
	}
}



/******************** IsHexBlocked *************************/
bool UPOTLGameInstance::IsHexBlocked(const UHexTile* Hex)
{
	bool Blocked = false;
	if (IsValid(Hex))
	{
		if (!IsHexTerrainBuildable(Hex))
		{
			Blocked = true;
		}
		else if (Hex->AttachedBuilding)
		{
			if (Hex->AttachedBuilding->BlockPathing)
			{
				Blocked = true;
			}
		}
	}
	return Blocked;
}


/******************** IsHexBuildable *************************/
bool UPOTLGameInstance::IsHexBuildable(const UHexTile* Hex)
{
	bool Buildable = true;
	if (IsValid(Hex))
	{
		if (!IsHexTerrainBuildable(Hex))
		{
			Buildable = false;
		}
		else if (Hex->AttachedBuilding)
		{
			if (!Hex->AttachedBuilding->IsPlaceholder)
			{
				Buildable = false;
			}
		}
	}
	return Buildable;
}


/******************** IsHexTerrainBuildable *************************/
bool UPOTLGameInstance::IsHexTerrainBuildable(const UHexTile* Hex)
{
	if (IsValid(Hex))
	{
		//FRotator HexRotation = Hex->Rotation;
		FVector HexRotation = FVector(Hex->Rotation.Pitch, Hex->Rotation.Yaw, Hex->Rotation.Roll);
		float maxFlatDiviation = HexRotation.GetAbsMax();
		return (maxFlatDiviation <= 15.f);
	}
	else
	{
		return false;
	}
}


/******************** PlantPlaceholderStructure *************************/
APOTLStructure* UPOTLGameInstance::PlantPlaceholderStructure(FVector CubeCoord, int32 RotationDirection, FString RowName, FString TreeId, APOTLStructure* EmitTo, bool InstaBuild)
{
	APOTLStructure* Structure = nullptr;
	Structure = PlantStructure(CubeCoord, RotationDirection, RowName, TreeId, EmitTo, InstaBuild, true);
	if (Structure)
	{
		Structure->IsPlaceholder = true;
		PlaceholderStructures.Add(Structure);
	}
	return Structure;
}


/******************** PlantStructure *************************/
APOTLStructure* UPOTLGameInstance::PlantStructure(FVector CubeCoord, int32 RotationDirection, FString RowName, FString TreeId, APOTLStructure* EmitTo, bool InstaBuild, bool IsPlaceholder)
{
	APOTLStructure* Structure = nullptr;
	if (Landscape && DATA_Structures)
	{
		static const FString ContextString(TEXT("GENERAL")); //~~ Key value for each column of values ~~//
		FST_Structure* StructureData = DATA_Structures->FindRow<FST_Structure>(*RowName, ContextString);
		if (StructureData)
		{
			FVector2D OffsetCoords = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(CubeCoord);
			int32 HexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(OffsetCoords, GridXCount);
			if (Hexes.IsValidIndex(HexIndex))
			{
				UHexTile* Hex = Hexes[HexIndex];
				if (!IsValid(Hex)) {
					return nullptr;
				}
				UWorld* World = Landscape->GetWorld();
				if (World)
				{
					//~~ Set the spawn parameters ~~//
					FActorSpawnParameters SpawnParams;
					//SpawnParams.Owner = this;
					//SpawnParams.Instigator = Instigator;
					//SpawnParams.bNoCollisionFail = true; //~~ Spawn event if collision ~~//
					SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					FVector SpawnLocation = Hex->Location;
					FRotator SpawnRotation = Hex->Rotation + FRotator(0, RotationDirection * (360 / 6), 0);
					//FRotator SpawnRotation = Hex->Rotation;

					// Spawn the pickup
					//APOTLStructure* const
					Structure = World->SpawnActor<APOTLStructure>(StructureData->StructureClass, SpawnLocation, SpawnRotation, SpawnParams);
					if (Structure)
					{

						//~~ Store cubecoord in structure ~~//
						Structure->CubeCoord = CubeCoord;

						//~~ Set Structure broadcast root hexindex on structure ~~// //!! Rotate logic is missing, I think ?
						FVector BroadcastCubeCoord = StructureData->BroadcastRoot + CubeCoord;
						BroadcastCubeCoord = UPOTLUtilFunctionLibrary::RotateCube(BroadcastCubeCoord, RotationDirection, CubeCoord);
						FVector2D BroadcastOffsetCoords = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(BroadcastCubeCoord);
						int32 BroadcastHexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(BroadcastOffsetCoords, GridXCount);
						if (Hexes.IsValidIndex(BroadcastHexIndex))
						{
							UHexTile* Hex = Hexes[BroadcastHexIndex];
							Structure->Hex = Hex;
						}

						//~~ Store hex index in structure ~~// //~~ CubeCoord is the rotation center cube coord ~~//
						FVector2D OffsetCoords = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(CubeCoord);
						int32 HexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(OffsetCoords, GridXCount);
						if (Hexes.IsValidIndex(HexIndex))
						{
							Structure->HexIndex = HexIndex;
						}

						//~~ Set Structure on all hexes based on cube location and structure size ~~//
						for (int32 i = 0; i < StructureData->CubeSizes.Num(); i++)
						{
							FVector LocalCubeCoord = StructureData->CubeSizes[i] + CubeCoord;
							LocalCubeCoord = UPOTLUtilFunctionLibrary::RotateCube(LocalCubeCoord, RotationDirection, CubeCoord);
							FVector2D OffsetCoords = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(LocalCubeCoord);
							int32 HexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(OffsetCoords, GridXCount);
							if (Hexes.IsValidIndex(HexIndex))
							{
								UHexTile* Hex = Hexes[HexIndex];
								if (IsValid(Hex))
								{
									Hex->AttachedBuilding = Structure;
								}
							}
						}

						//~~ Store structure raw data ~~//
						Structure->StructureRowName = RowName;
						Structure->StructureBaseData = *StructureData;
						Structure->BroadcastRange = StructureData->BaseBroadcastRange; //!! Use a read/load function instead
						Structure->StructureBaseData.RotationDirection = RotationDirection;
						Structure->IsPlaceholder = IsPlaceholder;
						if (Structure->IsPlaceholder)
						{

						}
						//~~ Create Broadcast/Emit Connection ~~//
						if (EmitTo)
						{
							CreateStructureConnection(Structure, EmitTo);
						}
						//~~ InstaBuild for debugging ~~//
						if (InstaBuild)
						{
							Structure->IsUnderConstruction = false;
						}
						//~~ Process Structure Data internally ~~//
						Structure->ProcessBaseData();
					}
				}
			}
		}
	}
	OnStructurePlanted.Broadcast(Structure);
	return Structure;
}


/******************** RemoveStructure *************************/
void UPOTLGameInstance::RemoveStructure(APOTLStructure* Structure)
{
	if (Structure)
	{
		RemoveStructureConnection(Structure, Structure->EmitTo);
		for (int32 i = 0; i < Structure->BroadcastTo.Num(); i++)
		{
			RemoveStructureConnection(Structure->BroadcastTo[i], Structure);
		}
		//~~ Remove self from hexes ~~//
		// Sizes cubecoords logic here
		FST_Structure& StructureBaseData = Structure->StructureBaseData;
		int RotationDirection = StructureBaseData.RotationDirection;

		for (int32 i = 0; i < StructureBaseData.CubeSizes.Num(); i++)
		{
			FVector LocalCubeCoord = StructureBaseData.CubeSizes[i] + Structure->CubeCoord; 
			LocalCubeCoord = UPOTLUtilFunctionLibrary::RotateCube(LocalCubeCoord, RotationDirection, Structure->CubeCoord);
			FVector2D OffsetCoords = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(LocalCubeCoord);
			int32 HexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(OffsetCoords, GridXCount);
			if (Hexes.IsValidIndex(HexIndex))
			{
				UHexTile* Hex = Hexes[HexIndex];
				if (IsValid(Hex) && Hex->AttachedBuilding == Structure)
				{
					Hex->AttachedBuilding = nullptr; //~~ Remove pointer set on hex ~~//
				}
			}
		}
		//~~ DESTROY ~~//
		Structure->Destroy();
	}
}



/*****************************************************************************************************/
/**************************************** MAP - CREATION *********************************************/
/*****************************************************************************************************/

/******************** TraceLandscape *************************/
void UPOTLGameInstance::TraceLandscape()
{
	StorageMap = NewObject<UStorageMap>();
	ResourceMap = NewObject<UResourceMap>();

	FString Tadasdadas = "adasd";
	//UGameplayStatics::
	if (Landscape)
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
					float Y = (Column + Row + 1) % 2 * 74 + (Row * 255) - (34 * Row); 
					FVector LineTraceFrom = ActorLocation + FVector{ X, Y, 3000 } +FVector{ 1.f, 1.f, 0.f };
					FVector LineTraceTo = ActorLocation + FVector{ X, Y, -3000 } +FVector{ 1.f, 1.f, 0.f };

					PlayerController->GetWorld()->LineTraceSingleByChannel(RV_Hit, LineTraceFrom, LineTraceTo, ChannelLandscape, RV_TraceParams);
					//if (RV_Hit.GetActor() != NULL)
					if (RV_Hit.bBlockingHit)
					{
						UHexPoint* Point = NewObject<UHexPoint>();
						Point->Location = RV_Hit.Location;
						Point->Row = Row;
						Point->Column = Column;
						Point->Exits = true;
						Points.Add(Point);
					}
				}
			}
		}
	}
}


/******************** CreateHexes *************************/
void UPOTLGameInstance::CreateHexes()
{
	if (Landscape != NULL) {
		// Get player controller at index 0
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(Landscape->GetWorld(), 0);

		if (PlayerController)
		{
			FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, PlayerController);
			RV_TraceParams.bTraceComplex = true;
			RV_TraceParams.bTraceAsyncScene = true;
			RV_TraceParams.bReturnPhysicalMaterial = false;

			// Reset hexes
			Hexes.Empty();

			//Re-initialize hit info
			FHitResult RV_Hit(ForceInit);

			float X = HexWidth / 2;
			float Y = HexWidth / FMath::Sqrt(3) * 2 / 4;

			for (int32 i = 0; i < Points.Num(); i++)
			{
				UHexPoint* Point = Points[i];
				FVector LineTraceFrom = Point->Location + FVector{ X, Y, 3000 };
				FVector LineTraceTo = Point->Location + FVector{ X, Y, -3000 };

				int32 Creator = ((Point->Row + 1) % 2) + ((Point->Column + 2) % 2);
				if (Creator == 1)
				{
					Point->IsCreator = true;
					PlayerController->GetWorld()->LineTraceSingleByChannel(RV_Hit, LineTraceFrom, LineTraceTo, ChannelLandscape, RV_TraceParams);
					if (RV_Hit.bBlockingHit)
					{
						UHexTile* Hex = NewObject<UHexTile>();
						Hex->Location = RV_Hit.Location;
						Hex->HexOffsetCoords = FVector2D{ (float)FMath::FloorToInt((float)Point->Column / 2), (float)FMath::FloorToInt((float)Point->Row) };
						Hex->HexCubeCoords = UPOTLUtilFunctionLibrary::ConvertOffsetToCube(Hex->HexOffsetCoords);

						// Points Ref
						int32 PointIndex = -1;
						Hex->Point0 = Point;
						
						PointIndex = UPOTLUtilFunctionLibrary::GetGridIndex(GridXCount, Point->Column + 1, Point->Row, true);
						
						if (Points.IsValidIndex(PointIndex))
						{
							Hex->Point1 = Points[PointIndex];
						}
						PointIndex = UPOTLUtilFunctionLibrary::GetGridIndex(GridXCount, Point->Column + 2, Point->Row, true);
						if (Points.IsValidIndex(PointIndex))
						{
							Hex->Point2 = Points[PointIndex];
						}
						PointIndex = UPOTLUtilFunctionLibrary::GetGridIndex(GridXCount, Point->Column + 2, Point->Row + 1, true);
						if (Points.IsValidIndex(PointIndex))
						{
							Hex->Point3 = Points[PointIndex];
						}
						PointIndex = UPOTLUtilFunctionLibrary::GetGridIndex(GridXCount, Point->Column + 1, Point->Row + 1, true);
						if (Points.IsValidIndex(PointIndex))
						{
							Hex->Point4 = Points[PointIndex];
						}
						PointIndex = UPOTLUtilFunctionLibrary::GetGridIndex(GridXCount, Point->Column, Point->Row + 1, true);
						if (Points.IsValidIndex(PointIndex))
						{
							Hex->Point5 = Points[PointIndex];
						}

						Hex->WorldRef = Landscape->GetWorld();
						Hex->Init();

						Hexes.Add(Hex);
					}
				}
			}
		}
	}
}


/******************** CleanHexes *************************/
void UPOTLGameInstance::CleanHexes()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "CleanHexes");
	TArray<UHexTile*> ValidHexes;
	int32 Count = 0;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Hexes.Num() - before clean" + FString::FromInt(Hexes.Num()));
	for (int32 i = 0; i < Hexes.Num(); i++)
	{
		UHexTile* Hex = Hexes[i];
		if (IsValid(Hex))
		{
			bool Remove = (!Hex->Point0 || !Hex->Point1 || !Hex->Point2 || !Hex->Point3 || !Hex->Point4 || !Hex->Point5);
			if (Remove)
			{
				Count++;
				Hex->Remove = true;
			}
			else
			{
				FString Msg = "Add Hex : ";
				Msg += Hex->Remove ? "true" : "false";
				ValidHexes.Add(Hex);
			}
		}
	}
	FString CountMsg = FString::FromInt(Count);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, CountMsg);

	Hexes = ValidHexes;
}


/******************** EnrichHexes *************************/
void UPOTLGameInstance::EnrichHexes()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "EnrichHexes");
	int32 Count = 0;
	for (int32 i = 0; i < Hexes.Num(); i++)
	{
		UHexTile* Hex = Hexes[i];
		if (IsValid(Hex))
		{
			Hex->HexIndex = i; // Set HexIndex

			TArray<FVector> CubeDirections;
			CubeDirections.Add({ 0, 1, -1 });
			CubeDirections.Add({ 1, 0, -1 });
			CubeDirections.Add({ 1, -1, 0 });
			CubeDirections.Add({ 0, -1, 1 });
			CubeDirections.Add({ -1, 0, 1 });
			CubeDirections.Add({ -1, 1, 0 });

			FVector CubeCoord = Hex->HexCubeCoords;
			for (int32 ii = 0; ii < CubeDirections.Num(); ii++)
			{
				FVector CubeDirection = CubeDirections[ii];
				FVector CombinedVector = CubeCoord + CubeDirection;

				FVector2D OffsetCoords = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(CombinedVector);
				int32 HexDirectionIndex = UPOTLUtilFunctionLibrary::GetHexIndex(OffsetCoords, GridXCount);
				if (Hexes.IsValidIndex(HexDirectionIndex))
				{
					Hex->HexNeighborIndexes[ii] = HexDirectionIndex;
				}
			}
		}
	}
}


/******************** CalcHexesRot *************************/
void UPOTLGameInstance::CalcHexesRot()
{
	float HexRealHeight = HexWidth / FMath::Sqrt(3) * 2;
	for (int32 i = 0; i < Hexes.Num(); i++)
	{
		UHexTile* Hex = Hexes[i];
		if (IsValid(Hex))
		{
			FRotator Rotation = FRotator(0.f, 0.f, 0.f);

			float Angle;
			float xDiff;
			float yDiff;

			xDiff = Hex->Point2->Location.X - Hex->Point0->Location.X;
			yDiff = Hex->Point2->Location.Z - Hex->Point0->Location.Z;
			Angle = FMath::Atan2(yDiff, xDiff) * (180 / 3.141592);
			Rotation.Pitch = Angle;

			Rotation.Yaw = 0.0f;

			xDiff = Hex->Point4->Location.Y - Hex->Point1->Location.Y;
			yDiff = Hex->Point4->Location.Z - Hex->Point1->Location.Z;
			Angle = FMath::Atan2(yDiff, xDiff) * (180 / 3.141592) * -1;
			Rotation.Roll = Angle;

			Hex->Rotation = Rotation;
		}
	}
}


/******************** AnalyseLandscape *************************/
void UPOTLGameInstance::AnalyseLandscape()
{
	//UGameplayStatics::
	if (Landscape)
	{
		FVector ActorLocation = Landscape->GetActorLocation();
		// Get player controller at index 0
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(Landscape->GetWorld(), 0);
		if (PlayerController)
		{
			//~~ Sphere trace for foliage analyse ~~//
			for (int32 i = 0; i < Hexes.Num(); i++)
			{
				UHexTile* Hex = Hexes[i];
				if (IsValid(Hex))
				{
					FVector LineTraceFrom = Hex->Location + FVector{ 0, 0, 1000 };
					FVector LineTraceTo = Hex->Location + FVector{ 0, 0, -1000 };

					//const FCollisionResponseParams & ResponseParam

					//const FName TraceTag("TraceAnalyseResources");
					//PlayerController->GetWorld()->DebugDrawTraceTag = TraceTag;

					FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, PlayerController);
					RV_TraceParams.bTraceComplex = true;
					RV_TraceParams.bTraceAsyncScene = true;
					RV_TraceParams.bReturnPhysicalMaterial = false;
					//RV_TraceParams.TraceTag = TraceTag;

					//TArray<FHitResult> OutHits;
					TArray<FOverlapResult> OutHits;

					PlayerController->GetWorld()->OverlapMultiByChannel(OutHits, Hex->Location, FQuat::Identity, ChannelFoliage, FCollisionShape::MakeSphere(666), RV_TraceParams);
					for (int32 ii = 0; ii < OutHits.Num(); ii++)
					{
						FOverlapResult& OutHit = OutHits[ii];
						if (OutHit.bBlockingHit)
						{
							UPrimitiveComponent* Component = OutHit.GetComponent();
							FString ComponentClassName = "";

							UInstancedStaticMeshComponent* MeshKeeper = Cast<UInstancedStaticMeshComponent>(Component);
							if (MeshKeeper)
							{
								FString adasds = "jjasdasd";
							}
						
							//UFoliageType_InstancedStaticMesh
							//UFoliageInstancedStaticMeshComponent* MeshKeeper = Cast<UFoliageInstancedStaticMeshComponent>(Component);
							/*
							if (MeshKeeper)
							{

							}
							*/
						}
					}
				}

				//OverlapMultiByChannel(Overlaps, TestLocation, FQuat::Identity, BlockingChannel, FCollisionShape::MakeCapsule(FMath::Max(Capsule->GetScaledCapsuleRadius() - Epsilon, 0.1f), FMath::Max(Capsule->GetScaledCapsuleHalfHeight() - Epsilon, 0.1f)), Params);
				//TArray hitList;
				//PlayerController->GetWorld()->SweepMulti(hitList, startLocation, startLocation + radius, FQuat(1.0f, 1.0f, 1.0f, 1.0f), ECollisionChannel::ECC_Pawn, collShape, queryParam);
			}

			//~~ Line trace for material and river analyse ~~//
			for (int32 i = 0; i < Hexes.Num(); i++)
			{
				UHexTile* Hex = Hexes[i];
				if (IsValid(Hex))
				{
					FVector LineTraceFrom = Hex->Location + FVector{ 0, 0, 1000 };
					FVector LineTraceTo = Hex->Location + FVector{ 0, 0, -1000 };

					FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, PlayerController);
					RV_TraceParams.bTraceComplex = true;
					RV_TraceParams.bTraceAsyncScene = true;
					RV_TraceParams.bReturnPhysicalMaterial = true;

					//Re-initialize hit info
					FHitResult RV_Hit(ForceInit);

					PlayerController->GetWorld()->LineTraceSingleByChannel(RV_Hit, LineTraceFrom, LineTraceTo, ChannelLandscape, RV_TraceParams);

					//PlayerController->GetWorld()->SweepMultiByChannel(RV_Hit, LineTraceFrom, LineTraceTo, CollisionChannel, RV_TraceParams);
					//if (RV_Hit.GetActor() != NULL)
					if (RV_Hit.bBlockingHit)
					{
						//RV_Hit.Location;
					}
				}
			}
		}
	}
}


/******************** CalcHexResourceDensity *************************/
void UPOTLGameInstance::CalcHexResourceDensity()
{
	int32 MaxForestDepth = 5;
	int32 MaxLakeDepth = 3;
	int32 DepthIndex;
	// Forest Depth
	for (DepthIndex = 0; DepthIndex < MaxForestDepth; DepthIndex++)
	{
		for (int32 i = 0; i < Hexes.Num(); i++)
		{
			UHexTile* Hex = Hexes[i];
			if (IsValid(Hex) && Hex->Resources.HasTrees)
			{
				bool Surrounded = true;
				for (int32 ii = 0; ii < Hex->HexNeighborIndexes.Num(); ii++)
				{
					int32 HexNeighborIndex = Hex->HexNeighborIndexes[ii];
					if (Hexes.IsValidIndex(HexNeighborIndex))
					{
						UHexTile* NeighborHex = Hexes[HexNeighborIndex];
						if (IsValid(NeighborHex) && !NeighborHex->Resources.HasTrees || NeighborHex->Resources.ForestDepth < DepthIndex)
						{
							Surrounded = false;
						}
					}
					else
					{
						Surrounded = false;
					}
				}
				if (Surrounded)
				{
					Hex->Resources.ForestDepth++;
				}
			}
		}
	}
	// Lake Depth
	for (DepthIndex = 0; DepthIndex < MaxLakeDepth; DepthIndex++)
	{
		for (int32 i = 0; i < Hexes.Num(); i++)
		{
			UHexTile* Hex = Hexes[i];
			if (IsValid(Hex) && Hex->Resources.HasLake)
			{
				bool Surrounded = true;
				for (int32 ii = 0; ii < Hex->HexNeighborIndexes.Num(); ii++)
				{
					int32 HexNeighborIndex = Hex->HexNeighborIndexes[ii];
					if (Hexes.IsValidIndex(HexNeighborIndex))
					{
						UHexTile* NeighborHex = Hexes[HexNeighborIndex];
						if (IsValid(NeighborHex) && !NeighborHex->Resources.HasLake || NeighborHex->Resources.LakeDepth < DepthIndex)
						{
							Surrounded = false;
						}
					}
					else
					{
						Surrounded = false;
					}
				}
				if (Surrounded)
				{
					Hex->Resources.LakeDepth++;
				}
			}
		}
	}
}


/*****************************************************************************************************/
/****************************************** Turn *****************************************************/
/*****************************************************************************************************/

/******************** SwitchTurn *************************/
void UPOTLGameInstance::SwitchTurn()
{
	OnTurnSwitched.Broadcast(32.f);
	//GetWorld()->GetTimerManager().SetTimer(this, &UPOTLGameInstance::NewTurn, 5.0f, false);
	//TimerManager->SetTimer(TurnTimerHandle, this, &UPOTLGameInstance::NewTurn, 1.0f, false);

	NewTurn(0.2f);
}

/******************** NewTurn *************************/
void UPOTLGameInstance::NewTurn(float WaitTime)
{
	if (Landscape)
	{
		bool AllResolved = true;
		for (int32 i = 0; i < Structures.Num(); i++)
		{
			APOTLStructure* Structure = Structures[i];
			if (!Structure->IsResolvedThisTurn)
			{
				AllResolved = false;
				break;
			}
		}
		if (AllResolved)
		{
			OnNewTurn.Broadcast(17.f);
		}
		else {
			FTimerHandle UniqueHandle;
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UPOTLGameInstance::NewTurn, WaitTime * 2); //~~ Doubles wait time for each loop ~~//
			Landscape->GetWorldTimerManager().SetTimer(UniqueHandle, TimerDelegate, WaitTime, false);
		}
	}
}

/*****************************************************************************************************/
/************************************** Map - Structure **********************************************/
/*****************************************************************************************************/

/******************** GetNearestStructure *************************/
APOTLStructure* UPOTLGameInstance::GetNearestStructure(FVector Location, TSubclassOf<APOTLStructure> StructureClass)
{
	APOTLStructure* NearestStructure = nullptr;
	float LastNearestDistance = 99999999999999.f;
	TArray<AActor*> FoundStructures;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), StructureClass, FoundStructures);
	for (auto Actor : FoundStructures)
	{
		APOTLStructure* Structure = Cast<APOTLStructure>(Actor);
		if (Structure)
		{
			FVector StructureLocation = Structure->GetActorLocation();
			FVector VectorDistance = StructureLocation - Location;
			VectorDistance = VectorDistance.GetAbs();
			//FVector2D Vector2DDistance = FVector2D({ VectorDistance.X, VectorDistance.Y });
			FVector2D Vector2DDistance = FVector2D::FVector2D(VectorDistance.X, VectorDistance.Y);
			float Distance = Vector2DDistance.Size();
			if (Distance < LastNearestDistance)
			{
				NearestStructure = Structure;
				LastNearestDistance = Distance;
			}
		}
	}
	return NearestStructure;
}



/******************** GetNearestCity *************************/
APOTLStructure* UPOTLGameInstance::GetNearestCity(FVector Location)
{
	APOTLStructure* RootStructure = nullptr;
	float LastNearestDistance = 99999999999999.f;
	for (auto Structure : RootStructures)
	{
		FVector StructureLocation = Structure->GetActorLocation();
		FVector VectorDistance = StructureLocation - Location;
		VectorDistance = VectorDistance.GetAbs();
		//FVector2D Vector2DDistance = FVector2D({ VectorDistance.X, VectorDistance.Y });
		FVector2D Vector2DDistance = FVector2D::FVector2D(VectorDistance.X, VectorDistance.Y);
		float Distance = Vector2DDistance.Size();
		if (Distance < LastNearestDistance)
		{
			RootStructure = Structure;
			LastNearestDistance = Distance;
		}
	}
	return RootStructure;
}


/******************** IncludeStorage *************************/
void UPOTLGameInstance::IncludeStorage(UStorageComponent* StorageComp)
{
	if (StorageMap)
	{
		StorageMap->IncludeStorage(StorageComp);
	}
}


/******************** FindResource *************************/
UStorageComponent * UPOTLGameInstance::FindResource(FString ResourceId, int Quantity)
{
	if (StorageMap)
	{
		
	}
	return nullptr;
}


/******************** RequestResource *************************/
UResource* UPOTLGameInstance::RequestResource(APOTLStructure* Requester, FString ResourceId)
{
	if (StorageMap)
	{
		return StorageMap->RequestResource(Requester, ResourceId);
	}
	return nullptr;
}


/******************** TransferResource *************************/
void UPOTLGameInstance::TransferResource(UResource* Resource, UStructureComponent* ToComp, bool Consume = false, bool IsFree = false)
{
	if (Resource)
	{
		if (ToComp)
		{
			if (Consume) {
				Resource->Consume();
			}
			else {
				UStorageComponent* StorageComp = Cast<UStorageComponent>(ToComp);
				if (StorageComp)
				{
					bool Transfered = Resource->Transfer(StorageComp);
					if (Transfered)
					{

					}
				}
			}
			// Trigger transaction result in wealth/benifit
			//TODO: Add transaction here
		}
		else if (Consume) {
			Resource->Consume();
			// No wealth. There is no end target
		}
	}

	// From Storage
	// To Storage

	// Consume

}



/*****************************************************************************************************/
/******************************************* PEOPLE **************************************************/
/*****************************************************************************************************/


/******************** CreatePerson *************************/
int32 UPOTLGameInstance::CreatePerson(FString FirstName, FString FamilyName, FString NickName, int32 Age, EPersonGender Gender, APOTLStructure* Home)
{
	int32 Index = 0;
	FST_Person Person;
	Person.FirstName = FirstName;
	Person.FamilyName = FamilyName;
	Person.NickName = NickName;
	Person.Age = Age;
	Person.Gender = Gender;
	Person.Home = Home;

	do 
	{
		Index = FMath::RandRange(0, 50000);
	} while (PeopleInMap.Contains(Index));
	Person.OwnIndex = Index;

	if (Home)
	{
		Home->PeopleIndexes.Add(Index);
	}
	PeopleInMap.Add(Index, Person);
	return Index;
}


/******************** SwitchHome *************************/
void UPOTLGameInstance::SwitchHome(UPARAM(ref) FST_Person& Person, APOTLStructure* NewHome)
{
	if (Person.Home)
	{
		if (Person.Home->PeopleIndexes.Contains(Person.OwnIndex))
		{
			Person.Home->PeopleIndexes.Remove(Person.OwnIndex);
		}
	}
	Person.Home = NewHome;
	NewHome->PeopleIndexes.Add(Person.OwnIndex);
}


/******************** GetPerson *************************/
FST_Person UPOTLGameInstance::GetPerson(int32 Index)
{
	FST_Person Person;
	if (PeopleInMap.Contains(Index))
	{
		Person = PeopleInMap[Index];
	}
	return Person;
}


/******************** GetPeople *************************/
TArray<FST_Person> UPOTLGameInstance::GetPeople(const TArray<int32>& Indexes)
{
	TArray<FST_Person> People;
	for (int32 i = 0; i < Indexes.Num(); i++)
	{
		int32 Index = Indexes[i];
		if (PeopleInMap.Contains(Index))
		{
			People.Add(PeopleInMap[Index]);
		}
	}
	return People;
}


/******************** SetPersonData *************************/
void  UPOTLGameInstance::SetPersonData(APOTLStructure* AssignedTo, EPersonBaseTaskList AssignedTask, int32 OwnIndex)
{
	if (PeopleInMap.Contains(OwnIndex))
	{
		FST_Person& Person = PeopleInMap[OwnIndex];
		Person.AssignedTo = AssignedTo;
		Person.AssignedTask = AssignedTask;
	}
	/*
	TArray<FString> Groups;
	TArray<EPersonTags> Tags;
	int32 Age;
	TMap<FString, int32> Modifiers;
	TMap<FString, int32> Traits;
	EPersonGender Gender;
	EPersonTypesEnum Type;
	APOTLStructure* Home;
	APOTLStructure* AssignedTo;
	EPersonBaseTaskList AssignedTask;
	int32 OwnIndex;
	*/
}


/*****************************************************************************************************/
/**************************************** UTIL - Hex *************************************************/
/*****************************************************************************************************/

/******************** MouseToHex *************************/
UHexTile* UPOTLGameInstance::MouseToHex()
{
	UHexTile* Hex = nullptr;
	if (Landscape)
	{
		FVector LandscapeLocation = Landscape->GetActorLocation();
		// Get player controller at index 0
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(Landscape->GetWorld(), 0);
		if (PlayerController)
		{

			//const FName TraceTag("MyTraceTag");
			//Landscape->GetWorld()->DebugDrawTraceTag = TraceTag;

			FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, PlayerController);
			RV_TraceParams.bTraceComplex = true;
			RV_TraceParams.bTraceAsyncScene = true;
			RV_TraceParams.bReturnPhysicalMaterial = false;
			//RV_TraceParams.TraceTag = TraceTag;

			//~~ Re-initialize hit info ~~//
			FHitResult RV_Hit(ForceInit);

			FVector WorldLocation;
			FVector WorldDirection;
			PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

			//FRotator currentCharacterRotation = this->GetActorRotation();
			//FRotator targetRotation = mouseDirection.Rotation();

			//WorldLocation = WorldLocation + LandscapeLocation;

			FVector LineTraceFrom = WorldLocation + FVector{ 1.f, 1.f, 0.f };
			FVector LineTraceTo = WorldDirection * 50000 + WorldLocation + FVector{ 1.f, 1.f, 0.f };

			PlayerController->GetWorld()->LineTraceSingleByChannel(RV_Hit, LineTraceFrom, LineTraceTo, ChannelLandscape, RV_TraceParams);
			if (RV_Hit.bBlockingHit)
			{
				// Point->Location = RV_Hit.Location;
				Hex = LocationToHex(RV_Hit.Location);
			}
		}
	}
	return Hex;
}


/******************** LocationToHex *************************/
UHexTile* UPOTLGameInstance::LocationToHex(FVector Location)
{
	UHexTile* Hex = nullptr;
	if (Landscape)
	{
		FVector Cube = UPOTLUtilFunctionLibrary::LocationToCube(GridXCount, HexWidth, HexHeight, Location - Landscape->GetActorLocation());
		int32 HexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(UPOTLUtilFunctionLibrary::ConvertCubeToOffset(Cube), GridXCount);
		if (Hexes.IsValidIndex(HexIndex)) {
			Hex = Hexes[HexIndex];
		}
	}
	return Hex;
}


/*****************************************************************************************************/
/**************************************** DEBUG - LOG ************************************************/
/*****************************************************************************************************/

/******************** Log *************************/
void UPOTLGameInstance::Log(FString Msg = "", float Duration = 5.0f, FColor DebugColor = FColor::Green, int32 GroupIndex = -1)
{
	GEngine->AddOnScreenDebugMessage(GroupIndex, Duration, DebugColor, Msg);
}




/*****************************************************************************************************/
/**************************************** DELEGATES **************************************************/
/*****************************************************************************************************/

//OnTurnSwitched.AddDynamic(this);
// Broadcast Delegate 
//OnTurnSwitched.Broadcast(CurrentHouer, CurrentMinute, CurrentSeconds);


//Log("Structure->BroadcastRange: " + FString::FromInt(Structure->BroadcastRange), 15.0f, FColor::Yellow, 3);

//Log("k: " + FString::FromInt(k) + "/" + FString::FromInt(m), 15.0f, FColor::Yellow, -1);
//Log("hex cube: " + Hex->HexCubeCoords.ToString(), 15.0f, FColor::Yellow, -1);


/*
for (int32 testI = 0; testI < Structure->Hex->HexNeighborIndexes.Num(); testI++)
{
int32 Index = Structure->Hex->HexNeighborIndexes[testI];

//Log("testI: " + FString::FromInt(testI) + "  Index:" + FString::FromInt(Index) + FString::Printf(TEXT("Bool: %s"), (Index != -1 && Hexes.IsValidIndex(Index) ? TEXT("true") : TEXT("false"))), 15.0f, FColor::Green, -1);

if (Index != -1 && Hexes.IsValidIndex(Index))
{
FST_Hex& NeighborHex = Hexes[Index];

// Make Construct Location
FST_ConstructLocation ConstructLocation;
ConstructLocation.Cube = NeighborHex->HexCubeCoords;
ConstructLocation.Hex = NeighborHex;
ConstructLocation.EmitTo.Add(NeighborHex);
ConstructLocations.Add(ConstructLocation);
}
}
Log("ConstructLocations.Num(): " + FString::FromInt(ConstructLocations.Num()), 15.0f, FColor::Green, 4);
return ConstructLocations;
*/