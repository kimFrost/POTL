// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLDataHolder.h"
#include "HexDecal.h"


// Sets default values
AHexDecal::AHexDecal(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
	//DynamicMaterial = CreateDynamicMaterialInstance();
	// DynamicMaterial = Decal(target) --> Create Dynamic material instance
	DynamicMaterial = Decal->CreateDynamicMaterialInstance();
	//DynamicMaterial->SetVectorParameterValue("ParamColor", FLinearColor::Black);
	//BoxComponent->SetBoxExtent(FVector(20,10,5), false);
	// Scale 500, 142, 124
	// Rotation 0, -90, 0
}



//CreateDynamicMaterialInstance
//auto dynamic = toAdd->CreateDynamicMaterialInstance(); 
//dynamic->SetScalarParameterValue(TEXT("Rotator"), FRandomStream().RandHelper(360)); 
//dynamic->SetVectorParameterValue("Color", FLinearColor(0, 0, 1)); 
//toAdd->SetMaterial(0, dynamic);


/******************** ChangeType *************************/
void AHexDecal::ChangeMaterial(EHighlightType Type)
{
	if (Type == EHighlightType::Blue)
	{
		//DynamicMaterial->SetScalarParameterValue();
		DynamicMaterial->SetVectorParameterValue("BodyColor", FLinearColor::Blue);
	}
	else if (Type == EHighlightType::Green)
	{
		DynamicMaterial->SetVectorParameterValue("BodyColor", FLinearColor::Green);
	}
	else if (Type == EHighlightType::Red)
	{
		DynamicMaterial->SetVectorParameterValue("BodyColor", FLinearColor::Red);
	}
}


/******************** BeginPlay *************************/
void AHexDecal::BeginPlay()
{
	Super::BeginPlay();

}