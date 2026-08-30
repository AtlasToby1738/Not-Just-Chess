// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Components/TimelineComponent.h>
#include <GameFramework/Actor.h>
#include "Parent_Character.generated.h"

class UCurveFloat;

UCLASS()
class TRPG_GAME_API AParent_Character : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParent_Character();

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

};
