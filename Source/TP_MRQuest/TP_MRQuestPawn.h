#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HeadMountedDisplayTypes.h"
#include "TP_MRQuestPawn.generated.h"

class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UIsdkHandRigComponentLeft;
class UIsdkHandRigComponentRight;
class UInputMappingContext;

/**
 * Bazowy pawn MR dla Meta Quest 3 (przeniesiony z MRSimulatorBeEco, bez logiki gry).
 * Struktura: VROrigin -> Camera (śledzi HMD), MotionControllerLeft/Right -> rigi dłoni ISDK (grab/poke/ray).
 * Passthrough (persistent, Underlay) startuje w BeginPlay.
 * Nie dodawaj w Blueprincie komponentów Oculus XR Hand ani drugiego passthrough – będą zdublowane.
 */
UCLASS()
class TP_MRQUEST_API ATP_MRQuestPawn : public APawn
{
	GENERATED_BODY()

public:
	ATP_MRQuestPawn();

	UFUNCTION(BlueprintPure, Category = "MR|Hands")
	UIsdkHandRigComponentLeft* GetLeftHandRig() const { return LeftHandRig; }

	UFUNCTION(BlueprintPure, Category = "MR|Hands")
	UIsdkHandRigComponentRight* GetRightHandRig() const { return RightHandRig; }

protected:
	virtual void BeginPlay() override;
	virtual void NotifyControllerChanged() override;

	/** Punkt odniesienia przestrzeni gracza (podłoga). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MR|Components")
	TObjectPtr<USceneComponent> VROrigin;

	/** Kamera przypięta do gogli. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MR|Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MR|Components")
	TObjectPtr<UMotionControllerComponent> MotionControllerLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MR|Components")
	TObjectPtr<UMotionControllerComponent> MotionControllerRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MR|Components")
	TObjectPtr<UIsdkHandRigComponentLeft> LeftHandRig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MR|Components")
	TObjectPtr<UIsdkHandRigComponentRight> RightHandRig;

	/** LocalFloor trzyma Z = 0 na prawdziwej podłodze (tego oczekuje m.in. MRUK). */
	UPROPERTY(EditDefaultsOnly, Category = "MR|Tracking")
	TEnumAsByte<EHMDTrackingOrigin::Type> TrackingOrigin = EHMDTrackingOrigin::LocalFloor;

	/** ISDK sam nie rejestruje swojego mapping contextu; bez niego akcje pinch nie odpalają się na Meta XR. */
	UPROPERTY(EditDefaultsOnly, Category = "MR|Input")
	TObjectPtr<UInputMappingContext> HandInputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "MR|Input")
	int32 HandInputMappingPriority = 0;

	UPROPERTY(EditDefaultsOnly, Category = "MR|Passthrough")
	bool bStartPassthrough = true;

	/** Krycie obrazu passthrough; poniżej 1 przyciemnia prawdziwy pokój za wirtualną treścią. */
	UPROPERTY(EditDefaultsOnly, Category = "MR|Passthrough", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float PassthroughOpacity = 1.0f;

private:
	void ApplyTrackingOrigin() const;
	void AddHandInputMapping() const;
	void StartPassthrough();

	UFUNCTION()
	void HandlePassthroughLayerResumed();

	UFUNCTION()
	void HandleLeftHandDataSourcesReady();

	UFUNCTION()
	void HandleRightHandDataSourcesReady();
};
