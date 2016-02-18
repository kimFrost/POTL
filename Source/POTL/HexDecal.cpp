// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLDataHolder.h"
#include "HexDecal.h"


// Sets default values
AHexDecal::AHexDecal(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{

	
	//static ConstructorHelpers::FObjectFinder<UMaterial>MaterialInstanceObj(TEXT("Material'/Game/Materials/Decals/M_HexDecal.M_HexDecal'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance>MaterialInstanceObj(TEXT("MaterialInstanceConstant'/Game/Materials/Decals/M_HexDecal_Inst.M_HexDecal_Inst'"));
	if (MaterialInstanceObj.Succeeded())
	{
		Material = MaterialInstanceObj.Object;
		GetDecal()->SetMaterial(0, Material);
		DynamicMaterial = GetDecal()->CreateDynamicMaterialInstance();
		//DynamicMaterial->SetVectorParameterValue("ParamColor", FLinearColor::Yellow);
	}
	
	//DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);

	/*
	static ConstructorHelpers::FObjectFinder<UMaterial> Material_BP (TEXT("MaterialInstanceConstant'/Game/Materials/Decals/M_HexDecal_Inst.M_HexDecal_Inst'"));
	//if (Material.Object != NULL)
	if (Material_BP.Succeeded())
	{
		Material = (UMaterial*)Material_BP.Object;
	}

	DynamicMaterial = GetDecal()->CreateDynamicMaterialInstance();
	*/


	//Blueprint'/Game/Blueprint/Actor/Buildings/BP_House.BP_House'
	//"BlueprintGeneratedClass'/Game/Blueprint/Actor/Buildings/BP_House.BP_House_C'"

	//DynamicMaterial = Decal->CreateDynamicMaterialInstance();

	//Decal->SetDecalMaterial();

	//DynamicMaterial = CreateDynamicMaterialInstance();
	// DynamicMaterial = Decal(target) --> Create Dynamic material instance

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
		DynamicMaterial->SetVectorParameterValue("ParamColor", FLinearColor::Blue);
	}
	else if (Type == EHighlightType::Green)
	{
		DynamicMaterial->SetVectorParameterValue("ParamColor", FLinearColor::Green);
	}
	else if (Type == EHighlightType::Red)
	{
		DynamicMaterial->SetVectorParameterValue("ParamColor", FLinearColor::Red);
	}
}


/******************** BeginPlay *************************/
void AHexDecal::BeginPlay()
{
	Super::BeginPlay();

}