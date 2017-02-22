// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DecalActor.h"
#include "POTLDataHolder.h"
#include "HexDecal.generated.h"

/**
 * 
 */
UCLASS()
class POTL_API AHexDecal : public ADecalActor
{
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties
	AHexDecal(const FObjectInitializer &ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Decal)
	UMaterialInterface* Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
	UMaterialInstanceDynamic* DynamicMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
	EHighlightType DecalType;

	// Material

	// Mesh
	// UPrimitiveComponent

	/*** FUNCTIONS **/

	UFUNCTION(BlueprintCallable, Category = "Decal")
	void HideDecal();

	UFUNCTION(BlueprintCallable, Category = "Decal")
	void ChangeMaterial(EDecalType Type);


	//MaterialInstance = UMaterialInstanceDynamic::Create(Material, this);

	//UMaterialInstanceDynamic* DynamicMaterial = GetRootPrimitiveComponent()->CreateDynamicMaterialInstance(0, GetMesh()->GetMaterial(0));
	//DynamicMaterial->SetVectorParameterValue("BodyColor", FLinearColor::Black);


	//~~ BeginPlay ~~//
	virtual void BeginPlay() override;


};
