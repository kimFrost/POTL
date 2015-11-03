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

#include "SCoherentInputForward.h"
#include "CoherentUIInputBindings.h"
#include "CoherentUIComponent.h"
#include "CoherentUIHUD.h"
#include "Coherent/UI/View.h"

#include "Engine/RendererSettings.h"

#include "StaticMeshResources.h"

SCoherentInputForward::~SCoherentInputForward()
{}

void SCoherentInputForward::Construct(const FArguments& InArgs)
{
	ChildSlot
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill);

	OwnerActor = InArgs._Owner;	
	FocusedViewComponent = nullptr;
	bHasKeyboardFocus = false;
	bAlwaysAcceptMouseInputEvents = false;
	bPropagateKeyboardEvents = false;
	LineTraceMode = InputLineTrace_Single;

	SetRaycastCollisionGroup(ECC_WorldDynamic);
	SetRaycastTextureAddressMode(TA_Wrap);
	SetRaycastQuality(RQ_Accurate);
	SetRaycastUVChannel(0);
}

FReply SCoherentInputForward::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!bHasKeyboardFocus && !bAlwaysAcceptMouseInputEvents)
	{
		return FReply::Unhandled();
	}

	OnMouseEvent(MyGeometry, MouseEvent, Coherent::UI::MouseEventData::MouseMove);
	return FReply::Unhandled();
}

FReply SCoherentInputForward::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!bHasKeyboardFocus && !bAlwaysAcceptMouseInputEvents)
	{
		return FReply::Unhandled();
	}

	OnCoherentInputWidgetMouseButtonDown.Broadcast(MouseEvent.GetEffectingButton());
	return OnMouseEvent(MyGeometry, MouseEvent, Coherent::UI::MouseEventData::MouseDown);
}

FReply SCoherentInputForward::OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!bHasKeyboardFocus && !bAlwaysAcceptMouseInputEvents)
	{
		return FReply::Unhandled();
	}

	return OnMouseEvent(MyGeometry, MouseEvent, Coherent::UI::MouseEventData::MouseDown);
}

FReply SCoherentInputForward::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!bHasKeyboardFocus && !bAlwaysAcceptMouseInputEvents)
	{
		return FReply::Unhandled();
	}

	OnCoherentInputWidgetMouseButtonUp.Broadcast(MouseEvent.GetEffectingButton());
	return OnMouseEvent(MyGeometry, MouseEvent, Coherent::UI::MouseEventData::MouseUp);
}

FReply SCoherentInputForward::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!bHasKeyboardFocus && !bAlwaysAcceptMouseInputEvents)
	{
		return FReply::Unhandled();
	}

	return OnMouseEvent(MyGeometry, MouseEvent, Coherent::UI::MouseEventData::MouseWheel);
}

FReply SCoherentInputForward::OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!bHasKeyboardFocus && !bAlwaysAcceptMouseInputEvents)
	{
		return FReply::Unhandled();
	}

	OnMouseEvent(MyGeometry, MouseEvent, Coherent::UI::MouseEventData::MouseMove);
	return FReply::Unhandled();
}

void SCoherentInputForward::SetForceAcceptInputEvents(bool bAccept)
{
	// DEPRECATED method
	bAlwaysAcceptMouseInputEvents = bAccept;
}

void SCoherentInputForward::SetAlwaysAcceptMouseInputEvents(bool bAccept)
{
	bAlwaysAcceptMouseInputEvents = bAccept;
}

void SCoherentInputForward::SetPropagateKeyboardEvents(bool bPropagate)
{
	bPropagateKeyboardEvents = bPropagate;
}

void SCoherentInputForward::SetLineTraceMode(EInputWidgetLineTraceMode Mode)
{
	LineTraceMode = Mode;
}

void SCoherentInputForward::RefocusView(UCoherentBaseComponent* NewFocusedView)
{
	if (FocusedViewComponent != NewFocusedView)
	{
		if (FocusedViewComponent != nullptr && FocusedViewComponent->GetView() != nullptr)
		{
			FocusedViewComponent->GetView()->KillFocus();
		}

		FocusedViewComponent = NewFocusedView;

		if (FocusedViewComponent != nullptr && FocusedViewComponent->GetView() != nullptr)
		{
			FocusedViewComponent->GetView()->SetFocus();
		}
	}
}

FReply SCoherentInputForward::DiscardViewFocus(Coherent::UI::MouseEventData::EventType EventType)
{
	if (EventType != Coherent::UI::MouseEventData::MouseMove)
	{
		RefocusView(nullptr);
	}

	return FReply::Unhandled();
}

static bool IntersectRayTriangle(
	const FVector& RayOrigin, const FVector& RayDirection,
	const FVector& V0, const FVector& V1, const FVector& V2,
	float& T, float& U, float& V)
{
	// Moller-Trumbore ray-triangle intersection
	FVector E1 = V1 - V0;
	FVector E2 = V2 - V0;

	FVector Pvec = FVector::CrossProduct(RayDirection, E2);

	float Det = FVector::DotProduct(E1, Pvec);

	static const float DET_EPSILON = 0.001f;

	// With culling
	if (Det < DET_EPSILON)
	{
		return false;
	}

	FVector Tvec = RayOrigin - V0;
	U = FVector::DotProduct(Tvec, Pvec);

	if (U < 0 || U > Det)
	{
		return false;
	}

	FVector Qvec = FVector::CrossProduct(Tvec, E1);

	V = FVector::DotProduct(RayDirection, Qvec);

	if (V < 0 || U + V > Det)
	{
		return false;
	}

	float InvDet = 1.0f / Det;
	T = FVector::DotProduct(E2, Qvec) * InvDet;
	U *= InvDet;
	V *= InvDet;

	return true;
}

static bool IntersectWithRay(const FVector& RayOriginModelSpace, const FVector& RayDirectionModelSpace,
	const TArray<FCoherentUIVertexUV>& UVs,
	const TArray<FVector>& PositionBuffer,
	const FIndexArrayView& IndexBuffer,
	const uint32 UVChannel,
	float& OutDist, float& OutU, float& OutV)
{
	static const int32 NO_HIT = -1;
	int32 HitFaceIndex = NO_HIT; // Actually index of the hit face's first vertex index in the indices array, but never mind
	float MinDist = FLT_MAX;
	float TexU = 0;
	float TexV = 0;

	for (int32 i = 0; i < IndexBuffer.Num(); i += 3)
	{
		float T, U, V;
		const FVector& V0 = PositionBuffer[IndexBuffer[i]];
		const FVector& V1 = PositionBuffer[IndexBuffer[i + 2]];
		const FVector& V2 = PositionBuffer[IndexBuffer[i + 1]];

		if (IntersectRayTriangle(RayOriginModelSpace, RayDirectionModelSpace, V0, V1, V2, T, U, V))
		{
			if (T < MinDist)
			{
				MinDist = T;
				// Temporarily save barycentric coordinates here
				TexU = U;
				TexV = V;

				HitFaceIndex = i;
			}
		}
	}

	if (HitFaceIndex != NO_HIT)
	{
		// Compute texture coordinates from barycentric coordinates
		FVector2D TexCoords =
			(1 - TexU - TexV) * UVs[IndexBuffer[HitFaceIndex]].Channels[UVChannel] +
			TexU * UVs[IndexBuffer[HitFaceIndex + 2]].Channels[UVChannel] +
			TexV * UVs[IndexBuffer[HitFaceIndex + 1]].Channels[UVChannel];

		OutU = TexCoords.X;
		OutV = TexCoords.Y;
		OutDist = MinDist;

		return true;
	}

	return false;
}

static void AdjustUVCoordinates(TextureAddress AddressMode, float& U, float& V)
{
	switch (AddressMode)
	{
	case TA_Wrap:
		// Account for negative values as well
		U = FMath::Fmod(FMath::Fmod(U, 1.0f) + 1.0f, 1.0f);
		V = FMath::Fmod(FMath::Fmod(V, 1.0f) + 1.0f, 1.0f);
		return;
	case TA_Clamp:
		U = FMath::Clamp(U, 0.0f, 1.0f);
		V = FMath::Clamp(V, 0.0f, 1.0f);
		return;
	case TA_Mirror:
		U = FMath::Fmod(FMath::Fmod(U, 2.0f) + 2.0f, 2.0f);
		if (U > 1.0f)
		{
			U = 1.0f - U;
		}
		V = FMath::Fmod(FMath::Fmod(V, 2.0f) + 2.0f, 2.0f);
		if (V > 1.0f)
		{
			V = 1.0f - V;
		}
		return;
	default:
		UE_LOG(LogCoherentUI, Warning, TEXT("Unsupported texture addressing mode when adjusting texture coordinates!"));
		return;
	}
}

FReply SCoherentInputForward::OnMouseEvent(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, Coherent::UI::MouseEventData::EventType EventType)
{
	UWorld* World = nullptr;
	if (OwnerActor.IsValid())
	{
		World = OwnerActor->GetWorld();
	}

	if (World == nullptr)
	{
		return DiscardViewFocus(EventType);
	}
	
	const FVector2D& ScaledViewportPosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());

	UGameViewportClient* GameViewportClient = World->GetGameViewport();

	if (GameViewportClient == nullptr)
	{
		return DiscardViewFocus(EventType);
	}

	FVector2D ViewportSize;
	GameViewportClient->GetViewportSize(ViewportSize);

	//The inner implementation of GetDefault should always return non null value, thus we dont check the URendererSettings for null ptr
#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
	float DPIScale = GetDefault<URendererSettings>(URendererSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y));
#else
	float DPIScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y));
#endif
	//Since the mouse event's coordinated are scaled to count for the desired UI dpi, we need to remove the scaling
	FVector2D ViewportPositionVec2D;
	ViewportPositionVec2D.X = ScaledViewportPosition.X * DPIScale;
	ViewportPositionVec2D.Y = ScaledViewportPosition.Y * DPIScale;

	APlayerController* LocalPC = GEngine->GetFirstLocalPlayerController(World);
	if (!LocalPC)
	{
		return DiscardViewFocus(EventType);
	}

	ULocalPlayer* Player = Cast<ULocalPlayer>(Cast<APlayerController>(LocalPC)->Player);

	FSceneViewProjectionData ProjectionData;
	const bool bProjectionDataObtained = Player->GetProjectionData(Player->ViewportClient->Viewport, eSSP_FULL, /*out*/ ProjectionData);

	// Check HUD Views
	for (auto It = UCoherentBaseComponent::GetUCoherentBaseComponentIterator(World); It; ++It)
	{
		if ((*It).IsValid() && (*It)->GetWorld() == World)
		{
			UCoherentBaseComponent* Component = (*It).Get();
			UClass* ComponentClass = Component->GetClass();
			auto Owner = Component->GetOwner();
			if (UCoherentUIHUD::StaticClass() == ComponentClass || (Owner && Cast<AHUD>(Owner) != nullptr))
			{
				Coherent::UI::View* View = Component->GetView();
				if (Component->bReceiveInput && View)
				{
					FVector2D ConstraintOffset(0, 0);
					if (bProjectionDataObtained)
					{
						const FIntRect& ConstrainedViewRect = ProjectionData.GetConstrainedViewRect();
						ConstraintOffset = FVector2D(float(ConstrainedViewRect.Min.X), float(ConstrainedViewRect.Min.Y));
					}

					FVector2D ConstrainedPos = ViewportPositionVec2D - ConstraintOffset;

					float NormalizedX = ConstrainedPos.X / View->GetWidth();
					float NormalizedY = ConstrainedPos.Y / View->GetHeight();

					if (NormalizedX >= 0 && NormalizedX <= 1 && NormalizedY >= 0 && NormalizedY <= 1)
					{
						Component->IssueMouseOnUIQuery(NormalizedX, NormalizedY);
						if (Component->IsMouseOnView())
						{
							if (EventType == Coherent::UI::MouseEventData::MouseDown)
							{
								RefocusView(Component);
							}
							Coherent::UI::MouseEventData CoUIMouseData;
							GetMouseDataFromFPointerEvent(CoUIMouseData, MyGeometry, MouseEvent, EventType);
							CoUIMouseData.X = ConstrainedPos.X;
							CoUIMouseData.Y = ConstrainedPos.Y;

							View->MouseEvent(CoUIMouseData);

							return FReply::Handled().DetectDrag(SharedThis(this), MouseEvent.GetEffectingButton()).PreventThrottling();
						}
					}
				}
			}
		}
	}

	// Raycast and find 3D Coherent UI Views from the WorldDynamic collision type
	FVector Start;
	FVector Direction;
	FVector End;
	if (bProjectionDataObtained)
	{
#if defined(COHERENT_UI_UE4_47_SUPPORT)
		const FMatrix InvViewMatrix = ProjectionData.ViewMatrix.Inverse();
		const FMatrix InvProjectionMatrix = ProjectionData.ProjectionMatrix.Inverse();
		FSceneView::DeprojectScreenToWorld(ViewportPositionVec2D, ProjectionData.GetConstrainedViewRect(), InvViewMatrix, InvProjectionMatrix, /*out*/ Start, /*out*/ Direction);
#else
		// Create a view family for the game viewport
		FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
			Player->ViewportClient->Viewport,
			World->Scene,
			Player->ViewportClient->EngineShowFlags)
			.SetRealtimeUpdate(true));


		// Calculate a view where the player is to update the streaming from the players start location
		FVector ViewLocation;
		FRotator ViewRotation;

		FSceneView* SceneView = Player->CalcSceneView(&ViewFamily, /*out*/ ViewLocation, /*out*/ ViewRotation, Player->ViewportClient->Viewport);

		if (!SceneView)
			return DiscardViewFocus(EventType);

		FSceneView::DeprojectScreenToWorld(ViewportPositionVec2D, ProjectionData.GetConstrainedViewRect(), SceneView->ViewMatrices.ViewMatrix.InverseFast(), SceneView->ViewMatrices.ProjMatrix.InverseFast(), /*out*/ Start, /*out*/ Direction);
#endif
	}

	End = Start + WORLD_MAX * Direction;
	TArray<FHitResult> HitResults;
	bool bHitSuccessful = false;

	switch (LineTraceMode.GetValue())
	{
	case InputLineTrace_Single:
		HitResults.Add(FHitResult());
#if defined(COHERENT_UI_UE4_47_SUPPORT)
		bHitSuccessful = World->LineTraceSingle(HitResults[0], Start, End,
			FCollisionQueryParams(true), FCollisionObjectQueryParams(RaycastCollisionChannel));
#else
		bHitSuccessful = World->LineTraceSingleByObjectType(HitResults[0], Start, End,
			FCollisionObjectQueryParams(RaycastCollisionChannel), FCollisionQueryParams(true));
#endif
		break;
	case InputLineTrace_Multi:
#if defined(COHERENT_UI_UE4_47_SUPPORT)
		bHitSuccessful = World->LineTraceMulti(HitResults, Start, End,
			FCollisionQueryParams(true), FCollisionObjectQueryParams(RaycastCollisionChannel));
#else
		bHitSuccessful = World->LineTraceMultiByObjectType(HitResults, Start, End,
			FCollisionObjectQueryParams(RaycastCollisionChannel), FCollisionQueryParams(true));
#endif
		break;
	}

	for (int32_t i = 0; bHitSuccessful && i < HitResults.Num(); ++i)
	{
		FHitResult Hit = HitResults[i];
		if (Hit.Actor == nullptr)
		{
			return DiscardViewFocus(EventType);
		}

		UCoherentUIComponent* View = Hit.Actor->FindComponentByClass<UCoherentUIComponent>();
		if (View == nullptr)
		{
			// An object without UCoherentUIComponent component is blocking the input ray
			return DiscardViewFocus(EventType);
		}

		if (!View->bReceiveInput)
		{
			// This UCoherentUIComponent doesn't receive input;
			// Continue along the FHitResult array and try finding one that does.
			continue;
		}

		Coherent::UI::MouseEventData CoUIMouseData;
		GetMouseDataFromFPointerEvent(CoUIMouseData, MyGeometry, MouseEvent, EventType);

		if (View->EnsureMeshData() && View->GetView())
		{
			auto NumMeshes = View->MeshData.Num();
			if (!NumMeshes)
			{
				UE_LOG(LogCoherentUI, Warning, TEXT("The Coherent UI Input widget currently supports input on Static meshes only."));
				return FReply::Handled().DetectDrag(SharedThis(this), MouseEvent.GetEffectingButton()).PreventThrottling();
			}
			auto Index = 0u;
			for (; Index != NumMeshes; ++Index)
			{
				auto& MeshData = View->MeshData[Index];
				auto MeshComponent = MeshData.Mesh;
				if (!MeshComponent.IsValid())
				{
					continue;
				}
				int32 LODLevelToRaycast = 0;
				switch (RaycastQuality)
				{
				case RQ_Fast:
					LODLevelToRaycast = MeshComponent->StaticMesh->RenderData->LODResources.Num() - 1;
					break;
				case RQ_Balanced:
					LODLevelToRaycast = MeshComponent->StaticMesh->RenderData->LODResources.Num() / 2;
					break;
				case RQ_Accurate:
					LODLevelToRaycast = 0;
					break;
				}

				auto& Indices = MeshData.LODs[LODLevelToRaycast].Indices;
				auto& UVs = MeshData.LODs[LODLevelToRaycast].UVs;
				auto& Positions = MeshData.LODs[LODLevelToRaycast].Positions;
				FIndexArrayView IndexBuffer(Indices.GetData(), Indices.Num(), true);

				FStaticMeshLODResources& LODRes = MeshComponent->StaticMesh->RenderData->LODResources[LODLevelToRaycast];
				check(LODRes.GetNumTriangles() == IndexBuffer.Num() / 3);

				FTransform ComponentTransform = MeshComponent->GetComponentTransform();

				FVector StartModelSpace = ComponentTransform.InverseTransformPosition(Start);
				FVector DirectionModelSpace = ComponentTransform.InverseTransformVector(Direction);
				float U, V, Dist;
				if (IntersectWithRay(StartModelSpace, DirectionModelSpace, UVs, Positions, IndexBuffer, UVChannel, Dist, U, V))
				{
					AdjustUVCoordinates(RaycastTextureAddressMode, U, V);
					CoUIMouseData.X = U * View->GetView()->GetWidth();
					CoUIMouseData.Y = V * View->GetView()->GetHeight();

					if (EventType == Coherent::UI::MouseEventData::MouseDown)
					{
						RefocusView(View);
					}
					View->GetView()->MouseEvent(CoUIMouseData);
					break;
				}
			}
			// we have hit the object, but not the view
			if (Index == NumMeshes)
			{
				return FReply::Handled();
			}
		}
		return FReply::Handled().DetectDrag(SharedThis(this), MouseEvent.GetEffectingButton()).PreventThrottling();
	}

	return DiscardViewFocus(EventType);
}

#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
FReply SCoherentInputForward::OnKeyDown(const FGeometry& MyGeometry, const FKeyboardEvent& KeyEvent)
#else
FReply SCoherentInputForward::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
#endif
{
	OnCoherentInputWidgetKeyDown.Broadcast(KeyEvent.GetKey());
	ProcessKeyEvent(KeyEvent, Coherent::UI::KeyEventData::KeyDown);
	return bPropagateKeyboardEvents ? FReply::Unhandled() : FReply::Handled();
}

#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
FReply SCoherentInputForward::OnKeyUp(const FGeometry& MyGeometry, const FKeyboardEvent& KeyEvent)
#else
FReply SCoherentInputForward::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
#endif
{
	OnCoherentInputWidgetKeyUp.Broadcast(KeyEvent.GetKey());
	ProcessKeyEvent(KeyEvent, Coherent::UI::KeyEventData::KeyUp);
	return bPropagateKeyboardEvents ? FReply::Unhandled() : FReply::Handled();
}

FReply SCoherentInputForward::OnKeyChar( const FGeometry& MyGeometry,const FCharacterEvent& InCharacterEvent )
{
	ProcessKeyEvent(InCharacterEvent, Coherent::UI::KeyEventData::Char);
	return bPropagateKeyboardEvents ? FReply::Unhandled() : FReply::Handled();
}

bool SCoherentInputForward::SupportsKeyboardFocus() const
{
	return true;
}

#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
FReply SCoherentInputForward::OnKeyboardFocusReceived( const FGeometry& MyGeometry, const FKeyboardFocusEvent& InKeyboardFocusEvent )
#else
FReply SCoherentInputForward::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InKeyboardFocusEvent)
#endif
{
	bHasKeyboardFocus = true;
	return FReply::Unhandled();
}

#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
void SCoherentInputForward::OnKeyboardFocusLost(const FKeyboardFocusEvent& InKeyboardFocusEvent)
#else
void SCoherentInputForward::OnFocusLost(const FFocusEvent& InKeyboardFocusEvent)
#endif
{
	bHasKeyboardFocus = false;
}

#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
void SCoherentInputForward::OnKeyboardFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath)
#else
void SCoherentInputForward::OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath)
#endif
{
}

static bool IsCapsLockOn()
{
#if PLATFORM_WINDOWS
	return ((::GetKeyState(VK_CAPITAL) & 0x0001) != 0);
#else
	return false;
#endif
}

static bool IsNumLockOn()
{
#if PLATFORM_WINDOWS
	return ((::GetKeyState(VK_NUMLOCK) & 0x0001) != 0);
#else
	return false;
#endif
}

Coherent::UI::KeyEventData SCoherentInputForward::MakeKeyEvent(const FInputEvent& Event, Coherent::UI::KeyEventData::EventType Type)
{
	Coherent::UI::KeyEventData Result;

	Result.IsAutoRepeat = Event.IsRepeat();
	Result.IsNumPad = false;
	Result.Type = Type;
	Result.Modifiers.IsAltDown = Event.IsLeftAltDown();
	Result.Modifiers.IsCapsOn = IsCapsLockOn();
	Result.Modifiers.IsCtrlDown = Event.IsControlDown();
	Result.Modifiers.IsMetaDown = false;
	Result.Modifiers.IsNumLockOn = IsNumLockOn();
	Result.Modifiers.IsShiftDown = Event.IsShiftDown();

	if (Type == Coherent::UI::KeyEventData::KeyDown || Type == Coherent::UI::KeyEventData::KeyUp)
	{
#if defined (COHERENT_UI_UE4_4_5_SUPPORT)
		FKey Key = static_cast<const FKeyboardEvent&>(Event).GetKey();
#else
		FKey Key = static_cast<const FKeyEvent&>(Event).GetKey();
#endif
		if (
			Key == EKeys::NumLock ||
			Key == EKeys::NumPadZero ||
			Key == EKeys::NumPadOne ||
			Key == EKeys::NumPadTwo ||
			Key == EKeys::NumPadThree ||
			Key == EKeys::NumPadFour ||
			Key == EKeys::NumPadFive ||
			Key == EKeys::NumPadSix ||
			Key == EKeys::NumPadSeven ||
			Key == EKeys::NumPadEight ||
			Key == EKeys::NumPadNine
			)
		{
			Result.IsNumPad = true;
		}
	#define GET_COHERENT_VK_FROM_FKEY(FKey, CoherentVK) \
		if (Key == EKeys::FKey) Result.KeyCode = CoherentVK; \

		FKEY_MACRO_ITERATOR(GET_COHERENT_VK_FROM_FKEY);
	#undef GET_COHERENT_VK_FROM_FKEY
	}
	else if (Type == Coherent::UI::KeyEventData::Char)
	{
		Result.KeyCode = static_cast<const FCharacterEvent&>(Event).GetCharacter();
	}
	else
	{
		check(false);
		Result.KeyCode = 0;
	}

	return Result;
}

void SCoherentInputForward::ProcessKeyEvent(const FInputEvent& Event, Coherent::UI::KeyEventData::EventType Type)
{
	bool IsPrintableCharacter = false;
	Coherent::UI::KeyEventData KeyData = MakeKeyEvent(Event, Type);

	if (FocusedViewComponent != nullptr)
	{
		FocusedViewComponent->GetView()->KeyEvent(KeyData);
	}
}

void SCoherentInputForward::GetMouseDataFromFPointerEvent(Coherent::UI::MouseEventData& Data, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, Coherent::UI::MouseEventData::EventType EventType)
{
	Data.Type = EventType;
	Data.WheelX = 0;
	Data.WheelY = 0;

	Data.Modifiers.IsAltDown = MouseEvent.IsLeftAltDown();
	Data.Modifiers.IsCapsOn = IsCapsLockOn();
	Data.Modifiers.IsCtrlDown = MouseEvent.IsControlDown();
	Data.Modifiers.IsMetaDown = false;
	Data.Modifiers.IsNumLockOn = IsNumLockOn();
	Data.Modifiers.IsShiftDown = MouseEvent.IsShiftDown();

	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		if (Data.Button == Coherent::UI::MouseEventData::ButtonNone)
		{
			Data.Button = Coherent::UI::MouseEventData::ButtonLeft;
		}
		Data.MouseModifiers.IsLeftButtonDown = true;
	}
	if (MouseEvent.IsMouseButtonDown(EKeys::MiddleMouseButton))
	{
		if (Data.Button == Coherent::UI::MouseEventData::ButtonNone)
		{
			Data.Button = Coherent::UI::MouseEventData::ButtonMiddle;
		}
		Data.MouseModifiers.IsMiddleButtonDown = true;
	}
	if (MouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		if (Data.Button == Coherent::UI::MouseEventData::ButtonNone)
		{
			Data.Button = Coherent::UI::MouseEventData::ButtonRight;
		}
		Data.MouseModifiers.IsRightButtonDown = true;
	}	

	if (EventType == Coherent::UI::MouseEventData::MouseWheel)
	{
		if (MouseEvent.IsShiftDown())
		{
			Data.WheelX = int32(MouseEvent.GetWheelDelta());
		}
		else
		{
			Data.WheelY = int32(MouseEvent.GetWheelDelta());
		}
	}
	else if ((EventType == Coherent::UI::MouseEventData::MouseDown ||
		EventType == Coherent::UI::MouseEventData::MouseUp) &&
		Data.Button == Coherent::UI::MouseEventData::ButtonNone)
	{
		FKey Btn = MouseEvent.GetEffectingButton();
		if (Btn == EKeys::LeftMouseButton)
		{
			Data.Button = Coherent::UI::MouseEventData::ButtonLeft;
		}
		else if (Btn == EKeys::RightMouseButton)
		{
			Data.Button = Coherent::UI::MouseEventData::ButtonRight;
		}
		else if (Btn == EKeys::MiddleMouseButton)
		{
			Data.Button = Coherent::UI::MouseEventData::ButtonMiddle;
		}
	}
}

void SCoherentInputForward::SetRaycastCollisionGroup(ECollisionChannel CollisionChannel)
{
	RaycastCollisionChannel = CollisionChannel;
}

void SCoherentInputForward::SetRaycastTextureAddressMode(TextureAddress TextureAddressMode)
{
	RaycastTextureAddressMode = TextureAddressMode;
}

void SCoherentInputForward::SetRaycastQuality(EInputWidgetRaycastQuality InRaycastQuality)
{
	this->RaycastQuality = InRaycastQuality;
}

void SCoherentInputForward::SetRaycastUVChannel(uint32 InUVChannel)
{
	this->UVChannel = InUVChannel;
}
