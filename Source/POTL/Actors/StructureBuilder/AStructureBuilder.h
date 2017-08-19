// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "POTLDataHolder.h"
#include "UObjects/UHexTile.h"
#include "AStructureBuilder.generated.h"



UCLASS()
class POTL_API AStructureBuilder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStructureBuilder();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Builder") //EditDefaultsOnly 
	TAssetPtr<UStaticMesh> DefaultMesh;

	UFUNCTION(BlueprintCallable, Category = "Builder")
	int Rotate(int Direction);
	 
	UFUNCTION(BlueprintCallable, Category = "Builder")
	void SetData(FST_Structure Data);

	void SetRootHex(UHexTile* Hex);
	//void SetLocation();
	void Build();

	void Hide();
	void Show();

	bool ValidatePlacement();

private:
	UHexTile* RootHex;
	UHexTile* AttachToHex;
	int Rotation;
	bool bIsBuildValid;
	FST_Structure StructureBaseData;
	UStaticMeshComponent* Mesh;
	// Build validation parameters
	// bIsRequiredResourcesPresent
	// bIsAttachmentValid


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
