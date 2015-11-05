// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "Kismet/GameplayStatics.h"
#include "POTLHUD.h"



// Sets default values
APOTLHUD::APOTLHUD(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{

}


/******************** GetNearestStructure *************************/
APOTLStructure* APOTLHUD::GetNearestStructure(FVector Location, TSubclassOf<APOTLStructure> StructureClass)
{
	APOTLStructure* NearestStructure = nullptr;
	float LastNearestDistance = 99999999999999.f;
	TArray<AActor*> FoundStructures;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), StructureClass, FoundStructures);
	for (auto Actor : FoundStructures)
	{
		APOTLStructure* Structure = Cast<APOTLStructure>(Actor);
		if (Structure)
		{
			FVector StructureLocation = Structure->GetActorLocation();
			FVector VectorDistance = StructureLocation - Location;
			VectorDistance = VectorDistance.GetAbs();
			FVector2D Vector2DDistance = FVector2D({ VectorDistance.X, VectorDistance.Y });
			float Distance = Vector2DDistance.Size();
			if (Distance < LastNearestDistance)
			{
				NearestStructure = Structure;
				LastNearestDistance = Distance;
			}
		}
	}
	return NearestStructure;
}