// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLStructure.h"
#include "POTLGameMode.h"
#include "POTLGameInstance.h"
#include "UResidentsComponent.h"


// Sets default values for this component's properties
UResidentsComponent::UResidentsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ResidentDensity = 1.f;
	NumOfHoseholds = 1;
	Wealth = 0.f;
	Food = 20.f;
	ConsumptionRate = 1.f;
}


/******************** CheckNeeds *************************/
void UResidentsComponent::CheckNeeds()
{
	// Parse needs and make requests
	UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
	if (GameInstance)
	{
		if (Food < 10.f)
		{
			// Request food of quality matching 1/10 of wealth
			//GameInstance->RequestResourceByTag();
		}
		if (Food < 1.f)
		{
			// Request food of quality matching below or equal wealth;
			UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
			if (GameInstance)
			{

			}
		}
	}
}


/******************** Init *************************/
void UResidentsComponent::Init()
{
	Super::Init();



	// CheckProduction every second
	GetWorld()->GetTimerManager().SetTimer(NeedsCheckTimer, this, &UResidentsComponent::CheckNeeds, 1.f, true);
}


/******************** OnTimeUpdate *************************/
void UResidentsComponent::OnTimeUpdate(float Time, float TimeProgressed)
{
	Super::OnTimeUpdate(Time, TimeProgressed);

	// Update needs
	Food -= TimeProgressed * (ConsumptionRate * NumOfHoseholds * ResidentDensity);
	//if (Food < 0) Food = 0.f; // Let it go under zero. It means starving
}



// Called when the game starts
void UResidentsComponent::BeginPlay()
{
	Super::BeginPlay();


}


// Called every frame
void UResidentsComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

