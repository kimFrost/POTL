// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLDataHolder.h"
#include "UObjects/UHexTile.h"
#include "AStructureBuilder.h"


// Sets default values
AStructureBuilder::AStructureBuilder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>BaseMeshObj(TEXT("StaticMesh'/Game/Island/Meshes/Tile/tile.tile'"));
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
	Rotation += Direction;

	return Rotation;
}

void AStructureBuilder::SetData(FST_Structure Data)
{
	StructureBaseData = Data;

	//StructureBaseData.Mesh

}

void AStructureBuilder::SetRootHex(UHexTile* Hex)
{
	if (Hex)
	{


		RootHex = Hex;
	}
}

void AStructureBuilder::Build()
{
	if (bIsBuildValid)
	{

	}
	else
	{

	}
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

