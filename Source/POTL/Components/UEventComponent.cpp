// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLStructure.h"
#include "UObjects/UHexTile.h"
#include "POTLGameInstance.h"
#include "UObjects/Singletons/UEventSingleton.h"
#include "UEventComponent.h"



// Sets default values for this component's properties
UEventComponent::UEventComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	

	// Get ref to singleton object. how??
	//USingletonManager::Get
	//USingletonManager::GetEventSingleton();

	//UEventSingleton* EventSingleton = USingletonManager::GetEventSingleton();
	// Get Game instance

	//Bind event to EventSingleton and sendt to all but caller

	// Implement intercept strategy with return handlers??


	
}


FOnCancelRetDelegate* UEventComponent::BindToCancelEvent(UObject* Listener, int Priority)
{
	if (EventSingleton)
	{
		return EventSingleton->BindToCancelEvent(Listener, Priority);
	}
	return nullptr;
}

void UEventComponent::UnbindToCancelEvent(UObject* Listener)
{
	if (EventSingleton)
	{ 
		EventSingleton->UnbindToCancelEvent(Listener);
	}
}

void UEventComponent::TriggerConfirmEvent()
{
	if (EventSingleton)
	{
		EventSingleton->TriggerConfirmEvent();
	}
}

void UEventComponent::TriggerCancelEvent()
{
	if (EventSingleton)
	{
		EventSingleton->TriggerCancelEvent();
	}
}



void UEventComponent::InitializeComponent()
{
	Super::InitializeComponent();

}


void UEventComponent::BeginPlay()
{
	Super::BeginPlay();

	UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
	if (GameInstance)
	{
		EventSingleton = GameInstance->GetEventSingleton();
	}
}