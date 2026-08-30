#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PlayerAction.generated.h"

class AGridManager;

UCLASS()
class TRPG_GAME_API UPlayerAction : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(AGridManager* grid) { GridRef = grid; }
	virtual void ExecuteAction(int Index);
	virtual void LeftClick(int Index);
	virtual void RightClick(int Index);

protected:
	AGridManager* GridRef;
	
};
