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

#include "CoherentUILiveViewComponent.h"
#include "Coherent/UI/View.h"
#include "Coherent/UI/ImageData.h"

UCoherentUILiveViewComponent::UCoherentUILiveViewComponent(
#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
	const FPostConstructInitializeProperties& PCIP)
#else
	const FObjectInitializer& PCIP)
#endif
	: Super(PCIP)
	, LinkName(TEXT("LiveViewLink"))
	, UpdateEveryFrame(true)
	, Texture(nullptr)
	, UpdateWhenPaused(false)
	, LiveViewImageData(nullptr)
	, bLiveViewImageDataReady(false)
	, bBindingsReady(false)
	, bImageDataNeedsResize(false)
	, Width(0)
	, Height(0)
{
	bWantsInitializeComponent = true;
	bAutoActivate = true;

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bTickEvenWhenPaused = UpdateWhenPaused;
}

void UCoherentUILiveViewComponent::InitializeComponent()
{
	Super::InitializeComponent();

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOG(LogCoherentUI, Error, TEXT("The Coherent UI Live View component requires a valid AActor owner!"));
		return;
	}

	CoherentBaseComponent = Owner->FindComponentByClass<UCoherentBaseComponent>();
	if (!CoherentBaseComponent.IsValid())
	{
		UE_LOG(LogCoherentUI, Error, TEXT("The Coherent UI Live View component requires a Coherent UI Base component!"));
		return;
	}

	CoherentBaseComponent->ReadyForBindings.AddDynamic(this, &UCoherentUILiveViewComponent::OnReadyForBindingsHandler);
	CoherentBaseComponent->BindingsReleased.AddDynamic(this, &UCoherentUILiveViewComponent::OnBindingsReleasedHandler);

	PrimaryComponentTick.bTickEvenWhenPaused = UpdateWhenPaused;

	TArray<USceneCaptureComponent2D*> comps;
	GetOwner()->GetComponents(comps);

	if (comps.Num())
	{
		for (uint8 i = 0; i < comps.Num(); ++i)
		{
			comps[i]->PrimaryComponentTick.bTickEvenWhenPaused = UpdateWhenPaused;
		}
	}
}

void UCoherentUILiveViewComponent::OnReadyForBindingsHandler(int32 FrameID, const FString& Path, bool IsMainFrame)
{
	if (IsMainFrame)
	{
		bBindingsReady = true;
	}
}

void UCoherentUILiveViewComponent::OnBindingsReleasedHandler(int32 FrameID, const FString& Path, bool IsMainFrame)
{
	if (IsMainFrame)
	{
		bBindingsReady = false;
		Destroy();
	}
}

void UCoherentUILiveViewComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (bImageDataNeedsResize)
	{
		RecreateImageData();
		bImageDataNeedsResize = false;
	}

	if (!UpdateEveryFrame)
	{
		return;
	}
	
	QueueUpdateLiveViewOnRenderThread();
}

void UCoherentUILiveViewComponent::RecreateImageData()
{
	verify(bBindingsReady);

	DestroyImageData();

	if (LinkName.IsEmpty())
	{
		UE_LOG(LogCoherentUI, Error, TEXT("The Coherent UI Live View component requires a unique, non-empty link name!"));
		return;
	}

	if (CoherentBaseComponent.IsValid())
	{
		Coherent::UI::View* View = CoherentBaseComponent->GetView();
		if (View)
		{
			ImageBytes.Init(uint8(0), Width * Height * 4);
			LiveViewImageData = View->CreateImageData(TCHAR_TO_ANSI(*LinkName), Width, Height, ImageBytes.GetData(), false);
			if (!LiveViewImageData)
			{
				UE_LOG(LogCoherentUI, Error, TEXT("The Coherent UI Live View failed to initialize! Link name: %s; Size: %d x %d"), *LinkName, Width, Height);
			}
			else
			{
				bLiveViewImageDataReady = true;
			}
		}
		else
		{
			UE_LOG(LogCoherentUI, Error, TEXT("The Coherent UI Live View failed to initialize because the Coherent UI View is null!"));
		}
	}
	else
	{
		UE_LOG(LogCoherentUI, Error, TEXT("The Coherent UI Live View failed to initialize because the Coherent UI Base component is not valid!"));
	}
}

void UCoherentUILiveViewComponent::OnUnregister()
{
	Super::OnUnregister();
	Destroy();
}

void UCoherentUILiveViewComponent::Destroy()
{
	DestroyImageData();

	bBindingsReady = false;
	bImageDataNeedsResize = false;
	ImageBytes.Reset();
	Width = 0;
	Height = 0;
}

void UCoherentUILiveViewComponent::DestroyImageData()
{
	if (LiveViewImageData)
	{
		if (CoherentBaseComponent.IsValid() && CoherentBaseComponent->GetView())
		{
			LiveViewImageData->Destroy();
		}
		LiveViewImageData = nullptr;
	}

	bLiveViewImageDataReady = false;
}

void UCoherentUILiveViewComponent::QueueUpdateLiveViewOnRenderThread()
{
	if (!bBindingsReady)
	{
		return;
	}

	ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
		CoherentUpdateLiveView,
		TWeakObjectPtr<UCoherentUILiveViewComponent>, CoherentLiveViewComponent, this,
		TWeakObjectPtr<UCoherentBaseComponent>, CoherentBaseComponent, CoherentBaseComponent,
		{
			if (CoherentBaseComponent.IsValid() && CoherentLiveViewComponent.IsValid())
			{
				UTexture* LVTexture = CoherentLiveViewComponent->Texture;
				if (LVTexture && LVTexture->Resource)
				{
					FTexture2DRHIRef RHIRef;
					EPixelFormat TexturePixelFormat = EPixelFormat::PF_Unknown;

					// Check the texture class
					if (LVTexture->GetClass() == UTexture2D::StaticClass())
					{
						RHIRef = static_cast<FTexture2DResource*>(LVTexture->Resource)->GetTexture2DRHI();
						TexturePixelFormat = static_cast<UTexture2D*>(LVTexture)->GetPixelFormat();
					}
					else if (LVTexture->GetClass() == UTextureRenderTarget2D::StaticClass())
					{
						RHIRef = static_cast<FTextureRenderTarget2DResource*>(LVTexture->Resource)->GetTextureRHI();
						TexturePixelFormat = static_cast<UTextureRenderTarget2D*>(LVTexture)->GetFormat();
					}
					else
					{
						UE_LOG(LogCoherentUI, Error, TEXT("Unsupported texture type for the Coherent UI Live View component! Supported types are: UTexture2D and UTextureRenderTarget2D."));
						return;
					}

					if (!RHIRef)
					{
						UE_LOG(LogCoherentUI, Error, TEXT("Unable to get Texture2DRHI from texture for Live View!"));
						return;
					}

					if (TexturePixelFormat != PF_B8G8R8A8 && TexturePixelFormat != PF_R8G8B8A8)
					{
						UE_LOG(LogCoherentUI, Error, TEXT("Unsupported texture format for the Coherent UI Live View component! Supported types are: PF_B8G8R8A8 and PF_R8G8B8A8."));
						return;
					}

					if ((int32(LVTexture->GetSurfaceWidth()) != CoherentLiveViewComponent->Width) ||
						(int32(LVTexture->GetSurfaceHeight()) != CoherentLiveViewComponent->Height))
					{
						CoherentLiveViewComponent->OnNewImageDataSize(int32(LVTexture->GetSurfaceWidth()), int32(LVTexture->GetSurfaceHeight()));
						return;
					}

					if (!CoherentLiveViewComponent->bLiveViewImageDataReady)
					{
						return;
					}

					const size_t SizeInBytes = CoherentLiveViewComponent->Width * CoherentLiveViewComponent->Height * 4;
					uint32 Stride = 0;
					void* MipData = GDynamicRHI->RHILockTexture2D(RHIRef, 0, RLM_ReadOnly, Stride, false);
					if (MipData)
					{
						CoherentLiveViewComponent->CopyBytesToLiveViewMemoryMirror(MipData, SizeInBytes, Stride, TexturePixelFormat);
						GDynamicRHI->RHIUnlockTexture2D(RHIRef, 0, false);
					}

					CoherentLiveViewComponent->UpdateLiveViewWithMemoryMirrorData();
				}
			}
		}
	);
}

#if defined(__CLANG__)
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wno-undefined-bool-conversion"
#endif

void UCoherentUILiveViewComponent::CopyBytesToLiveViewMemoryMirror(void* Source, int32 CountInBytes, uint32 Stride, EPixelFormat TexturePixelFormat)
{
	auto self = this;
	if (!self)
	{
		return;
	}

	const int32 LiveViewSizeInBytes = Width * Height * 4;
	verify(LiveViewSizeInBytes == ImageBytes.Num());
	if (LiveViewSizeInBytes < CountInBytes)
	{
		UE_LOG(LogCoherentUI, Warning, TEXT("Live view '%s' is smaller than the data about to be copied. Only part of the input data will be copied."), *LinkName);
	}
	else if (LiveViewSizeInBytes > CountInBytes)
	{
		UE_LOG(LogCoherentUI, Warning, TEXT("Live view '%s' is larger than the data about to be copied. This can result in uninitialized pixel data in the live view."), *LinkName);
	}

	switch (TexturePixelFormat)
	{
	case PF_R8G8B8A8:
		CountInBytes = FMath::Clamp(CountInBytes, 0, LiveViewSizeInBytes);
		FMemory::Memcpy(ImageBytes.GetData(), Source, CountInBytes);
		return;
	case PF_B8G8R8A8:
		{
			int32* SourceInt32 = reinterpret_cast<int32*>(Source);
			int32* TargetInt32 = reinterpret_cast<int32*>(ImageBytes.GetData());
			CountInBytes = FMath::Clamp(CountInBytes, 0, LiveViewSizeInBytes);
			for (int32 i = 0, Size = Width * Height; i < Size; ++i, ++SourceInt32, ++TargetInt32)
			{
				int32 Value = *SourceInt32;
				Value = (Value & 0xFF00FF00) | ((Value & 0xFF) << 16) | ((Value & 0x00FF0000) >> 16);
				*TargetInt32 = Value;
			}
		}
		return;
	default:
		UE_LOG(LogCoherentUI, Warning, TEXT("Unsupported texture pixel format for live view '%s'."), *LinkName);
		return;
	}
}

void UCoherentUILiveViewComponent::UpdateLiveViewWithMemoryMirrorData()
{
	auto self = this;

	if (!self || !CoherentBaseComponent.IsValid() || !CoherentBaseComponent->GetView())
	{
		return;
	}

	if (LiveViewImageData && ImageBytes.GetData())
	{
		LiveViewImageData->Update(ImageBytes.GetData(), false);
	}
}

void UCoherentUILiveViewComponent::OnNewImageDataSize(int32 NewWidth, int32 NewHeight)
{
	auto self = this;
	if (!self)
	{
		return;
	}

	Width = NewWidth;
	Height = NewHeight;
	bImageDataNeedsResize = true;
}

#if defined(__CLANG__)
	#pragma clang diagnostic pop
#endif