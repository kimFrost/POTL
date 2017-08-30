UENUM(BlueprintType)
enum class EHandleType : uint8
{
	Handled UMETA(DisplayName = "Handled"),
	HandledBreak UMETA(DisplayName = "HandledBreak"),
	Unhandled UMETA(DisplayName = "Unhandled")
};




DECLARE_DELEGATE_RetVal_OneParam(EHandleType, FOnHexClickedDelegate, UHexTile*);


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





///////////// STRUCTURE

void APOTLStructure::EnterEditMode()
{

	FOnHexClickedDelegate* Delegate = Hex->BindToOnHexClicked(this, 0);
	if (Delegate)
	{
		Delegate->BindUObject(this, &APOTLStructure::ToggleAllocateHex, false);
	}
}


void APOTLStructure::LeaveEditMode()
{
	if (bInEditMode)
	{
		for (auto& Hex : HexesInRange)
		{
			if (Hex)
			{
				Hex->HideDecal();
				Hex->UnbindToHexClicked(this);
			}
		}
		bInEditMode = false;
		OnLeaveEditMode();
	}
}



EHandleType APOTLStructure::ToggleAllocateHex(UHexTile* Hex, bool bUpdate)
{
	if (Hex)
	{
		bool bChanged = false;
		if (Hex->AllocatedTo == this)
		{
			bChanged = UnallocateHex(Hex);
		}
		else
		{
			bChanged = AllocateHex(Hex);
		}
		
		if (bChanged)
		{
			OnAllocatedHexesChangedDelegate.Broadcast();
			OnAllocatedHexesChanged();

			//~~ Update AllocatedHexes in all structure components ~~//
			TArray<UActorComponent*> StructureComponents = GetComponentsByClass(UStructureComponent::StaticClass());
			for (auto& Component : StructureComponents)
			{
				UStructureComponent* StructureComponent = Cast<UStructureComponent>(Component);
				if (StructureComponent)
				{
					StructureComponent->AllocatedHexes = AllocatedHexes;
				}
			}
		}
	}

	return EHandleType::Unhandled;
}