// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DecalActor.h"
#include "HexDecal.generated.h"

/**
 * 
 */
UCLASS()
class POTL_API AHexDecal : public ADecalActor
{
	GENERATED_BODY()

public:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
	UMaterialInstanceDynamic* MaterialInstanceTest;
	
	UFUNCTION(BlueprintCallable, Category = "Power")
	void ChangeMaterial();


	//MaterialInstance = UMaterialInstanceDynamic::Create(Material, this);

	//UMaterialInstanceDynamic* DynamicMaterial = GetRootPrimitiveComponent()->CreateDynamicMaterialInstance(0, GetMesh()->GetMaterial(0));
	//DynamicMaterial->SetVectorParameterValue("BodyColor", FLinearColor::Black);

};
