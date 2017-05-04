// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLDataHolder.h"
#include "UObjects/UHexPoint.h"
#include "HexDecal.h"
#include "UHexTile.h"




UHexTile::UHexTile()
{
	WorldRef = nullptr;
	HexDecal = nullptr;

	Location = { -1, -1, -1 };
	Rotation = { 0.f, 0.f, 0.f };

	Point0 = nullptr;
	Point1 = nullptr;
	Point2 = nullptr;
	Point3 = nullptr;
	Point4 = nullptr;
	Point5 = nullptr;

	Remove = false;
	AttachedBuilding = nullptr;
	AttachedActor = nullptr;
	AttachedMesh = nullptr;
	HexCubeCoords = { -1, -1, -1 };
	HexOffsetCoords = { -1, -1 };
	HexIndex = -1;
	HexNeighborIndexes.Init(-1, 6);
	HexNeighbors.Init(nullptr, 6);
	//HighlightDecal = nullptr;
	DebugMe = false;

	AllocatedTo = nullptr;
}


UHexTile::~UHexTile()
{

}


FOnHexClickedDelegate* UHexTile::BindToOnHexClicked(int Priority)
{
	FOnHexClickedDelegate* Delegate = new FOnHexClickedDelegate();
	OnHexClickedDelegates.Add(Delegate);
	//delete Delegate;
	return Delegate;
}
void UHexTile::ClickHex()
{
	bool Handled = false;
	for (auto& Delegate : OnHexClickedDelegates)
	{
		if (Delegate && Delegate->IsBound())
		{
			EHandleType Response = Delegate->Execute(this);
			if (Response == EHandleType::HandledBreak)
			{
				break;
			}
			else if (Response == EHandleType::Handled)
			{
				Handled = true;
			}
		}
	}
	if (!Handled)
	{
		//Select();
	}
}
void UHexTile::OnHexClicked_Implementation()
{

}

/******************** GetNeighbourHex *************************/
UHexTile* UHexTile::GetNeighbourHex(int Direction)
{
	UHexTile* NeighbourHex = HexNeighbors[Direction];
	if (NeighbourHex)
	{
		return NeighbourHex;
	}
	return nullptr;
}


/******************** Init *************************/
void UHexTile::HideDecal()
{
	if (HexDecal)
	{
		HexDecal->HideDecal();
	}
}


/******************** Init *************************/
void UHexTile::ShowDecal(EDecalType Type)
{
	if (HexDecal)
	{
		HexDecal->ChangeMaterial(Type);
	}
}


/******************** Init *************************/
void UHexTile::Init_Implementation()
{
	if (WorldRef)
	{
		// Create module placeholder
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		HexDecal = WorldRef->SpawnActor<AHexDecal>(AHexDecal::StaticClass(), Location, Rotation);
		if (HexDecal)
		{
			//PlaceholderModule->CurrentState = EModuleState::STATE_Placeholder;
			//PlaceholderModule->SpawnOrder = this;
			//PlaceholderModule->InitModule(); // InitModule is implemented in blueprint with _implemented
		}
	}
}
