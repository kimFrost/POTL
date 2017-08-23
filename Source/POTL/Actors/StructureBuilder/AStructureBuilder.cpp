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
	static ConstructorHelpers::FObjectFinder<UStaticMesh>BaseMeshObj(TEXT("StaticMesh'/Game/Meshes/SM_Building_House.SM_Building_House'")); 
	if (BaseMeshObj.Succeeded())
	{
		Mesh->SetStaticMesh(BaseMeshObj.Object);
		Mesh->SetupAttachment(RootComponent);
		Mesh->SetCastShadow(false);
		static ConstructorHelpers::FObjectFinder<UMaterial>MaterialObj(TEXT("Material'/Game/Island/Materials/Indicator/M_Indicator.M_Indicator'"));
		if (MaterialObj.Succeeded())
		{
			Mesh->SetMaterial(0, MaterialObj.Object);

		}
	}
}


int AStructureBuilder::Rotate(int Direction)
{
	Rotation = (Rotation + 1) % 6;

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
		// Set attachTo Hex
		if (StructureBaseData.AttachTo.Num() > 0)
		{

		}


		UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(GetGameInstance());
		if (GameInstance)
		{

		}

		TilesOn.Empty();
		// Rotate cubecoords around 
		for (int32 i = 0; i < StructureBaseData.CubeSizes.Num(); i++)
		{
			FVector CubeCoord = StructureBaseData.CubeSizes[i];// +RootHex->HexCubeCoords;
			//FVector RotatedCubeCoord = UPOTLUtilFunctionLibrary::RotateCube(CubeCoord, Rotation, Hex->HexCubeCoords);
			FVector RotatedCubeCoord = UPOTLUtilFunctionLibrary::RotateCube(CubeCoord, Rotation, FVector(0, 0, 0));
			UHexTile* OffsetHex = RootHex->GetNeighbourByOffset(RotatedCubeCoord);
			TilesOn.Add(OffsetHex);
			if (OffsetHex)
			{
				DrawDebugString(GetWorld(), OffsetHex->Location, "Tile", nullptr, FColor::Blue, 0.5, true);
			}
		}

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


		bIsBuildValid = ValidatePlacement();
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

void AStructureBuilder::Show()
{
	SetActorHiddenInGame(false);
}
void AStructureBuilder::Hide()
{
	SetActorHiddenInGame(true);
}

bool AStructureBuilder::ValidatePlacement()
{
	if (RootHex)
	{
		// Validate AttachTo
		if (StructureBaseData.AttachTo.Num() > 0)
		{


		}
		// Validate require Resource on roothex / or all hexes??
		if (StructureBaseData.ConstructionCost.Num() > 0)
		{

		}
	}
	return true;
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

