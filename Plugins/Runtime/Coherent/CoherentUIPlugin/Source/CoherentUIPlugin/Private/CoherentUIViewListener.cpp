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
#include "CoherentUIPluginPrivatePCH.h"
#include "CoherentUIViewListener.h"

#include "Coherent/UI/View.h"

#if PLATFORM_WINDOWS
#include "AllowWindowsPlatformTypes.h"
#include <d3d11.h>
#include "HideWindowsPlatformTypes.h"
#elif PLATFORM_LINUX
#include <sys/shm.h>
#endif

FCoherentUIViewListener::MappedMemory::MappedMemory(Coherent::UI::CoherentHandle handle, size_t size)
	: RefCount(new FThreadSafeCounter)
#if PLATFORM_WINDOWS
	, Memory(::MapViewOfFile(handle, FILE_MAP_READ, 0, 0, size))
#elif PLATFORM_MAC
	, Memory(::mmap(nullptr, size, PROT_READ, MAP_SHARED, handle.HandleValue, 0))
#elif PLATFORM_LINUX
	, Memory(::shmat(handle.HandleValue, 0, SHM_RDONLY))
#endif
	, Size(size)
{
	RefCount->Increment();

#if PLATFORM_MAC
	if (Memory == MAP_FAILED)
	{
		Memory = nullptr;
	}
#elif PLATFORM_LINUX
	if (Memory == ((void*)-1))
	{
		Memory = nullptr;
	}
#endif
	if (Memory == nullptr)
	{
		UE_LOG(LogCoherentUI, Error, TEXT("Unable to map memory for handle %d with size %d!"), handle.HandleValue, size);
	}
}

FCoherentUIViewListener::MappedMemory::MappedMemory(const MappedMemory& other)
	: RefCount(other.RefCount)
	, Memory(other.Memory)
	, Size(other.Size)
{
	RefCount->Increment();
}

FCoherentUIViewListener::MappedMemory::~MappedMemory()
{
	Release();
}

FCoherentUIViewListener::MappedMemory&
	FCoherentUIViewListener::MappedMemory::operator=(const MappedMemory& other)
{
	Release();
	RefCount = other.RefCount;
	Memory = other.Memory;
	Size = other.Size;
	RefCount->Increment();

	return *this;
}

void FCoherentUIViewListener::MappedMemory::Release()
{
	if (RefCount->Decrement() == 0)
	{
		delete RefCount;
		if (Memory)
		{
#if PLATFORM_WINDOWS
			::UnmapViewOfFile(Memory);
#elif PLATFORM_MAC
			::munmap(Memory, Size);
#elif PLATFORM_LINUX
			::shmdt(Memory);
#endif
			Memory = nullptr;
		}
	}
}

void* FCoherentUIViewListener::MappedMemory::GetMemory() const
{
	return Memory;
}

FCoherentUIViewListener::FCoherentUIViewListener()
	: Texture(nullptr)
	, View(nullptr)
{}

FCoherentUIViewListener::~FCoherentUIViewListener()
{
	Destroy();
}

Coherent::UI::View* FCoherentUIViewListener::GetView() const
{
	return View;
}

void FCoherentUIViewListener::OnViewCreated(Coherent::UI::View* view)
{
	View = view;
	RecreateTexture(View->GetWidth(), View->GetHeight());
}

void FCoherentUIViewListener::Release()
{
	View = nullptr;
}

void FCoherentUIViewListener::DestroyCurrentTexture()
{
	if (Texture)
	{
		Texture->RemoveFromRoot();

		if (Texture->Resource)
		{
			Texture->ReleaseResource();
		}
		Texture->MarkPendingKill();
		Texture = nullptr;
	}
}

void FCoherentUIViewListener::RecreateTexture(uint32 width, uint32 height)
{
	DestroyCurrentTexture();
	Texture = UTexture2D::CreateTransient(width, height);

	auto& mipLevel = Texture->PlatformData->Mips[0];
	auto data = mipLevel.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memset(data, 0, width * height * 4);
	mipLevel.BulkData.Unlock();

	Texture->AddToRoot();
	Texture->UpdateResource();
}

void FCoherentUIViewListener::Destroy()
{
	if (View)
	{
		View->Destroy();
		View = nullptr;
	}
	DestroyCurrentTexture();
}

void FCoherentUIViewListener::DoResize(uint32 width, uint32 height)
{
	if (View && (View->GetWidth() != width || View->GetHeight() != height))
	{
		View->Resize(width, height);
		RecreateTexture(width, height);
	}
}

void FCoherentUIViewListener::OnDraw(Coherent::UI::CoherentHandle handle, bool usesSharedMemory, int width, int height)
{
	if (Texture && Texture->Resource)
	{
		auto rhiRef = static_cast<FTexture2DResource*>(Texture->Resource)->GetTexture2DRHI();
		if (!rhiRef)
			return;

		if (usesSharedMemory)
		{
			MappedMemory mem = MapMemory(handle);
			if (!mem.GetMemory())
			{
				return;
			}
			const size_t size = width*height * 4;
			uint32 stride = 0;
			void* MipData = GDynamicRHI->RHILockTexture2D(rhiRef, 0, RLM_WriteOnly, stride, false);
			if (MipData)
			{
				FMemory::Memcpy(MipData, mem.GetMemory(), size);
				GDynamicRHI->RHIUnlockTexture2D(rhiRef, 0, false);
			}
		}
		else
		{
			auto srcRhiRef = SharedTextureHandles.Find(handle);
			if (!srcRhiRef)
			{
				UE_LOG(LogCoherentUI, Error, TEXT("Unable to find texture for handle!"));
				return;
			}

			RHICopySharedMips(rhiRef, *srcRhiRef);
		}
	}
}

void FCoherentUIViewListener::CreateSystemIfNecessary(UWorld* world)
{
	auto sysActor = GetSystemActor(world);
	if (!sysActor)
	{
		FActorSpawnParameters Params;
		Params.bAllowDuringConstructionScript = true;
		sysActor = world->SpawnActor<ACoherentUISystem>(Params);
	}

	System = sysActor->GetSystemObject();
}

ACoherentUISystem* FCoherentUIViewListener::GetSystemActor(UWorld* world) const
{
	for (TActorIterator<ACoherentUISystem> It(world); It; ++It)
	{
		return *It;
	}

	return nullptr;
}


bool FCoherentUIViewListener::IsReadyToCreateView() const
{
	return System.IsValid() && System->IsReady();
}

FCoherentUIViewListener::FCreateSurfaceTask::FCreateSurfaceTask()
: Width(0)
, Height(0)
, Response(nullptr)
{}

void FCoherentUIViewListener::TickListener()
{
	CreateSurfaceCS.Lock();
	for (int32 i = 0; i < CreateSurfaceTasksComplete.Num(); ++i)
	{
		const auto& task = CreateSurfaceTasksComplete[i];
		SharedTextureHandles.Add(task->Handle, task->Texture);
		task->Response->Signal(task->Handle);
	}
	CreateSurfaceTasksComplete.Empty();
	CreateSurfaceCS.Unlock();
}

void FCoherentUIViewListener::DoCreateSurfaceOnRenderThread(const TSharedPtr<FCreateSurfaceTask>& task)
{
#if PLATFORM_WINDOWS
	FRHIResourceCreateInfo createInfo;
	task->Texture = RHICreateTexture2D(task->Width, task->Height, PF_B8G8R8A8, 1, 1, TexCreate_RenderTargetable | TexCreate_ShaderResource | TexCreate_Shared, createInfo);

	auto resource = static_cast<ID3D11Resource*>(task->Texture->GetNativeResource());

	IDXGIResource* dxgiResource;
	resource->QueryInterface(__uuidof(IDXGIResource), (void**)&dxgiResource);

	if (!dxgiResource)
	{
		UE_LOG(LogCoherentUI, Error, TEXT("Unable to query DXGI resource for shared texture"));
		return;
	}

	HANDLE handle = nullptr;
	dxgiResource->GetSharedHandle(&handle);
	task->Handle = Coherent::UI::CoherentHandle(handle);

	dxgiResource->Release();

	CreateSurfaceCS.Lock();
	CreateSurfaceTasksComplete.Add(task);
	CreateSurfaceCS.Unlock();
#else
	UE_LOG(LogCoherentUI, Error, TEXT("FCoherentUIViewListener::DoCreateSurfaceOnRenderThread is implemented on Windows only!"));
#endif
}


#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
namespace
{
bool IsSharedMemoryHandleValid(const Coherent::UI::CoherentHandle& handle, unsigned pixelCount)
{
#if PLATFORM_WINDOWS
	if (handle.HandleValue == 0)
#elif PLATFORM_MAC || PLATFORM_LINUX
	if (handle.HandleValue == -1)
#endif
	{
		if (pixelCount == 0)
		{
			UE_LOG(LogCoherentUI, Error, TEXT("Surface resolution cannot be 0 (width * height != 0)!"));
		}
		else
		{
#if PLATFORM_WINDOWS
			auto errorCode = ::GetLastError();
#elif PLATFORM_MAC || PLATFORM_LINUX
			auto errorCode = errno;
#endif
			UE_LOG(LogCoherentUI, Error, TEXT("Could not create shared memory. Platform-specific error code: %d"), errorCode);
		}
		return false;
	}
	return true;
}
}
#endif

void FCoherentUIViewListener::CreateSurface(bool sharedMemory, unsigned width, unsigned height, Coherent::UI::SurfaceResponse* response)
{
#if PLATFORM_WINDOWS
	if (sharedMemory)
	{
		Coherent::UI::CoherentHandle result(::CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, width*height * 4, nullptr));

		if (!IsSharedMemoryHandleValid(result, width * height))
		{
			response->Signal(Coherent::UI::CoherentHandle());
			return;
		}

		response->Signal(result);
	}
	else
	{
		TSharedPtr<FCreateSurfaceTask> Task(new FCreateSurfaceTask);
		Task->Width = width;
		Task->Height = height;
		Task->Response = response;
		
		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			CreateCoherentSurface,
			TSharedPtr<FCreateSurfaceTask>, task, Task,
			FCoherentUIViewListener*, listener, this,
			{
				listener->DoCreateSurfaceOnRenderThread(task);
			}
		);
	}
#elif PLATFORM_MAC
	if (sharedMemory)
	{
		FString name = FString("/tmp/couibuf_") + TTypeToString<int>::ToString(rand()) + FString("_") + TTypeToString<int>::ToString(rand());
		auto fd = ::open(TCHAR_TO_UTF8(*name), O_RDWR | O_CREAT, 0666);
		Coherent::UI::CoherentHandle result(fd, TCHAR_TO_UTF8(*name));

		if (!IsSharedMemoryHandleValid(result, width * height))
		{
			response->Signal(Coherent::UI::CoherentHandle());
			return;
		}

		::ftruncate(fd, width * height * 4);

		response->Signal(result);
	}
	else
	{
		response->Signal(Coherent::UI::CoherentHandle());
		UE_LOG(LogCoherentUI, Error, TEXT("Shared textures are only supported on Windows"));
	}
#elif PLATFORM_LINUX
	if (sharedMemory)
	{
		int segmentID = ::shmget(IPC_PRIVATE, width * height * 4, 0600 | IPC_CREAT | IPC_EXCL);
		Coherent::UI::CoherentHandle result(segmentID);

		if (!IsSharedMemoryHandleValid(result, width * height))
		{
			response->Signal(Coherent::UI::CoherentHandle());
			return;
		}

		response->Signal(result);
	}
	else
	{
		response->Signal(Coherent::UI::CoherentHandle());
		UE_LOG(LogCoherentUI, Error, TEXT("Shared textures are only supported on Windows"));
	}
#else
#error Unsupported platform
#endif
}

void FCoherentUIViewListener::DestroySurface(Coherent::UI::CoherentHandle surface,
	bool usesSharedMemory)
{
#if PLATFORM_WINDOWS
	if (usesSharedMemory)
	{
		MemoryHandles.Remove(surface);
		::CloseHandle(surface);
	}
	else
	{
		SharedTextureHandles.Remove(surface);
	}
#elif PLATFORM_MAC
	if (usesSharedMemory)
	{
		MemoryHandles.Remove(surface);
		::close(surface.HandleValue);
		::unlink(surface.SharedMemoryName);
	}
	else
	{
		UE_LOG(LogCoherentUI, Error, TEXT("Shared textures are only supported on Windows"));
	}
#elif PLATFORM_LINUX
	if (usesSharedMemory)
	{
		MemoryHandles.Remove(surface);
		::shmctl(surface, IPC_RMID, nullptr);
	}
	else
	{
		UE_LOG(LogCoherentUI, Error, TEXT("Shared textures are only supported on Windows"));
	}
#else
#error Unsupported platform
#endif
}

FCoherentUIViewListener::MappedMemory FCoherentUIViewListener::MapMemory(Coherent::UI::CoherentHandle handle)
{
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	MappedMemory* mem = MemoryHandles.Find(handle);
	if(!mem)
	{
		const size_t size = View->GetWidth()*View->GetHeight() * 4;
		MappedMemory mapped(handle, size);
		MemoryHandles.Add(handle, mapped);
		return mapped;
	}
	else
	{
		return *mem;
	}
#else
#error Unsupported platform
#endif
}

void FCoherentUIViewListener::OnError(const Coherent::UI::ViewError& error)
{
	UE_LOG(LogCoherentUI, Error, TEXT("View error: %s"), UTF8_TO_TCHAR(error.Error));
}
