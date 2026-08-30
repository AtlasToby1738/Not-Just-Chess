#pragma once

#include "CoreMinimal.h"
#include "PlayerAction.h"
#include "SelectedAction.generated.h"

UCLASS()
class TRPG_GAME_API USelectedAction : public UPlayerAction
{
	GENERATED_BODY()

public:
	void LeftClick(int Index) override;

};
