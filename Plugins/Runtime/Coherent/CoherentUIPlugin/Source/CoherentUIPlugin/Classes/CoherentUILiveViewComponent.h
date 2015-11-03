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

#include "CoherentBaseComponent.h"
#include "CoherentUILiveViewComponent.generated.h"

namespace Coherent
{
namespace UI
{
class ImageData;
}
}

/**
* Coherent UI Live View Component
*
* The class exposes all functionality needed for live views usage in Coherent UI.
* The component is complementary to the base Coherent UI Component.
*/
UCLASS(ClassGroup = UI, hidecategories = Object, editinlinenew, meta = (BlueprintSpawnableComponent, DisplayName = "Coherent UI Live View"))
class COHERENTUIPLUGIN_API UCoherentUILiveViewComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()
public:
	/** The unique identifier of the live view link */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Live View")
	FString LinkName;

	/** Indicates whether the live view will be updated every frame */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Live View")
	bool UpdateEveryFrame;

	/** The texture whose data will be transferred to Coherent UI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Live View")
	UTexture* Texture;

	/** If false, the live view will not be updated when the game is paused. The update only works
	with the global setting of the Coherent UI System UpdateWhenPaused set to true*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Live View")
	bool UpdateWhenPaused;

	/** Manual update of the live view texture */
	UFUNCTION(BlueprintCallable, Category = "Live View")
	void QueueUpdateLiveViewOnRenderThread();

public:
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void OnUnregister() override;

	void Destroy();

	void CopyBytesToLiveViewMemoryMirror(void* Source, int32 CountInBytes, uint32 Stride, EPixelFormat TexturePixelFormat);
	void UpdateLiveViewWithMemoryMirrorData();
	void OnNewImageDataSize(int32 NewWidth, int32 NewHeight);

private:
	UFUNCTION()
	void OnReadyForBindingsHandler(int32 FrameID, const FString& Path, bool IsMainFrame);
	UFUNCTION()
	void OnBindingsReleasedHandler(int32 FrameID, const FString& Path, bool IsMainFrame);

	void DestroyImageData();
	void RecreateImageData();

private:
	Coherent::UI::ImageData* LiveViewImageData;
	TWeakObjectPtr<UCoherentBaseComponent> CoherentBaseComponent;
	TArray<uint8> ImageBytes;
	bool bLiveViewImageDataReady;
	bool bBindingsReady;
	bool bImageDataNeedsResize;
	int32 Width;
	int32 Height;
};
