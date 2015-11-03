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
#include "CoherentUIPluginPrivatePCH.h"

#include "CoherentMediaRequestHandler.h"
#include "Coherent/UI/MediaStream.h"

UCoherentMediaRequestHandler::UCoherentMediaRequestHandler(
#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
	const FPostConstructInitializeProperties& PCIP)
#else
	const FObjectInitializer& PCIP)
#endif
	: Super(PCIP)
	, m_SelectedAudio(0)
	, m_SelectedVideo(0)
{
}

void UCoherentMediaRequestHandler::SelectAudioStream(int32 deviceIndex)
{
	m_SelectedAudio = deviceIndex;
}

void UCoherentMediaRequestHandler::SelectVideoStream(int32 deviceIndex)
{
	m_SelectedVideo = deviceIndex;
}

namespace
{
TArray<FMediaDevice> GetDevices(Coherent::UI::MediaStreamDevice* devices, unsigned int count, Coherent::UI::MediaStreamType type)
{
	TArray<FMediaDevice> result;
	result.Reserve(count);

	for (auto i = 0u; i != count; ++i)
	{
		FMediaDevice device;
		device.Name = devices[i].Name;
		device.Id = devices[i].DeviceId;

		result.Add(device);
	}

	return result;
}

unsigned GetVideoDeviceOffset(Coherent::UI::MediaStreamDevice* devices, unsigned int count)
{
	auto offset = 0u;
	while (offset != count && devices[offset].Type != Coherent::UI::MST_DEVICE_VIDEO_CAPTURE)
	{
		++offset;
	}
	return offset;
}
}

void UCoherentMediaRequestHandler::Handle(Coherent::UI::MediaStreamRequest* request)
{
	auto devicesCount = request->GetDevicesCount();
	auto devices = request->GetDevices();

	if (AudioStreamRequest.IsBound())
	{
		AudioStreamRequest.Broadcast(GetDevices(devices, devicesCount, Coherent::UI::MST_DEVICE_AUDIO_CAPTURE));
	}

	if (VideoStreamRequest.IsBound())
	{
		VideoStreamRequest.Broadcast(GetDevices(devices, devicesCount, Coherent::UI::MST_DEVICE_VIDEO_CAPTURE));
	}

	unsigned response[2] = {0};
	unsigned count = 0;
	auto videoOffset = GetVideoDeviceOffset(devices, devicesCount);
	unsigned audioDevice = m_SelectedAudio;
	if (audioDevice < videoOffset)
	{
		response[count++] = audioDevice;
	}

	auto videoDevice = videoOffset + m_SelectedVideo;
	if (videoDevice < devicesCount)
	{
		response[count++] = videoDevice;
	}

	request->Respond(response, count);	
}
