#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UnitInterface.generated.h"

UINTERFACE(MinimalAPI)
class UUnitInterface : public UInterface
{
	GENERATED_BODY()
};

class TRPG_GAME_API IUnitInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetUnitAnimState(enum EUnitAnimState State);
};
