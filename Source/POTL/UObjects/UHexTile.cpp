// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "POTLDataHolder.h"
#include "POTLGameInstance.h"
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

	HexTileType = "Grassland";

	AllocatedTo = nullptr;
}


UHexTile::~UHexTile()
{

}


FOnHexClickedDelegate* UHexTile::BindToOnHexClicked(UObject* Listener, int Priority)
{
	if (Listener)
	{
		UnbindToHexClicked(Listener);
		FOnHexClickedDelegate* Delegate = new FOnHexClickedDelegate();
		OnHexClickedDelegates.Add(Listener, Delegate);
		return Delegate;
	}
	else
	{
		return nullptr;
	}
}
void UHexTile::UnbindToHexClicked(UObject* Listener)
{
	if (Listener && OnHexClickedDelegates.Contains(Listener))
	{
		FOnHexClickedDelegate* Delegate = OnHexClickedDelegates[Listener];
		Delegate->Unbind();
		delete Delegate;
		OnHexClickedDelegates.Remove(Listener);
	}
}
void UHexTile::Select()
{
	if (!bSelected)
	{
		bSelected = true;
		//OnSelected();
		if (WorldRef)
		{
			UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(WorldRef->GetGameInstance());
			if (GameInstance)
			{
				//GameInstance->OnStructureSelectedDelegate.Broadcast(this);
			}
		}
	}
}
void UHexTile::Deselect()
{
	if (bSelected)
	{
		bSelected = false;
		//OnDeselected();
		UPOTLGameInstance* GameInstance = Cast<UPOTLGameInstance>(WorldRef->GetGameInstance());
		if (GameInstance)
		{
			//GameInstance->OnStructureDeselectedDelegate.Broadcast(this);
		}
	}
}
void UHexTile::ClickHex()
{
	bool Handled = false;
	for (auto& Delegate : OnHexClickedDelegates)
	{
		if (Delegate.Key && Delegate.Value && Delegate.Value->IsBound())
		{
			EHandleType Response = Delegate.Value->Execute(this);
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

bool UHexTile::IsBuildable() //const FST_BuildInfo& BuildInfo
{
	//FST_BuildInfo, (FST_Structure, RotationDirection)

	// Structure check and allocate check
	if (AllocatedTo || AttachedBuilding)
	{
		return false;
	}
	// Terrain type check
	if (HexTileType != "Grassland")
	{
		return false;
	}
	/*
	// Check for attachto on neighbours (Don't let hex make this validation)
	if (BuildInfo.StructureInfo.AttachTo.Num() > 0)
	{
		bool AttachToValid = true;
		UHexTile* NeighbourHex = HexNeighbors[BuildInfo.RotationDirection];
		if (NeighbourHex)
		{
			if (NeighbourHex->AttachedBuilding)
			{
				FString NeighbourStructureId = NeighbourHex->AttachedBuilding->StructureBaseData.Id;
				if (!BuildInfo.StructureInfo.AttachTo.Contains(NeighbourStructureId))
				{
					AttachToValid = false;
				}
			}
			else
			{
				AttachToValid = false;
			}
		}
		else
		{
			AttachToValid = false;
		}
		if (!AttachToValid)
		{
			return false;
		}
	}
	*/
	

	// slope check


	return true;
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
