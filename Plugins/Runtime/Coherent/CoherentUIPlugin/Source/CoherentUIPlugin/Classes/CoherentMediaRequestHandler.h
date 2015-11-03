/*
This file is part of Coherent UI, modern user interface library for
games.

Copyright (c) 2012-2014 Coherent Labs Limited and/or its licensors. All
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

#include "CoherentUIViewListener.h"
#include "CoherentMediaRequestHandler.generated.h"

/**
* A media device on the system
*/
USTRUCT()
struct COHERENTUIPLUGIN_API FMediaDevice
{
	GENERATED_USTRUCT_BODY()
public:
	/** the human readable name of the device */
	UPROPERTY()
	FString Name;

	/** the device ID */
	UPROPERTY()
	FString Id;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUIMediaStreamRequest, const TArray<FMediaDevice>&, Devices);

/** Handler for user media requests
*
* Exposes events for choosing input audio and video stream.
*/
UCLASS(meta = (BlueprintSpawnableComponent), MinimalAPI, Blueprintable)
class UCoherentMediaRequestHandler : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	/** Fired when an audio stream was requests with the list of audio devices */
	UPROPERTY(BlueprintAssignable)
	FUIMediaStreamRequest AudioStreamRequest;

	/**
	* Choose an audio stream
	* @param index the index in the array given to AudioStreamRequest of the device to use.
	*/
	UFUNCTION(BlueprintCallable, Category = "Response")
	void SelectAudioStream(int32 Index);

	/** Fired when an audio stream was requests with the list of video devices */
	UPROPERTY(BlueprintAssignable)
	FUIMediaStreamRequest VideoStreamRequest;

	/**
	* Choose a video stream
	* @param index the index in the array given to VideoStreamRequest of the device to use.
	*/
	UFUNCTION(BlueprintCallable, Category = "Response")
	void SelectVideoStream(int32 Index);

	void Handle(Coherent::UI::MediaStreamRequest* request);

private:
	int32 m_SelectedAudio;
	int32 m_SelectedVideo;
};
