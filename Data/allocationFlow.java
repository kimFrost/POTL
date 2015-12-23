

RequestResources(APOTLStructure* RequestFrom, UFactoryComponent* Factory, TMap<FString, int32>& Request, TMap<FString, int32>& Payoff, int32 Sequence, int32 Steps, EAllocationType Type, bool Consume, bool Bubble)
	FST_ResourceRequest ResourceRequest;
	ResourceRequest.From = RequestFrom;
	ResourceRequest.Factory = Factory;
	ResourceRequest.Request = Request;
	ResourceRequest.Payoff = Payoff;
	ResourceRequest.Sequence = Sequence;
	ResourceRequest.Steps = Steps;
	ResourceRequest.Type = Type;
	ResourceRequest.Consume = Consume;
	ResourceRequest.Bubble = Bubble;
	ResourceRequests.Add(ResourceRequest);



AllocateResource(APOTLStructure* To, FString ResourceKey, int32 Quantity, EAllocationType Type, int32 Sequence, bool KeyLoop, int32 Key)
	FST_ResourceAllocation Allocation;
	Allocation.To = To;
	if (this->IsRoot)
	{
		Allocation.From = this; //~~ Always send from root for now ~~//
	}
	else
	{
		Allocation.From = this->Root;
	}
	Allocation.ResourceKey = ResourceKey;
	Allocation.Type = Type;
	Allocation.Quantity = Quantity;
	Allocation.Sequence = Sequence;

	if (Allocation.Type == EAllocationType::FactoryProduction)
	{
		if (FreeResources.Contains(ResourceKey))
		{

		}
		else
		{
			FreeResources.Add(ResourceKey, Quantity)
		}
	}
	else
	{
		//~~ Remove resources from FreeResources on this ~~//
		if (FreeResources.Contains(ResourceKey) && FreeResources[ResourceKey] >= Quantity)
		{
			FreeResources[ResourceKey] = FreeResources[ResourceKey] - Quantity;
			if (FreeResources[ResourceKey] == 0)
			{
				FreeResources.Remove(ResourceKey);
			}
		}
		else
		{
			return Key; //~~ If FreeResources doesn't have the resource. Just for safe handling ~~//
		}
	}

	//~~ Add the allocation ~~//
	if (AllocatedResources.Contains(Key))
	{
		//~~ Overwrite existing allocation ~~//
		AllocatedResources[Key] = Allocation;
	}
	else 
	{
		AllocatedResources.Add(Key, Allocation);
	}




ProcessFactories()
	this->Root->RequestResources(this, Factory, Factory->Requirements, Factory->Invoice, Sequence, 0, EAllocationType::RequestDirect, false, true);




ProcessResourceRequests()
	// FreeResources
	int32 AllocationIndex = AllocateResource(ResourceRequest.From, ReqResource.Key, AvailableQuantity, EAllocationType::FactoryBilling, ii, false, -1); // Consume?
	AllocationIndexes.Add(AllocationIndex);
	ReqResource.Value = ReqResource.Value - AvailableQuantity;

	// Reallocate
	int32 AllocationIndex = AllocateResource(ResourceRequest.From, ReqResource.Key, AvailableQuantity, EAllocationType::FactoryBilling, ii, false, -1); // Consume?
	AllocationIndexes.Add(AllocationIndex);
	Allocation.Quantity = Allocation.Quantity - AvailableQuantity; //! This will not give a clear picture of what is happening. How to I Reallocate an allocation, or split it into two, so it is clear to the user?





ResolveAllocations(EAllocationType::All, true)
	if (Type == EAllocationType::All || Allocation.Type == Type)
	{
		if (Allocation.Type == EAllocationType::FactoryBilling)
		{
			//~~ The resources are allready removed from freesources, so the allocation just needs to be removed ~~//
			//Allocation.To->AddResource(Allocation.ResourceKey, Allocation.Quantity * -1, EResourceList::Free); //!! Might not be all safe to do it with just a negative value.
			//AllocatedResources.Remove(AllocatedResource.Key);
		}
		else
		{
			Allocation.To->AddResource(Allocation.ResourceKey, Allocation.Quantity, EResourceList::Free);
			AllocatedResources.Remove(AllocatedResource.Key);
		}
	}