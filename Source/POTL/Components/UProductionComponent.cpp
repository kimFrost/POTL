// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLGameMode.h"
#include "POTLGameInstance.h"
#include "POTLStructure.h"
#include "UProductionComponent.h"



// Sets default values for this component's properties
UProductionComponent::UProductionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//TaskLength = 5.f;

	/*
	StorageCapacity = 0.f;
	PowerUpkeep = 0.f;
	PowerGenerated = 0.f;

	StoredPower = 0.f;
	SurplusPower = 0.f;
	*/
}


/******************** OnProduction *************************/
void UProductionComponent::OnProduction_Implementation()
{

}



/******************** OnProgressComplete *************************/
void UProductionComponent::OnProgressComplete()
{
	Super::OnProgressComplete();

	OnProduction();

	//UE_LOG(LogTemp, Log, TEXT("ULaborComponent::OnProgressComplete"));
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("UProductionComponent::OnProgressComplete"));

	if (IsValid(ParentStructure))
	{
		// Add resource to storage
		for (auto& ProductionItem : Production)
		{
			ParentStructure->AddResource(ProductionItem.Key, ProductionItem.Value, EResourceList::Free);
		}

		if (ParentStructure->AttachedTo)
		{

		}
	}
}


/*
float UProductionComponent::GeneratePower()
{
	return PowerGenerated;

}


// Called when the game starts
void UProductionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(Module))
	{
		AGameModeBattle* GameMode = Cast<AGameModeBattle>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{

		}
	}
}


// Called every frame
void UProductionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
*/
