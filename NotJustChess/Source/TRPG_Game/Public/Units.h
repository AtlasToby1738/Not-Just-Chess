#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitStruct.h"
#include "UnitInterface.h"
#include "Units.generated.h"

UCLASS()
class TRPG_GAME_API AUnits : public AActor, public IUnitInterface
{
	GENERATED_BODY()

public:	
	AUnits();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	USkeletalMeshComponent* SkeletalMesh;

	UFUNCTION(BlueprintCallable)
	void PlayAnim();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	EUnitTypes ThisType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	EUnitAnimState AnimState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	class UDataTable* dataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	class UUnitDataAsset* dataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assets")
	FUnitMeshAnim AnimMeshREf;

	FTimerHandle TimerHandle;

protected:
	virtual void BeginPlay() override;

	void OnConstruction(const FTransform& Transform) override;

public:
	virtual void Tick(float DeltaTime) override;

};
