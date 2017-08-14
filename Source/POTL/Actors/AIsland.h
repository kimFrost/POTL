// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "UObjects/UHexTile.h"
#include "AIsland.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class UTileManager;
class UOverlayManager;


UCLASS()
class POTL_API AIsland : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	AIsland(const FObjectInitializer &ObjectInitializer);

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
	//TSubclassOf<class UStaticMesh> TreeMeshClass;
	////TSubobjectPtr<UStaticMeshComponent> TreeMeshClass;
	////TSubclassOf<UStaticMeshComponent> TreeMeshClass;

	//StaticMesh'/Game/Meshes/Foliage/_SM_Tree01._SM_Tree01'

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
	UStaticMesh* TreeStaticMesh;

	UInstancedStaticMeshComponent* InstTreeMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
	TEnumAsByte<ECollisionChannel> ChannelLandscape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
	TEnumAsByte<ECollisionChannel> ChannelFoliage;


	UTileManager* TileManager;

	UOverlayManager* OverlayManager;


	void CreateAssets();

	void SpawnForest(UHexTile* OnHex, int Density);

	void SpawnHexRange(FVector Location, const TArray<UHexTile*>& Hexes);

	UFUNCTION(Category = "World")
	void InitializeIsland();


	UFUNCTION(BlueprintNativeEvent, Category = "World")
	void OnSpawnHexRange(FVector Location, const TArray<UHexTile*>& Hexes);

	UFUNCTION(BlueprintNativeEvent, Category = "World")
	void OnDrawProviders(const TArray<UHexTile*>& Hexes);


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
};
