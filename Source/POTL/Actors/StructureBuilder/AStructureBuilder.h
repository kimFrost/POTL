// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "POTLDataHolder.h"
#include "UObjects/UHexTile.h"
#include "Engine/StreamableManager.h"
#include "AStructureBuilder.generated.h"


class UArrowComponent;
class ARangeDecal;

UCLASS()
class POTL_API AStructureBuilder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStructureBuilder();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Builder") //EditDefaultsOnly 
	UStaticMesh* DefaultMesh;
	//TAssetPtr<UStaticMesh> DefaultMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Builder")
	UArrowComponent* Arrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Builder")
	ARangeDecal* RangeDecal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Builder")
	TSubclassOf<class ARangeDecal> RangeDecalBPClass;


	UFUNCTION(BlueprintCallable, Category = "Builder")
	int Rotate(int Direction);
	 
	UFUNCTION(BlueprintCallable, Category = "Builder")
	void SetData(FST_Structure Data);

	void SetRootHex(UHexTile* Hex);
	//void SetLocation();
	void Build();

	void TraceHexes();
	bool ValidatePlacement();

	void Hide();
	void Show();

private:
	UHexTile* RootHex;
	UHexTile* AttachToHex;
	TArray<UHexTile*> TilesOn;
	int Rotation;
	bool bIsBuildValid;
	FST_Structure StructureBaseData;
	UStaticMeshComponent* Mesh;
	UMaterialInstanceDynamic* DynamicMaterial;
	FStreamableManager AssetLoader;


	// Build validation parameters
	// bIsRequiredResourcesPresent
	// bIsAttachmentValid

	UStaticMesh* LoadMesh(TAssetPtr<UStaticMesh> MeshAssetID);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
