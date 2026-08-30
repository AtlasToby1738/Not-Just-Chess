#pragma once

#include "CoreMinimal.h"
#include "PlayerAction.h"
#include "Parent_Pawn.h"
#include "MovePawnAction.generated.h"

UCLASS()
class TRPG_GAME_API UMovePawnAction : public UPlayerAction
{
    GENERATED_BODY()

public:
    void LeftClick(int Index) override;

    AParent_Pawn* Pawn;
};
