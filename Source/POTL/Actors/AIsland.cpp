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
		InstTreeMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		//InstTreeMesh->SetCollisionResponseToChannel()
		//InstTreeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
		float MaxOffsetDistance = 100.f;

		Density = Density * Density;

		for (int i = 0; i < Density; i++)
		{
			FVector HexLocation = OnHex->Location;
			FVector Offset = FVector(
				FMath::RandRange(-MaxOffsetDistance, MaxOffsetDistance), 
				FMath::RandRange(-MaxOffsetDistance, MaxOffsetDistance), 
				FMath::RandRange(-20, 0)
			);

			//  Y, Z, X
			FQuat Rotation = FRotator(
				FMath::RandRange(-20, 20),
				FMath::RandRange(0, 360),
				FMath::RandRange(-20, 20)
			).Quaternion();
			Rotation.Normalize();

			FTransform ForestTransform;
			ForestTransform.SetLocation(HexLocation + Offset);
			ForestTransform.SetRotation(Rotation);
			//ForestTransform.SetScale3D(FVector(1, 1, 1));
			int _InstanceId = InstTreeMesh->AddInstance(ForestTransform);
		}
	}
}
void AIsland::SpawnHexRange(FVector Location, const TArray<UHexTile*>& Hexes)
{
	OnSpawnHexRange(Location, Hexes);
}
void AIsland::InitializeIsland()
{
	UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		for (auto& Hex : GameInstance->Hexes)
		{
			if (Hex && Hex->HexResourceInfo.HasTrees)
			{
				SpawnForest(Hex, Hex->HexResourceInfo.ForestDepth);
			}
		}
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

		GameInstance->OnMapReady.AddDynamic(this, &AIsland::InitializeIsland);

		GameInstance->InitializeWorld();
	}
}
void AIsland::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Native events

void AIsland::OnSpawnHexRange_Implementation(FVector Location, const TArray<UHexTile*>& Hexes)
{

}

