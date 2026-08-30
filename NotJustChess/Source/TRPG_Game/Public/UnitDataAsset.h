#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UnitStruct.h"
#include "UnitDataAsset.generated.h"

UCLASS()
class TRPG_GAME_API UUnitDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	EUnitTypes UnitType;
};
