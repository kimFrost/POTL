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

#include "SCoherentSubscription.h"

#include "Coherent/Libraries/Logging/Declarations.h"
#include "Coherent/Libraries/Logging/ILogHandler.h"
#include "Coherent/UI/FileHandler.h"
#include "Coherent/UI/URLRequestBase.h"
#include "Coherent/UI/HTTPHeader.h"
#include "Coherent/UI/License.h"
#include "Coherent/UI/ViewInfo.h"
#include "Coherent/UI/ViewContext.h"

#if PLATFORM_LINUX || PLATFORM_MAC
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
#include "Coherent/UI/URLParse.h"
#pragma clang diagnostic pop
#else
#include "Coherent/UI/URLParse.h"
#endif

#if !defined(COHERENT_UI_UE4_4_5_SUPPORT)
#include "RHI.h"
#endif

extern FString g_CoUIPath;

class FCoherentUILogger : public Coherent::Logging::ILogHandler
{
public:
	virtual void WriteLog(Coherent::Logging::Severity sev, const char* msg, size_t) override
	{
		switch (sev)
		{
		case Coherent::Logging::Trace:
			UE_LOG(LogCoherentUI, Verbose, TEXT("%s"), UTF8_TO_TCHAR(msg));
			break;
		case Coherent::Logging::Debug:
			UE_LOG(LogCoherentUI, Log, TEXT("%s"), UTF8_TO_TCHAR(msg));
			break;
		case Coherent::Logging::Info:
			UE_LOG(LogCoherentUI, Display, TEXT("%s"), UTF8_TO_TCHAR(msg));
			break;
		case Coherent::Logging::Warning:
			UE_LOG(LogCoherentUI, Warning, TEXT("%s"), UTF8_TO_TCHAR(msg));
			break;
		case Coherent::Logging::AssertFailure:
			UE_LOG(LogCoherentUI, Error, TEXT("%s"), UTF8_TO_TCHAR(msg));
			break;
		case Coherent::Logging::Error:
			UE_LOG(LogCoherentUI, Error, TEXT("%s"), UTF8_TO_TCHAR(msg));
			break;
		}
	}

	virtual void Assert(const char* msg) override
	{
		UE_LOG(LogCoherentUI, Error, TEXT("%s"), UTF8_TO_TCHAR(msg));
	}
};

class FCoherentUISystemListener : public Coherent::UI::ContextListener
{
public:
	FCoherentUISystemListener()
		: IsReady(false)
	{}

	virtual ~FCoherentUISystemListener()
	{}

	virtual void ContextReady() override
	{
		IsReady = true;
	}

	virtual void OnError(const Coherent::UI::ContextError& error) override
	{
		UE_LOG(LogCoherentUI, Error, TEXT("System error: %s"), UTF8_TO_TCHAR(error.Error));
	}

	virtual void OnSubscriptionCheckDone(const Coherent::UI::SubscriptionError& error) override
	{
		switch (error.ErrorCode)
		{
		case Coherent::UI::SET_Success:
			// Everything OK. Do nothing.
			break;
		case Coherent::UI::SET_SuccessNotify:
			SCoherentSubscription::ShowActivationSuccessfulDialog();
			break;
		case Coherent::UI::SET_Error:
			SCoherentSubscription::ShowErrorDialog(error.Error);
			break;
		}
	}

	bool IsSystemReady() const
	{
		return IsReady;
	}

private:
	bool IsReady;
};

class FCoherentUIFileHandler : public Coherent::UI::FileHandler
{
public:
	FCoherentUIFileHandler()
		: GameRoot(FPaths::GameDir())
		, GameContent(FPaths::GameContentDir())
	{}

	bool TryReadFileIntoResponse(const FString& FilePath, Coherent::UI::ResourceResponse* Response, uint64 Start, uint64 End, uint64 UnboundedRequest, int RangeIndex)
	{
		using namespace Coherent::UI;

		TUniquePtr<FArchive> Reader(IFileManager::Get().CreateFileReader(*FilePath));
		if (!Reader)
		{
			return false;
		}

		const uint64 TotalSize = FMath::Max(0LL, Reader->TotalSize());
		verify(Start >= 0 && Start < TotalSize && "Bad range request start index!");
		verify(((End >= 0 && End < TotalSize) || End == UnboundedRequest) && "Bad range request end index");
		verify(((End >= Start) || End == UnboundedRequest) && "Bad range request range");

		if (End == UnboundedRequest)
		{
			End = TotalSize - 1;

			// Clamp range requests since new chunks will be requested when needed
			const uint64 MaxReponseSize = 32 * 1024 * 1024;
			if (RangeIndex != -1 && (End - Start > MaxReponseSize))
			{
				End = Start + MaxReponseSize;
			}
		}
		uint64 RequestedSize = End - Start + 1;

		void* OutBuffer = Response->GetBuffer((unsigned long)(RequestedSize));
		if (!OutBuffer)
		{ 
			UE_LOG(LogCoherentUI, Warning, TEXT("Could not allocate enough memory to load file: '%s'"), *FilePath);
			return false;
		}
		else
		{
			TArray<uint8> InternalBuffer;
			InternalBuffer.AddUninitialized(RequestedSize);
			Reader->Seek(Start);
			Reader->Serialize(InternalBuffer.GetData(), InternalBuffer.Num());

			FMemory::StreamingMemcpy(OutBuffer, InternalBuffer.GetData(), InternalBuffer.Num());

			if (RangeIndex != -1)
			{
				Response->SetResponseHeader(HTTPHeader::GetHeaderField(HTTPHeader::AcceptRanges), "bytes");

				FString ContentRange = FString::Printf(TEXT("bytes %llu-%llu/%llu"), Start, End, TotalSize);
				Response->SetResponseHeader(HTTPHeader::GetHeaderField(HTTPHeader::ContentRange), TCHAR_TO_ANSI(*ContentRange));

				FString ContentLength = TTypeToString<uint64>::ToSanitizedString(RequestedSize);
				Response->SetResponseHeader(HTTPHeader::GetHeaderField(HTTPHeader::ContentLength), TCHAR_TO_ANSI(*ContentLength));

				Response->SetStatus(206);
			}

			bool Success = Reader->Close();
			if (!Success)
			{
				UE_LOG(LogCoherentUI, Warning, TEXT("File contains errors: '%s'"), *FilePath);
			}
			return Success;
		}
	}

	virtual void ReadFile(const wchar_t* Url,
		Coherent::UI::URLRequestBase* Request,
		Coherent::UI::ResourceResponse* Response) override
	{
		using namespace Coherent::UI;

		URLComponent Path;
		if (!(CoherentGetURLParser().Parse(Url, nullptr, &Path, nullptr, nullptr) && Path.Start))
		{
			UE_LOG(LogCoherentUI, Warning, TEXT("Could not parse file path: '%s'."), Url);
			Response->SignalFailure();
			return;
		}

		// Range of bytes for response: [start, end] (both inclusive)
		uint64 Start = 0;
		const uint64 UnboundedRequest = uint64(-1);
		uint64 End = UnboundedRequest;

		int RangeIndex = Request->GetExtraHeaderIndex(HTTPHeader::GetHeaderField(HTTPHeader::Range));
		if (RangeIndex >= 0)
		{
			HTTPHeader RangeHeader = Request->GetExtraHeaders()[RangeIndex];
			FString RangeHeaderContent(RangeHeader.Content);
			FString RangePrefix("bytes=");

			if (RangeHeaderContent.StartsWith(RangePrefix))
			{
				int32 DashIndex = RangePrefix.Len();
				while (DashIndex < RangeHeaderContent.Len() && RangeHeaderContent[DashIndex] != '-')
				{
					++DashIndex;
				}
				if (RangeHeaderContent[DashIndex] == '-')
				{
					FString RangeFrom = RangeHeaderContent.Mid(RangePrefix.Len(), DashIndex - RangePrefix.Len());
					TTypeFromString<uint64>::FromString(Start, *RangeFrom);

					if (DashIndex + 1 < RangeHeaderContent.Len())
					{
						FString RangeTo = RangeHeaderContent.RightChop(DashIndex + 1);
						TTypeFromString<uint64>::FromString(End, *RangeTo);
					}
				}
			}
		}

		FString PathComponentString(int32(Path.Length), Path.Start);

		// Perform the actual read
		FString PathFromContent = GameContent + PathComponentString;

		if (!TryReadFileIntoResponse(PathFromContent, Response, Start, End, UnboundedRequest, RangeIndex))
		{
			FString PathFromRoot = GameRoot + PathComponentString;

			if (TryReadFileIntoResponse(PathFromRoot, Response, Start, End, UnboundedRequest, RangeIndex))
			{
				UE_LOG(LogCoherentUI, Warning, TEXT("[DEPRECATED] Loading files from the game's root folder is deprecated. Put your UI resources in Content\\. File: '%s'"), *PathFromRoot);
			}
			else
			{
				UE_LOG(LogCoherentUI, Warning, TEXT("Failed to read file '%s'."), *PathFromContent);
				Response->SignalFailure();
				return;
			}
		}
		Response->SignalSuccess();
	}

	virtual void WriteFile(const wchar_t* url,
		Coherent::UI::ResourceData* resource) override
	{
		using namespace Coherent::UI;

		URLComponent path;
		if (!(CoherentGetURLParser().Parse(url, nullptr, &path, nullptr, nullptr) && path.Start))
		{
			UE_LOG(LogCoherentUI, Warning, TEXT("Could not parse file path: '%s'."), url);
			resource->SignalFailure();
			return;
		}

		FString PathComponentString(int32(path.Length), path.Start);

		FString finalPath = GameContent + PathComponentString;
		const void* buffer = resource->GetBuffer();
		TArray<uint8> internalBuffer;
#if defined(COHERENT_UI_UE4_47_SUPPORT)
		internalBuffer.Init(resource->GetSize());
#else
		internalBuffer.SetNumUninitialized(resource->GetSize());
#endif
		FMemory::StreamingMemcpy(internalBuffer.GetData(), buffer, resource->GetSize());

		if (FFileHelper::SaveArrayToFile(internalBuffer, *finalPath))
		{
			resource->SignalSuccess();
		}
		else
		{
			UE_LOG(LogCoherentUI, Warning, TEXT("Failed to write file '%s'."), *finalPath);
			resource->SignalFailure();
		}
	}

private:
	FString GameRoot;
	FString GameContent;
};

class FCoherentUIFactoryHolder
{
public:
	FCoherentUIFactoryHolder()
		: Logger(new FCoherentUILogger)
		, Factory(nullptr)
	{}
	~FCoherentUIFactoryHolder()
	{
		if (Factory)
		{
			Factory->Destroy();
			Factory = nullptr;
		}
	}
	TUniquePtr<FCoherentUILogger> Logger;
	Coherent::UI::ViewContextFactory* Factory;
};

CoUISystemWrapper::ViewContextFactoryWeakPtr CoUISystemWrapper::s_WeakFactory;


SystemWrapperSharedPtr CoUISystemWrapper::CreateContext(
	Coherent::UI::FactorySettings& factorySettings,
	Coherent::UI::ContextSettings& settings,
	Coherent::Logging::Severity logSeverity)
{
	using namespace Coherent::UI;
	auto hostPath = g_CoUIPath + TEXT("host/");
	factorySettings.HostDirectory = *hostPath;

	SystemWrapperSharedPtr result(new CoUISystemWrapper());

	
	result->Listener.Reset(new FCoherentUISystemListener);
	result->FileHandler.Reset(new FCoherentUIFileHandler);
	
	RenderingParameters renderingParams;
	auto factoryHolder = s_WeakFactory.Pin();

	if (!factoryHolder.IsValid())
	{
		factoryHolder = ViewContextFactoryPtr(new FCoherentUIFactoryHolder);
		factoryHolder->Factory = InitializeCoherentUI(COHERENT_UI_SDK_VER,
		                                         COHERENT_KEY,
		                                         factorySettings,
		                                         renderingParams,
		                                         logSeverity,
												 factoryHolder->Logger.Get(),
		                                         nullptr);
		if (!factoryHolder->Factory)
		{
			return SystemWrapperSharedPtr();
		}
		s_WeakFactory = factoryHolder;
	}

	auto context = factoryHolder->Factory->CreateViewContext(settings,
	                                          result->Listener.Get(),
	                                          result->FileHandler.Get());

	if (!context)
	{
		return SystemWrapperSharedPtr();
	}

	result->Factory = factoryHolder;
	result->Context = context;

	return result;
}

void CoUISystemWrapper::DestroySystem(Coherent::UI::ViewContext* nakedContext)
{
	if (nakedContext)
	{
		nakedContext->Uninitialize();

		Listener.Reset();
		FileHandler.Reset();
	}
	Factory.Reset();
}

bool CoUISystemWrapper::IsReady()
{
	return Listener && Listener->IsSystemReady();
}

CoUISystemWrapper::CoUISystemWrapper()
	: Context(nullptr)
	, Factory(nullptr)
{
}

CoUISystemWrapper::~CoUISystemWrapper()
{
	DestroySystem(Context);
	Context = nullptr;
}

void CoUISystemWrapper::Update()
{
	Context->Update();
}

void CoUISystemWrapper::FetchSurfaces()
{
	Context->FetchSurfaces();
}

void CoUISystemWrapper::CreateView(UCoherentBaseComponent* owner, const Coherent::UI::ViewInfo& info, const wchar_t* path)
{
#if PLATFORM_WINDOWS
#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
	bool SharedTexturesSupport = !::IsOpenGLPlatform(GRHIShaderPlatform);
#else
	bool SharedTexturesSupport = !::IsOpenGLPlatform(GetFeatureLevelShaderPlatform(ERHIFeatureLevel::SM5));
#endif
#else
	bool SharedTexturesSupport = false;
#endif
	Coherent::UI::ViewInfo ActualViewInfo(info);
	if (!SharedTexturesSupport && !ActualViewInfo.UsesSharedMemory)
	{
		UE_LOG(LogCoherentUI, Error, TEXT("Shared textures are only supported on Windows using DirectX renderer! Falling back to shared memory."));
		ActualViewInfo.UsesSharedMemory = true;
	}

	Context->CreateView(ActualViewInfo, path, owner);
	owner->ViewRequested();
	RegisterComponent(owner);
}

void CoUISystemWrapper::UnregisterComponent(UCoherentBaseComponent* component)
{
	WeakCoherentComponents.Remove(component);
}


void CoUISystemWrapper::RegisterComponent(UCoherentBaseComponent* component)
{
	if (WeakCoherentComponents.Find(component) == INDEX_NONE)
	{
		WeakCoherentComponents.Add(component);
	}
}

CoUISystemWrapper::WeakCoherentComponentsArray::TIterator CoUISystemWrapper::GetCoherentComponentsIterator()
{
	return CoUISystemWrapper::WeakCoherentComponentsArray::TIterator(WeakCoherentComponents);
}
