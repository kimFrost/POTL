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
		float MaxOffsetDistance = 100.f;
		for (int i = 0; i < Density; i++)
		{
			FVector HexLocation = OnHex->Location;
			FVector Offset = FVector(
				FMath::RandRange(-MaxOffsetDistance, MaxOffsetDistance), 
				FMath::RandRange(-MaxOffsetDistance, MaxOffsetDistance), 
				FMath::RandRange(-20, 0)
			);




			FQuat Rotation = FQuat();
			//Rotation.X = FMath::RandRange(-20.f, 20.f);
			Rotation.X = 180.f;
			//Rotation.Y = FMath::RandRange(-20.f, 20.f);
			Rotation.Y = 180.f;
			Rotation.Z = 180.f;
			//Rotation.Z = FMath::RandRange(0.f, 360.f);
			Rotation.Normalize();

			/*
			FQuat Rotation = FQuat(
				FMath::RandRange(-20, 20),
				FMath::RandRange(-20, 20),
				FMath::RandRange(0, 360),
				0
			);
			*/

			Rotation = FRotator(20.f, 5.f, 0.f).Quaternion();
			Rotation.Normalize();

			FTransform ForestTransform;
			ForestTransform.SetLocation(HexLocation + Offset);
			ForestTransform.SetRotation(Rotation);
			//ForestTransform.SetScale3D(FVector(1, 1, 1));
			int _InstanceId = InstTreeMesh->AddInstance(ForestTransform);


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

		GameInstance->InitializeWorld();
	}
}

void AIsland::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

