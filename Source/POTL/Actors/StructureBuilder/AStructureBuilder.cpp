// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLDataHolder.h"
#include "UObjects/UHexTile.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "POTLUtilFunctionLibrary.h"
#include "AStructureBuilder.h"


// Sets default values
AStructureBuilder::AStructureBuilder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsBuildValid = false;
	Rotation = 0;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>BaseMeshObj(TEXT("StaticMesh'/Game/Meshes/SM_HexCylinder.SM_HexCylinder'")); 
	if (BaseMeshObj.Succeeded())
	{
		//DefaultMesh = &BaseMeshObj;
		Mesh->SetStaticMesh(BaseMeshObj.Object);
		Mesh->SetupAttachment(RootComponent);
		Mesh->SetCastShadow(false);
		Mesh->SetMobility(EComponentMobility::Movable);

		// Builder material
		static ConstructorHelpers::FObjectFinder<UMaterial>MaterialObj(TEXT("Material'/Game/Materials/StructureBuilder/M_StructureBuilder.M_StructureBuilder'"));
		if (MaterialObj.Succeeded())
		{
			if (Mesh)
			{
				DynamicMaterial = UMaterialInstanceDynamic::Create(MaterialObj.Object, nullptr);
				Mesh->SetMaterial(0, DynamicMaterial);
			}
		}
	}
	
}


int AStructureBuilder::Rotate(int Direction)
{
	Rotation = (Rotation + 1) % 6;

	SetActorRotation(FRotator(0, Rotation * (360 / 6), 0));

	TraceHexes();

	return Rotation;
}

void AStructureBuilder::SetData(FST_Structure Data)
{
	StructureBaseData = Data;
	Mesh->SetStaticMesh(StructureBaseData.PreviewMesh.Get());
}

void AStructureBuilder::SetRootHex(UHexTile* Hex)
{
	if (Hex)
	{
		//FHitResult* HitResult = FHitResult();
		FHitResult HitResult(ForceInit);
		SetActorLocation(Hex->Location, false, &HitResult, ETeleportType::TeleportPhysics);
		RootHex = Hex;

		TraceHexes();

		/*
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
		*/


	}
}
void AStructureBuilder::Build()
{
	if (bIsBuildValid && RootHex)
	{
		UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(GetGameInstance());
		if (GameInstance)
		{
			GameInstance->PlantStructure(RootHex->HexCubeCoords, Rotation, StructureBaseData.Id, nullptr, true, false); //!! Might not be correct way to do it !!//
		}
	}
	else
	{
		// Send return message. Build not valid (EReason/ReasonTranslation(NoValidAttach/NoRequiredResources))
	}
}
void AStructureBuilder::TraceHexes()
{
	// Set attachTo Hex
	FVector RotatedEntranceCubeCoord = StructureBaseData.Entrance + UPOTLUtilFunctionLibrary::DirectionToCube(Rotation);
	AttachToHex = RootHex->GetNeighbourByOffset(RotatedEntranceCubeCoord);

	// Get all tiles on
	TilesOn.Empty();
	for (int32 i = 0; i < StructureBaseData.CubeSizes.Num(); i++)
	{
		FVector CubeCoord = StructureBaseData.CubeSizes[i];
		FVector RotatedCubeCoord = UPOTLUtilFunctionLibrary::RotateCube(CubeCoord, Rotation, FVector(0, 0, 0));
		UHexTile* OffsetHex = RootHex->GetNeighbourByOffset(RotatedCubeCoord);
		TilesOn.Add(OffsetHex);
		if (OffsetHex)
		{
			DrawDebugString(GetWorld(), OffsetHex->Location, "Tile", nullptr, FColor::Blue, 0.2, true);
		}
	}

	bIsBuildValid = ValidatePlacement();

	if (bIsBuildValid)
	{
		if (DynamicMaterial) {
			DynamicMaterial->SetVectorParameterValue("Color", FLinearColor::Green);
		}
	}
	else
	{
		if (DynamicMaterial) {
			DynamicMaterial->SetVectorParameterValue("Color", FLinearColor::Red);
		}
	}
}
bool AStructureBuilder::ValidatePlacement()
{
	if (RootHex)
	{
		// Validate AttachTo
		if (StructureBaseData.AttachTo.Num() > 0)
		{
			if (AttachToHex && AttachToHex->AttachedBuilding)
			{
				if (!StructureBaseData.AttachTo.Contains(AttachToHex->AttachedBuilding->StructureBaseData.Id))
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		// Validate require Resource on all tiles on
		if (StructureBaseData.ConstructionCost.Num() > 0)
		{
			for (auto& Entry : StructureBaseData.ConstructionCost)
			{
				for (auto& Hex : TilesOn)
				{
					if (!Hex || !Hex->Resources.Contains(Entry.Id) || Hex->Resources[Entry.Id] < Entry.Amount)
					{
						return false;
					}
				}
			}
		}
		return true;
	}
	return false;
}
void AStructureBuilder::Show()
{
	SetActorHiddenInGame(false);
}
void AStructureBuilder::Hide()
{
	SetActorHiddenInGame(true);
}

// Called when the game starts or when spawned
void AStructureBuilder::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStructureBuilder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

