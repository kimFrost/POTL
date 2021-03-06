// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLDataHolder.h"
#include "UObjects/UHexPoint.h"
#include "UObjects/UHexTile.h"
#include "UObjects/UResourceMap.h"
#include "UObjects/UStorageMap.h"
#include "UObjects/UTransaction.h"
#include "UObjects/Singletons/UEventSingleton.h"
#include "POTLUtilFunctionLibrary.h"
#include "Actors/AIsland.h"
#include "POTLStructure.h"
#include "Components/UStructureComponent.h"
#include "Components/UStorageComponent.h"
#include "Components/UConstructionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
//#include "Runtime/Foliage/Public/FoliageInstancedStaticMeshComponent.h"
//#include "Components/InstancedStaticMeshComponent.h"
#include "POTLGameInstance.h"



// Sets default values
UPOTLGameInstance::UPOTLGameInstance(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
	HexWidth = 255.0f;
	HexHeight = HexWidth / FMath::Sqrt(3) * 2.f;
	//Landscape = nullptr;
	GridXCount = 200; // Temp. Needs to be calc in point creation.
	GridYCount = 200; // Temp. Needs to be calc in point creation.
	HexGridReady = false;
	ResourceUniqueIdCounter = 0;

	//~~ Table data ~~//
	DATA_Recipes = nullptr;
	DATA_Structures = nullptr;
	DATA_Resources = nullptr;
	DATA_Collections = nullptr;

	//~~ For setting custom channels from blueprint ~~//
	ChannelLandscape = ECollisionChannel::ECC_WorldStatic;
	ChannelFoliage = ECollisionChannel::ECC_WorldStatic;

	CurrentWorld = nullptr;

	//StaticMesh'/Game/Meshes/Foliage/_SM_Tree01._SM_Tree01'
	//Material'/Game/Materials/Folliage/BirchTreeBark/Birch_Tree__Bark_MAT.Birch_Tree__Bark_MAT'
	//Material'/Game/Materials/Folliage/LargeLeafShrub/Large_Leaf_Shrub_MAT.Large_Leaf_Shrub_MAT'

	/*
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CMesh(TEXT("StaticMesh'/Game/sm/box.box'"));
	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("container")); // text("") can be just about anything.
	Mesh->SetStaticMesh(CMesh.Object);
	Mesh->AttachParent = RootComponent;
	*/

}

UEventSingleton* UPOTLGameInstance::GetEventSingleton()
{
	if (!EventSingleton)
	{
		EventSingleton = NewObject<UEventSingleton>();
		//EventSingleton->AddToRoot(); // Prevent Garbage collection // UPROP instead
	}
	return EventSingleton;
}


/******************** DATA *************************/

void UPOTLGameInstance::InitializeWorld()
{
	// Read tables
	ReadTables();
	// set channels

	// trace landscape
	TraceLandscape();
	// create hexes
	CreateHexes();
	// clean hexes
	CleanHexes();
	// enrich hexes
	EnrichHexes();
	// Calc hexes rot
	CalcHexesRot();
	//! trace landscape data (BP)

	InjectTestValuesToHexes();

	// Calc resources density
	CalcHexResourceDensity();
	// Set hex grid ready
	HexGridReady = true;
	// Call OnMapReady
	OnMapReady.Broadcast();
}

void UPOTLGameInstance::ReadTables()
{
	//auto cls = StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("/Game/Wood_Table_Blueprint")); 
	//UObject* Asset = (UObject*)StaticLoadObject(UBlueprint::StaticClass(), nullptr, TEXT("DataTable'/Game/Resources/Structures'"));
	//~~ Structures ~~//
	UDataTable* StructureTable = (UDataTable*)StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("DataTable'/Game/Resources/Structures.Structures'"));
	if (StructureTable)
	{
		DATA_Structures = StructureTable;
		TArray<FName> RowNames;
		RowNames = DATA_Structures->GetRowNames();
		for (auto& Name : RowNames)
		{
			CachedStructureIds.Add(Name.ToString());
		}
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
		TArray<FName> RowNames;
		RowNames = DATA_Resources->GetRowNames();
		for (auto& Name : RowNames)
		{
			CachedResourceIds.Add(Name.ToString());
		}
	}
	//~~ Collections ~~//
	UDataTable* CollectionsTable = (UDataTable*)StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("DataTable'/Game/Resources/DATA_ResourceCollections.DATA_ResourceCollections'"));
	if (CollectionsTable)
	{
		DATA_Collections = CollectionsTable;
		TArray<FName> RowNames;
		RowNames = DATA_Collections->GetRowNames();
		for (auto& Name : RowNames)
		{
			CachedCollectionIds.Add(Name.ToString());
		}
	}
}

/******************** MAP - CONSTRUCTION *************************/

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
bool UPOTLGameInstance::ValidatePlaceStructureOnHex(UHexTile* Hex, const FST_BuildInfo& BuildInfo)
{
	bool Valid = false;
	FST_Structure* StructureData = GetStructureRowData(BuildInfo.StructureInfo.Id);
	if (StructureData && Hex)
	{
		Valid = true;

		// Validate attachTo Structure is present
		if (StructureData->AttachTo.Num() > 0)
		{
			UHexTile* AttachToHex = Hex->GetNeighbourHex(BuildInfo.RotationDirection);
			if (AttachToHex && AttachToHex->AttachedBuilding)
			{
				if (!StructureData->AttachTo.Contains(AttachToHex->AttachedBuilding->StructureBaseData.Id))
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		// Validate all hexes are free
		for (int32 i = 0; i < StructureData->CubeSizes.Num(); i++)
		{
			FVector LocalCubeCoord = StructureData->CubeSizes[i] + Hex->HexCubeCoords;
			LocalCubeCoord = UPOTLUtilFunctionLibrary::RotateCube(LocalCubeCoord, BuildInfo.RotationDirection, Hex->HexCubeCoords);
			FVector2D OffsetCoords = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(LocalCubeCoord);
			int32 HexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(OffsetCoords, GridXCount);
			if (Hexes.IsValidIndex(HexIndex))
			{
				UHexTile* Hex = Hexes[HexIndex];
				if (Hex)
				{	
					if (!Hex->IsBuildable())
					{
						return false;
					}
				}
			}
		}
	}
	return Valid;
}
FST_Structure* UPOTLGameInstance::GetStructureRowData(FString RowName)
{
	if (DATA_Structures)
	{
		static const FString ContextString(TEXT("GENERAL")); //~~ Key value for each column of values ~~//
		return DATA_Structures->FindRow<FST_Structure>(*RowName, ContextString);
	}
	return nullptr;
}
void UPOTLGameInstance::PlantForest(UHexTile* OnHex, int Density)
{

	OnHex = Hexes[FMath::RandRange(0, Hexes.Num() - 1)];

	if (OnHex)
	{
		if (WorldActor)
		{
			WorldActor->SpawnForest(OnHex, Density);
		}

		//AIsland* IslandWorld = nullptr;
		/*
		if (WorldActor && WorldActor->TreeStaticMesh)
		{
			UInstancedStaticMeshComponent* InstTreeMesh = NewObject<UInstancedStaticMeshComponent>(WorldActor);
			InstTreeMesh->RegisterComponent();
			InstTreeMesh->SetStaticMesh(WorldActor->TreeStaticMesh);
			InstTreeMesh->SetFlags(RF_Transactional);
			WorldActor->AddInstanceComponent(InstTreeMesh);

			//InstTreeMesh->SetWorldLocation(OnHex->Location);


			InstTreeMesh->AddInstance(FTransform(OnHex->Location));
		}
		*/

		/*
		TArray<UActorComponent*> currentICs = this->GetInstanceComponents();
		for (UActorComponent* ic : currentICs)
			ic->DestroyComponent();
		*/

	}
}
APOTLStructure* UPOTLGameInstance::PlantPlaceholderStructure(FVector CubeCoord, int32 RotationDirection, FString RowName, APOTLStructure* AttachTo, bool InstaBuild)
{
	APOTLStructure* Structure = nullptr;
	Structure = PlantStructure(CubeCoord, RotationDirection, RowName, AttachTo, InstaBuild, true);
	if (Structure)
	{
		PlaceholderStructures.Add(Structure);
	}
	return Structure;
}
APOTLStructure* UPOTLGameInstance::PlantStructure(FVector CubeCoord, int32 RotationDirection, FString RowName, APOTLStructure* AttachTo, bool InstaBuild, bool IsPlaceholder)
{
	APOTLStructure* Structure = nullptr;
	if (CurrentWorld && DATA_Structures)
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
				Structure = CurrentWorld->SpawnActor<APOTLStructure>(StructureData->StructureClass, SpawnLocation, SpawnRotation, SpawnParams);
				if (Structure)
				{
					//~~ Store cubecoord in structure ~~//
					Structure->CubeCoord = CubeCoord;

					//~~ Store structure raw data ~~//
					Structure->StructureRowName = RowName;
					Structure->StructureBaseData = *StructureData;
					Structure->StructureBaseData.RotationDirection = RotationDirection;
					Structure->IsPlaceholder = IsPlaceholder;
					if (Structure->IsPlaceholder)
					{

					}


					//~~ Store hex in structure ~~// //~~ CubeCoord is the rotation center cube coord ~~//
					FVector2D OffsetCoords = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(CubeCoord);
					int32 HexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(OffsetCoords, GridXCount);
					if (Hexes.IsValidIndex(HexIndex))
					{
						UHexTile* Hex = Hexes[HexIndex];
						Structure->BaseHex = Hex;
						//Structure->HexIndex = HexIndex;
					}

					//~~ Set Structure broadcast root hexindex on structure ~~// //!! Rotate logic is missing, I think ?
					FVector BroadcastCubeCoord = StructureData->Entrance + CubeCoord;
					BroadcastCubeCoord = UPOTLUtilFunctionLibrary::RotateCube(BroadcastCubeCoord, RotationDirection, CubeCoord);
					FVector2D BroadcastOffsetCoords = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(BroadcastCubeCoord);
					int32 BroadcastHexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(BroadcastOffsetCoords, GridXCount);
					if (Hexes.IsValidIndex(BroadcastHexIndex))
					{
						//UHexTile* Hex = Hexes[BroadcastHexIndex];
						//Structure->Hex = Hex;
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
								if (!IsPlaceholder)
								{
									Hex->AttachedBuilding = Structure;
								}
								Structure->OccupiedHexes.Add(Hex);
							}
						}
					}

					//~~ InstaBuild for debugging ~~//
					if (InstaBuild)
					{
						Structure->IsUnderConstruction = false;
					}

					if (AttachTo)
					{
						Structure->AttachedTo = AttachTo;
					}
					else
					{
						UHexTile* AttachToHex = Hex->GetNeighbourHex(RotationDirection);
						if (AttachToHex && AttachToHex->AttachedBuilding)
						{
							if (AttachToHex->AttachedBuilding != Structure)
							{
								Structure->AttachedTo = AttachToHex->AttachedBuilding;
							}
						}
					}

					//~~ Process construction cost and store resources in structure construction component ~~//
					if (Structure->ConstructionComponent)
					{
						//for (auto& CostEntry : StructureData.ConstructionCost)
						//TransferResource();
						//Structure->ConstructionComponent->
					}

					//~~ Process Structure Data internally ~~//
					Structure->Init();
				}
			}
		}
	}
	OnStructurePlanted.Broadcast(Structure);
	return Structure;
}
void UPOTLGameInstance::RemoveStructure(APOTLStructure* Structure)
{
	if (Structure)
	{
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

/******************** MAP - CREATION *************************/

void UPOTLGameInstance::TraceLandscape()
{
	StorageMap = NewObject<UStorageMap>();
	ResourceMap = NewObject<UResourceMap>();

	if (WorldActor)
	{
		CurrentWorld = WorldActor->GetWorld();
	}

	if (CurrentWorld)
	{
		//FVector ActorLocation = Landscape->GetActorLocation();
		FVector ActorLocation = FVector(0.f, 0.f, 0.f);
		// Get player controller at index 0
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(CurrentWorld, 0);
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

					UHexPoint* Point = NewObject<UHexPoint>();
					Point->Location = FVector(X, Y, 0.f);
					Point->Row = Row;
					Point->Column = Column;
					Point->Exits = true;
					Points.Add(Point);

					CurrentWorld->LineTraceSingleByChannel(RV_Hit, LineTraceFrom, LineTraceTo, ChannelLandscape, RV_TraceParams);
					if (RV_Hit.bBlockingHit)
					{
						Point->Location = RV_Hit.Location;
					}
				}
			}
		}
	}
}
void UPOTLGameInstance::CreateHexes()
{
	if (CurrentWorld) {
		// Get player controller at index 0
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(CurrentWorld, 0);

		if (PlayerController)
		{
			FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, PlayerController);
			RV_TraceParams.bTraceComplex = true;
			RV_TraceParams.bTraceAsyncScene = true;
			RV_TraceParams.bReturnPhysicalMaterial = false;

			/*
			const FName TraceTag("TracePointHeight");
			PlayerController->GetWorld()->DebugDrawTraceTag = TraceTag;
			RV_TraceParams.TraceTag = TraceTag;
			*/

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
					CurrentWorld->LineTraceSingleByChannel(RV_Hit, LineTraceFrom, LineTraceTo, ChannelLandscape, RV_TraceParams);
					
					UHexTile* Hex = NewObject<UHexTile>();
					Hex->Location = FVector(LineTraceFrom.X, LineTraceFrom.Y, 0.f);
					Hex->HexOffsetCoords = FVector2D{ (float)FMath::FloorToInt((float)Point->Column / 2), (float)FMath::FloorToInt((float)Point->Row) };
					Hex->HexCubeCoords = UPOTLUtilFunctionLibrary::ConvertOffsetToCube(Hex->HexOffsetCoords);

					if (RV_Hit.bBlockingHit)
					{
						Hex->Location = RV_Hit.Location;
					}

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

					Hex->WorldRef = CurrentWorld;
					Hex->Init();

					Hexes.Add(Hex);
				}
			}
		}
	}
}
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
			CubeDirections.Add({ 1, 0, -1 });
			CubeDirections.Add({ 1, -1, 0 });
			CubeDirections.Add({ 0, -1, 1 });
			CubeDirections.Add({ -1, 0, 1 });
			CubeDirections.Add({ -1, 1, 0 });
			CubeDirections.Add({ 0, 1, -1 });

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
					Hex->HexNeighbors[ii] = Hexes[HexDirectionIndex];
				}
			}
		}
	}
}
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
void UPOTLGameInstance::AnalyseLandscape()
{
	//!! NOT BEING USED. IS BEING TRACED IN BLUEPRINT AT THE MOMENT !!//
	//UGameplayStatics::
	if (CurrentWorld)
	{
		//FVector ActorLocation = Landscape->GetActorLocation();
		FVector ActorLocation = FVector(0.f, 0.f, 0.f);
		// Get player controller at index 0
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(CurrentWorld, 0);
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

					CurrentWorld->OverlapMultiByChannel(OutHits, Hex->Location, FQuat::Identity, ChannelFoliage, FCollisionShape::MakeSphere(666), RV_TraceParams);
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

					CurrentWorld->LineTraceSingleByChannel(RV_Hit, LineTraceFrom, LineTraceTo, ChannelLandscape, RV_TraceParams);

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
			if (IsValid(Hex) && Hex->HexResourceInfo.HasTrees)
			{
				Hex->Resources.Add("Wood", 1);
				bool Surrounded = true;
				for (int32 ii = 0; ii < Hex->HexNeighborIndexes.Num(); ii++)
				{
					int32 HexNeighborIndex = Hex->HexNeighborIndexes[ii];
					if (Hexes.IsValidIndex(HexNeighborIndex))
					{
						UHexTile* NeighborHex = Hexes[HexNeighborIndex];
						if (IsValid(NeighborHex) && !NeighborHex->HexResourceInfo.HasTrees || NeighborHex->HexResourceInfo.ForestDepth < DepthIndex)
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
					Hex->HexResourceInfo.ForestDepth++;
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
			if (IsValid(Hex) && Hex->HexResourceInfo.HasLake)
			{
				Hex->Resources.Add("Fish", 1);
				bool Surrounded = true;
				for (int32 ii = 0; ii < Hex->HexNeighborIndexes.Num(); ii++)
				{
					int32 HexNeighborIndex = Hex->HexNeighborIndexes[ii];
					if (Hexes.IsValidIndex(HexNeighborIndex))
					{
						UHexTile* NeighborHex = Hexes[HexNeighborIndex];
						if (IsValid(NeighborHex) && !NeighborHex->HexResourceInfo.HasLake || NeighborHex->HexResourceInfo.LakeDepth < DepthIndex)
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
					Hex->HexResourceInfo.LakeDepth++;
				}
			}
		}
	}
	// Clean hexes with forest depth of 1
	for (auto& Hex : Hexes)
	{
		if (Hex)
		{
			if (Hex->HexResourceInfo.ForestDepth == 1)
			{
				Hex->HexResourceInfo.ForestDepth = 0;
				Hex->HexResourceInfo.HasTrees = false;
				Hex->HexTileType = "Grassland";
			}
			if (Hex->HexResourceInfo.ForestDepth > 1)
			{
				Hex->HexTileType = "Woodland";
			}
		}
	}
}

void UPOTLGameInstance::InjectTestValuesToHexes()
{
	//FRandomStream Stream = FRandomStream(451);
	//Stream.GenerateNewSeed();
	for (auto& Hex : Hexes)
	{
		if (Hex)
		{
			//bool HasTress = (Stream.FRand() > 0.2f);
			bool HasTress = (FRandomStream(Hex->HexIndex * 256).FRand() > 0.2f);
			if (HasTress)
			{
				Hex->HexResourceInfo.HasTrees = HasTress;
				Hex->HexResourceInfo.ForestDepth = 1;
				//Hex->HexTileType = "Woodland";
			}
		}
	}
}

/******************** MAP - Resources *************************/

void UPOTLGameInstance::IncludeStorage(UStorageComponent* StorageComp)
{
	if (StorageMap)
	{
		StorageMap->IncludeStorage(StorageComp);
	}
}
UResource* UPOTLGameInstance::CreateResource(FString Id)
{
	if (DATA_Resources)
	{
		if (CachedResourceIds.Contains(Id))
		{
			static const FString ContextString(TEXT("ResourceLookup")); //~~ Key value for each column of values ~~//
			FST_Resource* ResourceData = DATA_Resources->FindRow<FST_Resource>(*Id, ContextString, false);
			if (ResourceData)
			{
				ResourceUniqueIdCounter++;
				//Structure->StructureBaseData = *StructureData;
				UResource* Resource = NewObject<UResource>();
				//UResource* Resource = NewNamedObject<UResource>(this, FName(*("Resource_" + ResourceId + FString::FromInt(ResourceUniqueIdCounter))), RF_NoFlags, nullptr); // Crashes on cleanup on PIE close

				Resource->ResourceId = Id;
				Resource->Tags = ResourceData->Tags;
				Resource->Value = ResourceData->Value;
				Resource->AddToRoot(); // Prevent Garbage collection

				//const TCHAR* ResourceName = *(Resource->GetFName().ToString() + ResourceId);
				//Resource->Rename(ResourceName, this, RF_NoFlags);

				return Resource;
			}
		}
		// Resource Collection lookup
		if (DATA_Collections && CachedCollectionIds.Contains(Id))
		{
			static const FString ContextString(TEXT("CollectionLookup")); //~~ Key value for each column of values ~~//
			FST_Collection* CollectionData = DATA_Resources->FindRow<FST_Collection>(*Id, ContextString, false);
			if (CollectionData)
			{
				int Min = 0;
				int Max = CollectionData->Resources.Num() - 1;
				int RandIndex = Min + (rand() % (int)(Max - Min + 1));
				FString RandId = CollectionData->Resources[RandIndex];
				return CreateResource(RandId);
			}
		}
		//~~ Search by tag ~~//
		return CreateResourceByTag(Id);
	}
	return nullptr;
}
UResource* UPOTLGameInstance::CreateResourceByTag(FString TagId)
{
	if (DATA_Resources)
	{
		static const FString ContextString(TEXT("ResourceTagLookup"));
		TArray<FST_Resource*> ResourceRows;
		DATA_Resources->GetAllRows(ContextString, ResourceRows);
		//~~ Shuffle rows ~~//
		ResourceRows.Sort([this](const FST_Resource Item1, const FST_Resource Item2) {
			return FMath::FRand() < 0.5f;
		});
		for (auto& Row : ResourceRows)
		{
			if (Row)
			{
				if (Row->Tags.Contains(TagId))
				{
					return CreateResource(Row->Id);
				}
			}
		}
	}
	return nullptr;
}
UResource* UPOTLGameInstance::RequestResource(APOTLStructure* Requester, FString ResourceId)
{
	if (StorageMap)
	{
		return StorageMap->RequestResource(Requester, ResourceId);
	}

	return nullptr;
}
UResource* UPOTLGameInstance::RequestResourceByTag(APOTLStructure* Requester, FString Tag)
{
	if (StorageMap)
	{
		return StorageMap->RequestResourceByTag(Requester, Tag);
	}
	return nullptr;
}
void UPOTLGameInstance::TransferResource(UResource* Resource, UStructureComponent* ToComp, bool Consume = false, bool IsFree = false)
{
	if (Resource)
	{
		if (ToComp)
		{
			if (Consume) {
				// Trigger transaction result in wealth/benifit
				APOTLStructure* ResourceOwner = Resource->GetOwner();
				if (ResourceOwner && ToComp->ParentStructure)
				{
					ToComp->ParentStructure->SubtractWealth(1.f);
					ResourceOwner->AddWealth(1.f);
				}
				UResourceTransaction* Transaction = NewObject<UResourceTransaction>();
				if (Transaction)
				{
					Transaction->Seller = Resource->GetOwner();
					Transaction->Buyer = ToComp->ParentStructure;
					Transaction->Resource = Resource;
					Transaction->bConsume = Consume;
					OnTransaction.Broadcast(Transaction);
				}
				Resource->Consume();
			}
			else {
				UStorageComponent* StorageComp = Cast<UStorageComponent>(ToComp);
				if (StorageComp)
				{
					// Trigger transaction result in wealth/benifit
					APOTLStructure* ResourceOwner = Resource->GetOwner();
					if (ResourceOwner && ToComp->ParentStructure)
					{
						ToComp->ParentStructure->SubtractWealth(1.f);
						ResourceOwner->AddWealth(1.f);
					}
					UResourceTransaction* Transaction = NewObject<UResourceTransaction>();
					if (Transaction)
					{
						Transaction->Seller = Resource->GetOwner();
						Transaction->Buyer = ToComp->ParentStructure;
						Transaction->Resource = Resource;
						Transaction->bConsume = Consume;
						bool Transfered = Resource->Transfer(StorageComp);
						if (Transfered)
						{
							OnTransaction.Broadcast(Transaction);
						}
					}
				}
			}
		}
		else if (Consume) {
			Resource->Consume();
			// No transaction if there is no end target
		}
	}
}

/******************** UTIL - Hex *************************/
UHexTile* UPOTLGameInstance::MouseToHex()
{
	UHexTile* Hex = nullptr;
	if (CurrentWorld)
	{
		//FVector LandscapeLocation = Landscape->GetActorLocation();
		FVector LandscapeLocation = FVector(0.f, 0.f, 0.f);
		// Get player controller at index 0
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(CurrentWorld, 0);
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

			CurrentWorld->LineTraceSingleByChannel(RV_Hit, LineTraceFrom, LineTraceTo, ChannelLandscape, RV_TraceParams);
			if (RV_Hit.bBlockingHit)
			{
				Hex = LocationToHex(RV_Hit.Location);
			}
			else
			{
				// Intersect at Z=0 plane
				/*
				float T; // It appears to be the distance from the start point to the point of intersection, as a fraction of the total line length. If you multiply this by the total length of the line passed to the Line Plane Intersection function, it give the actual distance between the points.
				FVector Intersection;
				FPlane IntersectPlane = FPlane(FVector(), FVector());
				if (UKismetMathLibrary::LinePlaneIntersection(LineTraceFrom, LineTraceTo, IntersectPlane, T, Intersection))
				{

				}
				*/
				FVector Normal = FVector(0, 0, 1);
				FPlane Plane = FPlane(Normal, Normal);
				FVector IntersectionLocation = FMath::LinePlaneIntersection(LineTraceFrom, LineTraceTo, FVector::ZeroVector, Normal);
				Hex = LocationToHex(IntersectionLocation);
			}
		}
	}
	return Hex;
}
UHexTile* UPOTLGameInstance::LocationToHex(FVector Location)
{
	UHexTile* Hex = nullptr;

	//FVector Cube = UPOTLUtilFunctionLibrary::LocationToCube(GridXCount, HexWidth, HexHeight, Location - Landscape->GetActorLocation());
	FVector Cube = UPOTLUtilFunctionLibrary::LocationToCube(GridXCount, HexWidth, HexHeight, Location - FVector(0.f, 0.f, 0.f));
	int32 HexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(UPOTLUtilFunctionLibrary::ConvertCubeToOffset(Cube), GridXCount);
	if (Hexes.IsValidIndex(HexIndex)) {
		Hex = Hexes[HexIndex];
	}

	return Hex;
}

UHexTile* UPOTLGameInstance::CubeCoordToHex(FVector CubeCoord)
{
	FVector2D OffsetCoords = UPOTLUtilFunctionLibrary::ConvertCubeToOffset(CubeCoord);
	int32 HexIndex = UPOTLUtilFunctionLibrary::GetHexIndex(OffsetCoords, GridXCount);
	if (Hexes.IsValidIndex(HexIndex))
	{
		return Hexes[HexIndex];
	}
	return nullptr;
}


void UPOTLGameInstance::ShowFeedbackMsg(FString Message, EMessageType Type = EMessageType::Common, FVector WorldLocation = FVector(0))
{
	OnMessageDelegate.Broadcast(Message, Type, WorldLocation);
}

/******************** DEBUG - LOG *************************/
void UPOTLGameInstance::Log(FString Msg = "", float Duration = 5.0f, FColor DebugColor = FColor::Green, int32 GroupIndex = -1)
{
	GEngine->AddOnScreenDebugMessage(GroupIndex, Duration, DebugColor, Msg);
}

