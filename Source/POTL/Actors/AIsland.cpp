// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLGameInstance.h"
#include "AIsland.h"


// Sets default values
AIsland::AIsland(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*
	TSubobject<UStaticMeshComponent> mesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("/Game/Mesh/Cube/YourMesh"));
	mesh->SetStaticMesh(MeshObj.Object);
	*/

	/*
	CubeMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("CubeMesh"));

	static ConstructorHelpers::FObjectFinder<UBlueprint> CubeBP(TEXT("Blueprint'/Game/BP_Cube.BP_Cube'"));
	if (CubeBP.Object) {
		CubeBlueprint = (UClass*)CubeBP.Object->GeneratedClass;
	}
	}

	*/
}

void AIsland::CreateAssets()
{
	//AIsland* IslandWorld = nullptr;
	if (TreeStaticMesh)
	{
		InstTreeMesh = NewObject<UInstancedStaticMeshComponent>(this);
		InstTreeMesh->RegisterComponent();
		InstTreeMesh->SetStaticMesh(TreeStaticMesh);
		InstTreeMesh->SetFlags(RF_Transactional);
		//InstTreeMesh->SetWorldLocation(OnHex->Location);
		this->AddInstanceComponent(InstTreeMesh);
	}
}

void AIsland::SpawnForest(UHexTile* OnHex, int Density)
{
	if (OnHex && InstTreeMesh)
	{
		InstTreeMesh->AddInstance(FTransform(OnHex->Location));
	}
}

void AIsland::BeginPlay()
{
	Super::BeginPlay();

	CreateAssets();

	UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->WorldActor = this;
		GameInstance->ChannelLandscape = ChannelLandscape;
		GameInstance->ChannelFoliage = ChannelFoliage;

		GameInstance->InitializeWorld();
	}
}

void AIsland::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

