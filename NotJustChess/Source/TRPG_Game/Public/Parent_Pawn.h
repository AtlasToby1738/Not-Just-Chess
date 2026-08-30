#pragma once

#include "CoreMinimal.h"
#include <Components/TimelineComponent.h>
#include <GameFramework/Pawn.h>
#include "Parent_Pawn.generated.h"


class UCurveFloat;

UCLASS()
class TRPG_GAME_API AParent_Pawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AParent_Pawn();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	TArray<int> MovementTiles{ -1, 0, 1 };

private :
	FVector2D pos;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UMaterialInterface* Material;

	UPROPERTY(meta = (MakeEditWidget = true), EditAnywhere, BlueprintReadWrite, Category = "TimeLine")
	FVector StartLocation;

	UPROPERTY(meta = (MakeEditWidget = true), EditAnywhere, BlueprintReadWrite, Category = "TimeLine")
	FVector EndLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimeLine")
	UCurveFloat* TextXY = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimeLine")
	UCurveFloat* TestZ = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimeLine")
	float PlayRate;

	FTimeline TimeLineXY;
	FTimeline TimeLineZ;

	UFUNCTION()
	void TimeLineXYUpdate(float XY);

	UFUNCTION()
	void TimeLineZUpdate(float Z);

	UFUNCTION()
	void TimelineXYFinished();

	UFUNCTION()
	void TimelineZFinished();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//static void Move();
};
