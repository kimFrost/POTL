// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "UObjects/UHexTile.h"
#include "AIsland.generated.h"


//~~~~~ Forward Declarations ~~~~~//
class UTileManager;
class UOverlayManager;
class ARangeDecal;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World")
	UTileManager* TileManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
	UOverlayManager* OverlayManager;


	void CreateAssets();

	void SpawnForest(UHexTile* OnHex, int Density);

	ARangeDecal* SpawnHexRange(FVector Location, const TArray<UHexTile*>& Hexes);

	UFUNCTION(Category = "World")
	void InitializeIsland();


	UFUNCTION(BlueprintNativeEvent, Category = "World")
	ARangeDecal* OnSpawnHexRange(FVector Location, const TArray<UHexTile*>& Hexes) const; // Const if return should work in BP


	UFUNCTION(BlueprintNativeEvent, Category = "World")
	void OnDrawProviders(const TArray<UHexTile*>& Hexes);


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
};
