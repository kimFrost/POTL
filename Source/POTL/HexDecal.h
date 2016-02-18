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



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
	UMaterialInstanceDynamic* DynamicMaterial;
	
	// Material

	// Mesh
	// UPrimitiveComponent

	/*** FUNCTIONS **/

	UFUNCTION(BlueprintCallable, Category = "Power")
	void ChangeMaterial(EHighlightType Type);


	//MaterialInstance = UMaterialInstanceDynamic::Create(Material, this);

	//UMaterialInstanceDynamic* DynamicMaterial = GetRootPrimitiveComponent()->CreateDynamicMaterialInstance(0, GetMesh()->GetMaterial(0));
	//DynamicMaterial->SetVectorParameterValue("BodyColor", FLinearColor::Black);


	//~~ BeginPlay ~~//
	virtual void BeginPlay() override;


};
