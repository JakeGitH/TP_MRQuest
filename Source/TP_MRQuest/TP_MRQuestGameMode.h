#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TP_MRQuestGameMode.generated.h"

/**
 * Bazowy GameMode szablonu MR. Blueprint BP_MRGameMode nadpisuje Default Pawn Class na BP_MRPawn.
 */
UCLASS()
class TP_MRQUEST_API ATP_MRQuestGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATP_MRQuestGameMode();
};
