// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
#include "FactoryComponent.h"


// Sets default values for this component's properties
UFactoryComponent::UFactoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	Recipe = TEXT("");
}


int32 UFactoryComponent::ProcessInvoice(UDataTable* RecipeTable)
{
	Requirements.Empty();
	Invoice.Empty();
	Invoice.Add(Recipe, Quantity);
	int32 MinSequence = 0;
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
	return MinSequence;
}



void UFactoryComponent::Resolve(APOTLStructure* Caller, TMap<FString, int32>& FreeResources, UDataTable* RecipeTable, TMap<FString, int32>& Production, TMap<FString, int32>& Billing)
{
	if (RecipeTable)
	{
		//~~ The production ~~//
		int32 i = 0;
		//TArray<FName> IdList;
		//Invoice.GenerateKeyArray(IdList);
		TArray<int32> ValueList;
		Invoice.GenerateValueArray(ValueList);

		//~~ Calculate total singleton length of production ~~//
		int32 SingletonLength = 0;
		for (i = 0; i < ValueList.Num(); i++)
		{
			SingletonLength += ValueList[i];
		}

		TMap<FString, int32> InvoiceCopy = Invoice;
		TMultiMap<FString, int32> SingletonQue;
		//~~ Loop through SingletonLength and pull values from the invoice ~~//
		for (i = 0; i < SingletonLength; i++)
		{
			if (SingletonQue.Num() < SingletonLength)
			{
				for (auto& InvoiceItem : InvoiceCopy)
				{
					if (InvoiceItem.Value > 0)
					{
						SingletonQue.Add(InvoiceItem.Key, 1);
						InvoiceItem.Value -= 1;
					}
				}
			}
			else
			{
				break;
			}
		}

		//~~ Produce items ~~//
		bool InvoiceFulfilled = true;
		for (auto& Singleton : SingletonQue)
		{
			static const FString ContextString(TEXT("GENERAL")); //~~ Key value for each column of values ~~//
			//FST_ResourceRecipe* Recipe = RecipeTable->FindRow<FST_ResourceRecipe>(Singleton.Key, ContextString);
			FST_ResourceRecipe* Recipe = RecipeTable->FindRow<FST_ResourceRecipe>(*Singleton.Key, ContextString);
			if (Recipe) //~~ If recipe for invoce item is found ~~//
			{
				bool ResourcesRequirementFulfilled = true;
				for (auto& Ingredient : Recipe->Ingredients)
				{
					if (Ingredient.Id.ToString() != "")
					{
						if (FreeResources.Contains(Ingredient.Id.ToString()))
						{
							GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Magenta, "Ingredient.Id.ToString(): " + Ingredient.Id.ToString());
							GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Magenta, "FreeResources[Ingredient.Id.ToString()]: " + FString::FromInt(FreeResources[Ingredient.Id.ToString()]));
							GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Magenta, "Ingredient.Quantity: " + FString::FromInt(Ingredient.Quantity));
							if (FreeResources[Ingredient.Id.ToString()] < Ingredient.Quantity)
							{
								ResourcesRequirementFulfilled = false;
								GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Magenta, "ResourcesRequirementFulfilled FALSE");
							}
						}
						else {
							ResourcesRequirementFulfilled = false;
						}
					}
					else {
						ResourcesRequirementFulfilled = false;
					}
				}
				//~~ If there are enough resources for the single item production, then produce it and consume the resources ~~//
				if (ResourcesRequirementFulfilled)
				{
					//~~ Remove resources ~~//
					for (auto& Ingredient : Recipe->Ingredients)
					{
						if (Billing.Contains(Ingredient.Id.ToString()))		Billing[Ingredient.Id.ToString()] += Ingredient.Quantity;
						else												Billing.Add(Ingredient.Id.ToString(), Ingredient.Quantity);
					}
					//~~ Add to production ~~//
					Production.Add(Singleton.Key, Recipe->Servings);
				}
			}
		}
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, "Production.Num(): " + FString::FromInt(Production.Num()));
	}
}


// Called when the game starts
void UFactoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UFactoryComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

