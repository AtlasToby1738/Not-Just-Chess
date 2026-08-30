#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UnitStruct.generated.h"

UENUM(BlueprintType)
enum class EUnitTypes : uint8
{
	MAGE,
	RANGER,
	MAX UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EUnitTypes, EUnitTypes::MAX);

UENUM(BlueprintType)
enum class EUnitAnimState : uint8
{
	IDLE,
	WALK,
	ATTACK,
	HIT,
	DEATH,
	MAX UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EUnitAnimState, EUnitAnimState::MAX);

USTRUCT(BlueprintType)
struct FSUnit : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUnitTypes Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimClass;
};

USTRUCT(BlueprintType)
struct FUnitMeshAnim 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* MageMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* RangerMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> MageAnimClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> RangerAnimClass;
};

class TRPG_GAME_API UnitStruct
{
public:
	UnitStruct();
	~UnitStruct();


};
