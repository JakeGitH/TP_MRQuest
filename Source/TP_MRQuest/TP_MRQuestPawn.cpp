#include "TP_MRQuestPawn.h"

#include "TP_MRQuest.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "IXRTrackingSystem.h"
#include "InputMappingContext.h"
#include "MotionControllerComponent.h"
#include "OculusXRPassthroughLayerComponent.h"
#include "OculusXRPassthroughLayerShapes.h"
#include "OculusXRPassthroughSubsystem.h"
#include "OculusXRPersistentPassthroughInstance.h"
#include "Rig/IsdkHandRigComponent.h"
#include "UObject/ConstructorHelpers.h"

ATP_MRQuestPawn::ATP_MRQuestPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	SetRootComponent(VROrigin);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VROrigin);
	Camera->bLockToHmd = true;

	MotionControllerLeft = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionControllerLeft"));
	MotionControllerLeft->SetupAttachment(VROrigin);
	MotionControllerLeft->SetTrackingMotionSource(FName(TEXT("Left")));

	MotionControllerRight = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionControllerRight"));
	MotionControllerRight->SetupAttachment(VROrigin);
	MotionControllerRight->SetTrackingMotionSource(FName(TEXT("Right")));

	// Rigi dłoni ISDK muszą być bezpośrednimi dziećmi motion controllera, żeby znaleźć dane śledzenia.
	LeftHandRig = CreateDefaultSubobject<UIsdkHandRigComponentLeft>(TEXT("LeftHandRig"));
	LeftHandRig->SetupAttachment(MotionControllerLeft);

	RightHandRig = CreateDefaultSubobject<UIsdkHandRigComponentRight>(TEXT("RightHandRig"));
	RightHandRig->SetupAttachment(MotionControllerRight);

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> HandImcFinder(
		TEXT("/OculusInteraction/Inputs/IMC_IsdkHand.IMC_IsdkHand"));
	if (HandImcFinder.Succeeded())
	{
		HandInputMappingContext = HandImcFinder.Object;
	}
}

void ATP_MRQuestPawn::BeginPlay()
{
	// Przed Super::BeginPlay, które uruchamia BeginPlay rigów.
	LeftHandRig->DataSourcesReady.AddDynamic(this, &ATP_MRQuestPawn::HandleLeftHandDataSourcesReady);
	RightHandRig->DataSourcesReady.AddDynamic(this, &ATP_MRQuestPawn::HandleRightHandDataSourcesReady);

	Super::BeginPlay();

	ApplyTrackingOrigin();

	if (bStartPassthrough)
	{
		StartPassthrough();
	}
}

void ATP_MRQuestPawn::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
	AddHandInputMapping();
}

void ATP_MRQuestPawn::ApplyTrackingOrigin() const
{
	if (!GEngine || !GEngine->XRSystem.IsValid())
	{
		UE_LOG(LogTP_MRQuest, Warning, TEXT("No XR system - tracking origin not set (running without HMD?)"));
		return;
	}

	GEngine->XRSystem->SetTrackingOrigin(TrackingOrigin);
	UE_LOG(LogTP_MRQuest, Log, TEXT("Tracking origin set to %d"), static_cast<int32>(TrackingOrigin));
}

void ATP_MRQuestPawn::AddHandInputMapping() const
{
	const APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController || !HandInputMappingContext)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (!InputSubsystem)
	{
		UE_LOG(LogTP_MRQuest, Warning, TEXT("Enhanced Input subsystem missing - hand input mapping not added"));
		return;
	}

	InputSubsystem->AddMappingContext(HandInputMappingContext, HandInputMappingPriority);
	UE_LOG(LogTP_MRQuest, Log, TEXT("Hand input mapping context added: %s"), *HandInputMappingContext->GetName());
}

void ATP_MRQuestPawn::StartPassthrough()
{
	UOculusXRPassthroughSubsystem* PassthroughSubsystem =
		UOculusXRPassthroughSubsystem::GetPassthroughSubsystem(GetWorld());
	if (!PassthroughSubsystem)
	{
		UE_LOG(LogTP_MRQuest, Warning,
			TEXT("Passthrough subsystem not available - check Project Settings > Meta XR > Passthrough Enabled"));
		return;
	}

	// Outer = subsystem, żeby kształt żył tak długo jak trwała warstwa, a nie ten pawn.
	UOculusXRStereoLayerShapeReconstructed* Shape =
		NewObject<UOculusXRStereoLayerShapeReconstructed>(PassthroughSubsystem);
	Shape->LayerOrder = PassthroughLayerOrder_Underlay;
	Shape->TextureOpacityFactor = PassthroughOpacity;

	FOculusXRPersistentPassthroughParameters Parameters;
	Parameters.bVisible = true;
	Parameters.Shape = Shape;
	// W edytorze Shape jest odrzucany i odtwarzany z zapisanych wartości, więc też muszą być wypełnione (domyślnie Overlay).
	Parameters.ApplyShape();

	FOculusXRPassthrough_LayerResumed_Single LayerResumed;
	LayerResumed.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(ATP_MRQuestPawn, HandlePassthroughLayerResumed));

	const UOculusXRPersistentPassthroughInstance* Instance =
		PassthroughSubsystem->InitializePersistentPassthrough(Parameters, LayerResumed);
	if (Instance)
	{
		UE_LOG(LogTP_MRQuest, Log, TEXT("Persistent passthrough initialized (Underlay, opacity %.2f)"), PassthroughOpacity);
	}
	else
	{
		UE_LOG(LogTP_MRQuest, Error, TEXT("Persistent passthrough initialization failed"));
	}
}

void ATP_MRQuestPawn::HandlePassthroughLayerResumed()
{
	UE_LOG(LogTP_MRQuest, Log, TEXT("Passthrough layer resumed (visible)"));
}

void ATP_MRQuestPawn::HandleLeftHandDataSourcesReady()
{
	UE_LOG(LogTP_MRQuest, Log, TEXT("Left hand rig ready"));
}

void ATP_MRQuestPawn::HandleRightHandDataSourcesReady()
{
	UE_LOG(LogTP_MRQuest, Log, TEXT("Right hand rig ready"));
}
