#pragma once

#include "CoreMinimal.h"
#include "PlayerAction.h"
#include "HoveredAction.generated.h"

UCLASS()
class TRPG_GAME_API UHoveredAction : public UPlayerAction
{
	GENERATED_BODY()
	
public:
	void ExecuteAction(int Index) override;
	void UnHoverTile(int index);
};
