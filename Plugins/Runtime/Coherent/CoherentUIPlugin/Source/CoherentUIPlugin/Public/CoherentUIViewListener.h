/*
This file is part of Coherent UI, modern user interface library for
games.

Copyright (c) 2012-2014 Coherent Labs AD and/or its licensors. All
rights reserved in all media.

The coded instructions, statements, computer programs, and/or related
material (collectively the "Data") in these files contain confidential
and unpublished information proprietary Coherent Labs and/or its
licensors, which is protected by United States of America federal
copyright law and by international treaties.

This software or source code is supplied under the terms of a license
agreement and nondisclosure agreement with Coherent Labs Limited and may
not be copied, disclosed, or exploited except in accordance with the
terms of that agreement. The Data may not be disclosed or distributed to
third parties, in whole or in part, without the prior written consent of
Coherent Labs Limited.

COHERENT LABS MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS
SOURCE CODE FOR ANY PURPOSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER, ITS AFFILIATES,
PARENT COMPANIES, LICENSORS, SUPPLIERS, OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OR PERFORMANCE OF THIS SOFTWARE OR SOURCE CODE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include "Coherent/UI/ViewListener.h"

namespace Coherent
{
namespace UI
{
	
FORCEINLINE uint32 GetTypeHash(const Coherent::UI::CoherentHandle& handle)
{
	return FCrc::MemCrc32(&handle.HandleValue, sizeof(handle.HandleValue));
}

}
}

class CoUISystemWrapper;
typedef TSharedPtr<CoUISystemWrapper, ESPMode::ThreadSafe> SystemWrapperSharedPtr;

class COHERENTUIPLUGIN_API FCoherentUIViewListener
	: public Coherent::UI::ViewListener
{
public:
	FCoherentUIViewListener();
	virtual ~FCoherentUIViewListener();
	
	Coherent::UI::View* GetView() const;
	bool IsReadyToCreateView() const;

	void CreateSystemIfNecessary(UWorld* world);
	class ACoherentUISystem* GetSystemActor(UWorld* world) const;
	
protected:
	void DoResize(uint32 width, uint32 height);

	virtual void OnViewCreated(Coherent::UI::View* view) override;
	virtual void Release() override;
	virtual void OnDraw(Coherent::UI::CoherentHandle handle,
		bool usesSharedMemory,
		int width,
		int height) override;
	
	virtual void CreateSurface(bool sharedMemory,
		unsigned width,
		unsigned height,
		Coherent::UI::SurfaceResponse* response) override;
	virtual void DestroySurface(Coherent::UI::CoherentHandle surface,
		bool usesSharedMemory) override;
	
	virtual void OnError(const Coherent::UI::ViewError& error) override;
	
	virtual void TickListener();

	class COHERENTUIPLUGIN_API MappedMemory
	{
	public:
		explicit MappedMemory(Coherent::UI::CoherentHandle handle, size_t size);
		MappedMemory(const MappedMemory& other);
		~MappedMemory();
		MappedMemory& operator=(const MappedMemory& other);

		void* GetMemory() const;

	private:
		void Release();
		FThreadSafeCounter* RefCount;
		void* Memory;
		size_t Size;
	};

protected:
	void Destroy();

	SystemWrapperSharedPtr System;
	UTexture2D* Texture;

private:
	MappedMemory MapMemory(Coherent::UI::CoherentHandle handle);
	
	void DestroyCurrentTexture();
	void RecreateTexture(uint32 width, uint32 height);

	Coherent::UI::View* View;
	
	TMap<Coherent::UI::CoherentHandle, MappedMemory> MemoryHandles;
	
	TMap<Coherent::UI::CoherentHandle, FTexture2DRHIRef> SharedTextureHandles;
	struct FCreateSurfaceTask
	{
		FCreateSurfaceTask();

		unsigned Width;
		unsigned Height;
		Coherent::UI::SurfaceResponse* Response;
		FTexture2DRHIRef Texture;
		Coherent::UI::CoherentHandle Handle;
	};
	FCriticalSection CreateSurfaceCS;
	typedef TArray<TSharedPtr<FCreateSurfaceTask>> CreateSurfaceTaskArray;
	CreateSurfaceTaskArray CreateSurfaceTasksComplete;

	void DoCreateSurfaceOnRenderThread(const TSharedPtr<FCreateSurfaceTask>& task);
};