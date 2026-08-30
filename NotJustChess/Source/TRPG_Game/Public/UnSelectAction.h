#pragma once

#include "CoreMinimal.h"
#include "PlayerAction.h"
#include "UnSelectAction.generated.h"

UCLASS()
class TRPG_GAME_API UUnSelectAction : public UPlayerAction
{
	GENERATED_BODY()

public:
	void RightClick(int Index) override;
	
};
