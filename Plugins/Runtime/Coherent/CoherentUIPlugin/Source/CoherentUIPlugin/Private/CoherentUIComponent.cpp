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

#include "CoherentUIComponent.h"
#include "CoherentFStringBinder.h"
#include "CoherentUObjectBinder.h"
#include "Coherent/UI/ViewContext.h"
#include "Coherent/UI/View.h"
#include "Coherent/UI/ViewInfo.h"
#include "Coherent/UI/MediaStream.h"
#include "Coherent/UI/Binding/EventHandler.h"

#include "CoherentCallFunction.h"

#define UI_TEXTURE_NAME TEXT("UITexture")

void FCoherentComponentTickFunction::ExecuteTick(float DeltaTime, enum ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	if ((TickType == LEVELTICK_All || TickType == LEVELTICK_PauseTick) && Target && !Target->HasAnyFlags(RF_PendingKill | RF_Unreachable))
	{
		FScopeCycleCounterUObject ComponentScope(Target);
		FScopeCycleCounterUObject AdditionalScope(Target->AdditionalStatObject());
		Target->TickCustom(this);
	}
}

FString FCoherentComponentTickFunction::DiagnosticMessage()
{
	return Target->GetFullName() + TEXT("[UCoherentUI::CoherentComponentTickFunction]");
}

UCoherentBaseComponent::UCoherentBaseComponent(
#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
	const FPostConstructInitializeProperties& PCIP)
#else
	const FObjectInitializer& PCIP)
#endif
	: Super(PCIP)
	, bReceiveInput(true)
	, bIsReadyForBindings(false)
	, bIsReadyForScripting(false)
	, bIsReadyForFinishDestroy(true)
{
	bWantsInitializeComponent = true;
	bAutoActivate = true;

	PostPhysicsComponentTick.bCanEverTick = true;
	PostPhysicsComponentTick.bStartWithTickEnabled = true;
	PostPhysicsComponentTick.bTickEvenWhenPaused = true;
	PostPhysicsComponentTick.TickGroup = TG_PostPhysics;
}

void UCoherentBaseComponent::InitializeComponent()
{
	Super::InitializeComponent();
	bIsReadyForBindings = false;
	bIsReadyForFinishDestroy = true;
}

void UCoherentBaseComponent::TickCustom(FCoherentComponentTickFunction* function)
{}

void UCoherentBaseComponent::OnRegister()
{
	if (System.IsValid() && System->IsReady())
	{
		System->RegisterComponent(this);
	}
	Super::OnRegister();
}

void UCoherentBaseComponent::OnUnregister()
{
	if (System.IsValid() && System->IsReady())
	{
		System->UnregisterComponent(this);
	}
	Super::OnUnregister();
}

void UCoherentBaseComponent::OnComponentDestroyed()
{
	Destroy();
}

void UCoherentBaseComponent::BeginDestroy()
{
	Super::BeginDestroy();
	Destroy();
	if (System.Get())
	{
		System.Reset();
	}
}

bool UCoherentBaseComponent::IsReadyForFinishDestroy()
{

	return bIsReadyForFinishDestroy;
}

void UCoherentBaseComponent::RegisterTickFunctions(bool bRegister)
{
	if (bRegister)
	{
		if (SetupActorComponentTickFunction(&PostPhysicsComponentTick))
		{
			PostPhysicsComponentTick.Target = this;
		}
	}
	else
	{
		if (PostPhysicsComponentTick.IsTickFunctionRegistered())
		{
			PostPhysicsComponentTick.UnRegisterTickFunction();
		}
	}
}

void UCoherentBaseComponent::OnViewCreated(Coherent::UI::View* View)
{
	FCoherentUIViewListener::OnViewCreated(View);

	UIOnViewCreated.Broadcast();
}

void UCoherentBaseComponent::OnScriptingReady()
{
	bIsReadyForScripting = true;
	UIScriptingReady.Broadcast();
}

void UCoherentBaseComponent::ViewRequested()
{
	bIsReadyForFinishDestroy = false;
}

void UCoherentBaseComponent::Release()
{
	FCoherentUIViewListener::Release();
	bIsReadyForFinishDestroy = true;
}


void UCoherentBaseComponent::OnReadyForBindings(int frameId,
	const wchar_t* path,
	bool isMainFrame)
{
	bIsReadyForBindings = true;

	if (isMainFrame)
	{
		GetView()->BindCall("UIScriptingReady", Coherent::UI::MakeHandler(this, &UCoherentBaseComponent::OnScriptingReady));
	}

	ReadyForBindings.Broadcast(frameId, path, isMainFrame);
}

void UCoherentBaseComponent::OnBindingsReleased(int frameId,
	const wchar_t* path,
	bool isMainFrame)
{
	BindingsReleased.Broadcast(frameId, path, isMainFrame);
}

void UCoherentBaseComponent::OnFinishLoad(int frameId,
	const wchar_t* validatedPath,
	bool isMainFrame,
	int statusCode,
	Coherent::UI::HTTPHeader* headers,
	unsigned headersCount)
{
	FinishLoad.Broadcast(frameId, validatedPath, isMainFrame, statusCode);
}

void UCoherentBaseComponent::OnFailLoad(int frameId, const wchar_t* validatedPath, bool isMainFrame, const char* error)
{
	FailLoad.Broadcast(frameId, validatedPath, isMainFrame, UTF8_TO_TCHAR(error));
}

void UCoherentBaseComponent::OnNavigateTo(const wchar_t* path)
{
	OnNavigate.Broadcast(path);
}

void UCoherentBaseComponent::OnScriptMessage(MessageLevel level, const wchar_t* message, const wchar_t* sourceId, int line)
{
	ScriptMessage.Broadcast(message, sourceId, line);
}

void UCoherentBaseComponent::OnCursorChanged(Coherent::UI::CursorTypes cursor)
{
	CursorChanged.Broadcast((int32)cursor);
}

void UCoherentBaseComponent::OnJavaScriptMessage(const wchar_t* message, const wchar_t* defaultPrompt, const wchar_t* frameUrl, int messageType)
{
	JavaScriptMessage.Broadcast(message, defaultPrompt, frameUrl, messageType);
}

void UCoherentBaseComponent::OnError(const Coherent::UI::ViewError& error)
{
	FCoherentUIViewListener::OnError(error);
	ErrorOccured.Broadcast(UTF8_TO_TCHAR(error.Error));
}

void UCoherentBaseComponent::OnCallback(const char* eventName, const Coherent::UI::EventArguments& arguments)
{
	auto payload = NewObject<UCoherentUIJSPayload>();
	payload->EventName = UTF8_TO_TCHAR(eventName);
	payload->SetArguments(&arguments);

	JavaScriptEvent.Broadcast(payload);

	{
		auto Owner = GetOwner();
		auto Function = Owner->FindFunction(FName(eventName));
		if (Function)
		{
			Coherent::UI::CallFunction(Owner, Function, arguments);
		}
	}
}

void UCoherentBaseComponent::Resize(int32 width, int32 height)
{
	DoResize(width, height);
}

void UCoherentBaseComponent::IssueMouseOnUIQuery(float normX, float normY)
{
	auto view = GetView();
	if (!view) return;

	if (view->HasMouseQueryFinished() == Coherent::UI::VE_InvalidCall)
	{
		view->IssueMouseOnUIQuery(normX, normY);
	}
}

bool UCoherentBaseComponent::HasMouseQueryFinished()
{
	auto view = GetView();
	if (!view) return false;

	return view->HasMouseQueryFinished() == Coherent::UI::VE_Success;
}

void UCoherentBaseComponent::FetchMouseOnUIQuery()
{
	UE_LOG(LogCoherentUI, Warning, TEXT("[DEPRECATED] UCoherentBaseComponent::FetchMouseOnUIQuery is deprecated"))
}

bool UCoherentBaseComponent::IsMouseOnView()
{
	auto view = GetView();
	if (!view) return false;

	if (HasMouseQueryFinished())
	{
		view->FetchMouseOnUIQuery();
		bIsMouseOnUI = view->IsMouseOnView();
	}

	return bIsMouseOnUI;
}

void UCoherentBaseComponent::SetClickThroughAlphaThreshold(float threshold)
{
	auto view = GetView();
	if (!view) return;

	view->SetClickThroughAlphaThreshold(threshold);
}

float UCoherentBaseComponent::GetClickThroughAlphaThreshold() const
{
	auto view = GetView();
	if (!view) return 0;

	return view->GetClickThroughAlphaThreshold();
}

void UCoherentBaseComponent::SetTargetFramerate(int32 target)
{
	auto view = GetView();
	if (!view) return;

	view->SetTargetFramerate(target);
}

int32 UCoherentBaseComponent::GetTargetFramerate() const
{
	auto view = GetView();
	if (!view) return 0;

	return view->GetTargetFramerate();
}

bool UCoherentBaseComponent::IsViewOnDemand() const
{
	auto view = GetView();
	if (!view) return false;

	return view->IsOnDemand();
}

bool UCoherentBaseComponent::IsTransparent() const
{
	auto view = GetView();
	if (!view) return false;

	return view->IsTransparent();
}

void UCoherentBaseComponent::Redraw() const
{
	auto view = GetView();
	if (!view) return;

	view->Redraw();
}

void UCoherentBaseComponent::Load(const FString& path)
{
	auto view = GetView();
	if (!view) return;

	view->Load(*path);
}

void UCoherentBaseComponent::Reload(bool ignoreCache)
{
	auto view = GetView();
	if (!view) return;

	view->Reload(ignoreCache);
}

FString UCoherentBaseComponent::GetCurentViewPath() const
{
	auto view = GetView();
	if (!view) return L"";

	return view->GetCurentViewPath();
}

FString UCoherentBaseComponent::GetLastRequestedPath() const
{
	auto view = GetView();
	if (!view) return L"";

	return view->GetLastRequestedPath();
}

FString UCoherentBaseComponent::GetLastLoadedPath() const
{
	auto view = GetView();
	if (!view) return L"";

	return view->GetLastLoadedPath();
}

FString UCoherentBaseComponent::GetLastFailedPath() const
{
	auto view = GetView();
	if (!view) return L"";

	return view->GetLastFailedPath();
}

UTexture2D* UCoherentBaseComponent::GetTexture() const
{
	return Texture;
}

UCoherentUIJSEvent* UCoherentBaseComponent::CreateJSEvent()
{
	return NewObject<UCoherentUIJSEvent>();
}

void UCoherentBaseComponent::TriggerJSEvent(const FString& name, UCoherentUIJSEvent* eventData) const
{
	if (!bIsReadyForBindings)
		return;

	if (!eventData || eventData->OrderedVariables.Num() == 0)
	{
		GetView()->TriggerEvent(TCHAR_TO_UTF8(*name));
		return;
	}
	
	Coherent::UI::ScriptEventWriter* writer = GetScriptEventWriter(GetView());
	auto binder = writer->GetBinder();
	writer->BeginEvent(TCHAR_TO_UTF8(*name), eventData->OrderedVariables.Num());
	
	for (auto i = 0; i < eventData->OrderedVariables.Num(); ++i)
	{
		const auto& pair = eventData->OrderedVariables[i];

		switch (pair.Type)
		{
		case UCoherentUIJSEvent::VT_Int32:
			CoherentBindInternal(binder, eventData->Ints[pair.Id]);
			break;
		case UCoherentUIJSEvent::VT_String:
			CoherentBindInternal(binder, eventData->Strings[pair.Id]);
			break;
		case UCoherentUIJSEvent::VT_Object:
			CoherentBindInternal(binder, eventData->Objects[pair.Id].Get());
			break;
		case UCoherentUIJSEvent::VT_UInt32:
			CoherentBindInternal(binder, eventData->UInts[pair.Id]);
			break;
		case UCoherentUIJSEvent::VT_Float:
			CoherentBindInternal(binder, eventData->Floats[pair.Id]);
			break;
		case UCoherentUIJSEvent::VT_Bool:
			CoherentBindInternal(binder, eventData->Bools[pair.Id]);
			break;
		default:
			UE_LOG(LogCoherentUI, Warning, TEXT("Triggering JS event with unknown type!"));
			break;
		}
	}
	writer->EndEvent();
	eventData->Clear();
}

bool UCoherentBaseComponent::IsReadyForBindings() const
{
	return bIsReadyForBindings;
}

bool UCoherentBaseComponent::IsReadyForScripting() const
{
	return bIsReadyForScripting;
}

namespace Coherent
{
namespace UI
{

template<typename Prop, typename Ctype>
bool BindProperty(UProperty* Property, Coherent::UI::Binder* binder, UObject* object)
{
	Prop* prop = Cast<Prop>(Property);
	if (prop)
	{
		binder->PropertyName(TCHAR_TO_ANSI(*prop->GetName()));
		auto value = prop->GetPropertyValue(prop->template ContainerPtrToValuePtr<Ctype>(object));

		CoherentBindInternal(binder, value);
		return true;
	}
	return false;
}

void CoherentBindInternal(Binder* binder, UObject* object)
{
	if (!object)
		return;

	auto cl = object->GetClass();

	binder->OnTypeBegin();
	binder->TypeName(TCHAR_TO_ANSI(*cl->GetName()));

	for (TFieldIterator<UProperty> PropertyIt(cl, EFieldIteratorFlags::IncludeSuper);
		PropertyIt;
		++PropertyIt)
	{
		UProperty* Property = *PropertyIt;
		if (BindProperty<UUInt32Property, uint32>(Property, binder, object))
			continue;
		if (BindProperty<UIntProperty, int32>(Property, binder, object))
			continue;
		if (BindProperty<UBoolProperty, bool>(Property, binder, object))
			continue;
		if (BindProperty<UFloatProperty, float>(Property, binder, object))
			continue;
		if (BindProperty<UDoubleProperty, double>(Property, binder, object))
			continue;
		if (BindProperty<UStrProperty, FString>(Property, binder, object))
			continue;
	}

	binder->OnTypeEnd();
}

}
}

///////////////////////////////////////////////////////////////////////////////

UCoherentUIComponent::UCoherentUIComponent(
#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
	const FPostConstructInitializeProperties& PCIP)
#else
	const FObjectInitializer& PCIP)
#endif
	: Super(PCIP)
	, bDidTryToCreateView(false)
	, bIsLoadingMeshData(false)
	, bHasMeshData(false)
{
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bTickEvenWhenPaused = true;

	URL = TEXT("http://google.com");
	Width = 1024;
	Height = 1024;
	Transparent = true;
	SupportClickThrough = false;
	MaxFPS = 30;
	ForceSoftwareRendering = false;
	IsOnDemand = false;
	ManualTexture = false;
}

void UCoherentUIComponent::InitializeComponent()
{
	Super::InitializeComponent();
	CreateSystemIfNecessary(GetWorld());
	bIsLoadingMeshData = false;
	bHasMeshData = false;
}

void UCoherentUIComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (!bDidTryToCreateView && IsReadyToCreateView())
	{
		bDidTryToCreateView = true;

		Coherent::UI::ViewInfo info;
		info.Width = Width;
		info.Height = Height;
		info.TargetFrameRate = MaxFPS;
		info.UsesSharedMemory = UseSharedMemory || ForceSoftwareRendering;
		info.IsTransparent = Transparent;
		info.SupportClickThrough = SupportClickThrough;
		info.ForceSoftwareRendering = ForceSoftwareRendering;
		info.IsOnDemand = IsOnDemand;

		CreateView(info, *URL);
	}

	TickListener();
}

void UCoherentUIComponent::OnViewCreated(Coherent::UI::View* view)
{
	FCoherentUIViewListener::OnViewCreated(view);

	if (!ManualTexture)
	{
		SetupUITextures();
	}

	UIOnViewCreated.Broadcast();
}

namespace
{
	class FUpdateCoherentUITextureTask
	{
		TWeakObjectPtr<UCoherentBaseComponent> CoherentComponent;

	public:
		FUpdateCoherentUITextureTask(TWeakObjectPtr<UCoherentBaseComponent> Component)
			: CoherentComponent(Component)
		{}

		FORCEINLINE TStatId GetStatId() const
		{
			RETURN_QUICK_DECLARE_CYCLE_STAT(FUpdateCoherentUITextureTask, STATGROUP_TaskGraphTasks);
		}
		static ENamedThreads::Type GetDesiredThread()
		{
			return ENamedThreads::GameThread;
		}
		static ESubsequentsMode::Type GetSubsequentsMode()
		{
			return ESubsequentsMode::FireAndForget;
		}

		void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
		{
			if (CoherentComponent.IsValid() && CoherentComponent->GetTexture())
			{
				CoherentComponent->GetTexture()->UpdateResource();
			}
		}
	};

	UMaterialInstanceDynamic* EnsureMaterialInstanceDynamic(UMaterialInterface* Material, UPrimitiveComponent* Primitive, int32 index)
	{
		auto MaterialInstance = Cast<UMaterialInstanceDynamic>(Material);
		if (!MaterialInstance)
		{
			MaterialInstance = UMaterialInstanceDynamic::Create(Material, Primitive);
			Primitive->SetMaterial(index, MaterialInstance);
		}
		return MaterialInstance;
	}

	template <typename PropType>
	void VerifyTexturePropertyIsDefault(const FString& Name, const PropType& Source, const PropType& Default)
	{
		if (Source != Default)
		{
			UE_LOG(LogCoherentUI, Warning, TEXT("Editor-only texture property \"%s\" cannot be applied to Coherent UI texture!"), *Name);
		}
	}

	void CopyTextureProperties(UTexture* Source, UTexture* Destination)
	{
		if (!Source || !Destination)
		{
			UE_LOG(LogCoherentUI, Warning, TEXT("Skipping texture properties copying because source or destination texture is null!"));
			return;
		}

#if WITH_EDITORONLY_DATA
		UTexture* DefaultTextureObject = UTexture::StaticClass()->GetDefaultObject<UTexture>();
		VerifyTexturePropertyIsDefault(TEXT("AdjustBrightness")     , Source->AdjustBrightness     , DefaultTextureObject->AdjustBrightness);
		VerifyTexturePropertyIsDefault(TEXT("AdjustBrightnessCurve"), Source->AdjustBrightnessCurve, DefaultTextureObject->AdjustBrightnessCurve);
		VerifyTexturePropertyIsDefault(TEXT("AdjustVibrance")       , Source->AdjustVibrance       , DefaultTextureObject->AdjustVibrance);
		VerifyTexturePropertyIsDefault(TEXT("AdjustSaturation")     , Source->AdjustSaturation     , DefaultTextureObject->AdjustSaturation);
		VerifyTexturePropertyIsDefault(TEXT("AdjustRGBCurve")       , Source->AdjustRGBCurve       , DefaultTextureObject->AdjustRGBCurve);
		VerifyTexturePropertyIsDefault(TEXT("AdjustHue")            , Source->AdjustHue            , DefaultTextureObject->AdjustHue);
		VerifyTexturePropertyIsDefault(TEXT("AdjustMinAlpha")       , Source->AdjustMinAlpha       , DefaultTextureObject->AdjustMinAlpha);
		VerifyTexturePropertyIsDefault(TEXT("AdjustMaxAlpha")       , Source->AdjustMaxAlpha       , DefaultTextureObject->AdjustMaxAlpha);
		VerifyTexturePropertyIsDefault(TEXT("bDitherMipMapAlpha")   , Source->bDitherMipMapAlpha   , DefaultTextureObject->bDitherMipMapAlpha);
		VerifyTexturePropertyIsDefault(TEXT("bPreserveBorder")      , Source->bPreserveBorder      , DefaultTextureObject->bPreserveBorder);
		VerifyTexturePropertyIsDefault(TEXT("bFlipGreenChannel")    , Source->bFlipGreenChannel    , DefaultTextureObject->bFlipGreenChannel);
		VerifyTexturePropertyIsDefault(TEXT("MipGenSettings")       , Source->MipGenSettings       , DefaultTextureObject->MipGenSettings);
		VerifyTexturePropertyIsDefault(TEXT("CompositeTexture")     , Source->CompositeTexture     , DefaultTextureObject->CompositeTexture);
		VerifyTexturePropertyIsDefault(TEXT("CompositeTextureMode") , Source->CompositeTextureMode , DefaultTextureObject->CompositeTextureMode);
		VerifyTexturePropertyIsDefault(TEXT("CompositePower")       , Source->CompositePower       , DefaultTextureObject->CompositePower);
		VerifyTexturePropertyIsDefault(TEXT("CompressionNoAlpha")   , Source->CompressionNoAlpha   , DefaultTextureObject->CompressionNoAlpha);
		VerifyTexturePropertyIsDefault(TEXT("DeferCompression")     , Source->DeferCompression     , DefaultTextureObject->DeferCompression);
#endif // WITH_EDITORONLY_DATA

		Destination->LODBias               = Source->LODBias;
		Destination->NumCinematicMipLevels = Source->NumCinematicMipLevels;
		Destination->SRGB                  = Source->SRGB;
		Destination->NeverStream           = Source->NeverStream;
		Destination->CompressionSettings   = Source->CompressionSettings;
		Destination->Filter                = Source->Filter;
		Destination->LODGroup              = Source->LODGroup;

		UTexture2D* Source2D = Cast<UTexture2D>(Source);
		UTexture2D* Destination2D = Cast<UTexture2D>(Destination);
		if (Source2D && Destination2D)
		{
			Destination2D->AddressX = Source2D->AddressX;
			Destination2D->AddressY = Source2D->AddressY;
		}
	}
}

void UCoherentUIComponent::SetupUITextures() const
{
	auto Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	TArray<UPrimitiveComponent*> Primitives;
	Owner->GetComponents<UPrimitiveComponent>(Primitives);
	auto PrimitivesCount = Primitives.Num();
	for (auto i = 0; i != PrimitivesCount; ++i)
	{
		auto Primitive = Primitives[i];
		auto MaterialsCount = Primitive->GetNumMaterials();
		for (auto m = 0; m != MaterialsCount; ++m)
		{
			auto Material = Primitive->GetMaterial(m);
			UTexture* Texture = nullptr;
			if (!Material)
			{
				UE_LOG(LogCoherentUI, Error, TEXT("Coherent UI Component requires the primitives in the UPrimitiveComponent to have a material assigned!"));
				continue;
			}

			if (Material->GetTextureParameterValue(UI_TEXTURE_NAME, Texture))
			{
				UTexture2D* CoherentTexture = GetTexture();

				CopyTextureProperties(Texture, CoherentTexture);
				// Calling CoherentTexture->UpdateResource(); here seems to cause a deadlock in shipping builds so we defer it
				TGraphTask<FUpdateCoherentUITextureTask>::CreateTask(nullptr).ConstructAndDispatchWhenReady(this);
				CoherentTexture->RefreshSamplerStates();

				auto MaterialInstance = EnsureMaterialInstanceDynamic(Material, Primitive, m);
				MaterialInstance->SetTextureParameterValue(UI_TEXTURE_NAME, CoherentTexture);

				MaterialInstance->RecacheUniformExpressions();
			}
		}
	}
}

void UCoherentUIComponent::CreateView(const Coherent::UI::ViewInfo& info, const FString& url)
{
	check(System.IsValid());
	System->CreateView(this, info, *url);

	if (!IsOnDemand)
	{
		if (PostPhysicsComponentTick.IsTickFunctionRegistered())
		{
			PostPhysicsComponentTick.SetTickFunctionEnable(false);
		}
	}
}

void UCoherentUIComponent::RegisterComponentTickFunctions(bool bRegister)
{
	Super::RegisterComponentTickFunctions(bRegister);

	if (!IsOnDemand)
	{
		return;
	}

	RegisterTickFunctions(bRegister);
}

void UCoherentUIComponent::TickCustom(FCoherentComponentTickFunction* function)
{
	if (!IsOnDemand || !GetView())
	{
		return;
	}

	if (function == &PostPhysicsComponentTick)
	{
		GetView()->RequestFrame();
		ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
			CoherentFetchSurface,
			TWeakObjectPtr<UCoherentBaseComponent>, CoherentComponent, this,
			{
				if (CoherentComponent.IsValid())
				{
					Coherent::UI::View* View = CoherentComponent->GetView();
					if (View)
					{
						View->FetchSurface();
					}
				}
			}
		);
	}
}

UCoherentBaseComponent::WeakCoherentComponentsArray::TIterator UCoherentBaseComponent::GetUCoherentBaseComponentIterator(UWorld* world)
{
	for (TActorIterator<ACoherentUISystem> It(world); It; ++It)
	{
		auto System = (*It)->GetSystemObject();
		if (System.IsValid())
		{
			return System->GetCoherentComponentsIterator();
		}
	}

	static WeakCoherentComponentsArray EmptyComponentsArray;
	return EmptyComponentsArray.CreateIterator();
}

void UCoherentBaseComponent::OnRequestMediaStream(Coherent::UI::MediaStreamRequest* request)
{
	auto handler = GetMediaRequestHandler();
	handler->Handle(request);
}

UCoherentMediaRequestHandler* UCoherentBaseComponent::GetMediaRequestHandler()
{
	if (!m_MediaRequestHandler)
	{
		m_MediaRequestHandler = Cast<UCoherentMediaRequestHandler>(CoherentUINewObjectSimple<UCoherentMediaRequestHandler>(UCoherentMediaRequestHandler::StaticClass()));
	}
	return m_MediaRequestHandler;
}

namespace
{
bool HasCoherentUIView(UStaticMeshComponent* Mesh)
{
	auto MaterialsCount = Mesh->GetNumMaterials();
	for (auto m = 0; m != MaterialsCount; ++m)
	{
		auto Material = Mesh->GetMaterial(m);
		UTexture* Texture = nullptr;

		if (Material && Material->GetTextureParameterValue(UI_TEXTURE_NAME, Texture))
		{
			return true;
		}
	}
	return false;
}
}

bool UCoherentUIComponent::EnsureMeshData()
{
	auto Owner = GetOwner();
	if (!bHasMeshData && Owner && !bIsLoadingMeshData)
	{
		bIsLoadingMeshData = true;
		TArray<UStaticMeshComponent*> StaticMeshes;
		Owner->GetComponents<UStaticMeshComponent>(StaticMeshes);

		TArray<TWeakObjectPtr<UStaticMeshComponent>> WeakMeshes;

		auto NumMeshes = StaticMeshes.Num();
		for (auto Index = 0u; Index != NumMeshes; ++Index)
		{
			if (HasCoherentUIView(StaticMeshes[Index]))
			{
				WeakMeshes.Add(StaticMeshes[Index]);
			}
		}

		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(CoherentUILoadMeshData,
			TWeakObjectPtr<UCoherentUIComponent>, ViewComponent, this,
			TArray<TWeakObjectPtr<UStaticMeshComponent>>, Meshes, WeakMeshes,
			{
				if (ViewComponent.IsValid())
				{
					ViewComponent->LoadMeshData(Meshes);
				}
			});
	}
	return bHasMeshData;
}

void UCoherentUIComponent::LoadMeshData(const TArray<TWeakObjectPtr<UStaticMeshComponent>>& Meshes)
{
	auto NumMeshes = Meshes.Num();
	MeshData.Reserve(NumMeshes);
	for (auto Index = 0u; Index != NumMeshes; ++Index)
	{
		if (Meshes[Index].IsValid())
		{
			MeshData.Add(::LoadMeshData(Meshes[Index].Get()));
		}
	}
	bHasMeshData = true;
}
