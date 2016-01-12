// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "GatherComponent.h"


// Sets default values for this component's properties
UGatherComponent::UGatherComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
	
	GatherRecipe = "";
}



// Get available resource from gather hex indexes

// On hex resource provideTo switch, recal prev provideTo without the removed hex index



int32 UGatherComponent::CalcAvaiableResources(UDataTable* RecipeTable)
{
	int32 QuantityAvaible = 0;
	//GatherRecipe

	for (int32 i = 0; i < GatherFromIndexes.Num(); i++)
	{

	}

	//GatherFromIndexes

	/*
	Requirements.Empty();
	Invoice.Empty();
	Invoice.Add(Recipe, Quantity);
	if (RecipeTable)
	{
		for (auto& InvoiceItem : Invoice)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, InvoiceItem.Key);
			static const FString ContextString(TEXT("GENERAL")); //~~ Key value for each column of values ~~//
																 //FST_ResourceRecipe* Recipe = RecipeTable->FindRow<FST_ResourceRecipe>(InvoiceItem.Key, ContextString);
			FST_ResourceRecipe* Recipe = RecipeTable->FindRow<FST_ResourceRecipe>(*InvoiceItem.Key, ContextString);
			if (Recipe)
			{
				for (auto& Ingredient : Recipe->Ingredients)
				{
					if (Requirements.Contains(Ingredient.Id.ToString()))	Requirements[Ingredient.Id.ToString()] += Ingredient.Quantity;
					else													Requirements.Add(Ingredient.Id.ToString(), Ingredient.Quantity);
				}
				if (Recipe->Sequence > MinSequence) {
					MinSequence = Recipe->Sequence;
				}
			}
		}
	}
	*/
	return QuantityAvaible;
}


void UGatherComponent::Resolve(APOTLStructure* Caller)
{

}


// Called when the game starts
void UGatherComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
}


// Called every frame
void UGatherComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
	// ...
}

