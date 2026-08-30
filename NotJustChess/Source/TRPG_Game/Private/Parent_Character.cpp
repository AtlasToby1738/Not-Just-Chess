// Fill out your copyright notice in the Description page of Project Settings.


#include "Parent_Character.h"
#include <Components/InstancedStaticMeshComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Components/PrimitiveComponent.h>

// Sets default values
AParent_Character::AParent_Character()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");

}

// Called when the game starts or when spawned
void AParent_Character::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	EndLocation += StartLocation;

	FOnTimelineFloat ProgressUpdateXY;
	ProgressUpdateXY.BindUFunction(this, FName("TimeLineXYUpdate"));

	FOnTimelineFloat ProgressUpdateZ;
	ProgressUpdateZ.BindUFunction(this, FName("TimeLineZUpdate"));

	FOnTimelineFloat FinishedEventXY;
	FinishedEventXY.BindUFunction(this, FName("TimelineXYFinished"));

	FOnTimelineFloat FinishedEventZ;
	FinishedEventZ.BindUFunction(this, FName("TimelineZFinished"));

	TimeLineXY.AddInterpFloat(TextXY, ProgressUpdateXY);
	TimeLineZ.AddInterpFloat(TestZ, ProgressUpdateZ);
	//TestTimeline.SetTimelineFinishedFunc(TimelineFinished());


	TimeLineXY.SetPlayRate(PlayRate);
	TimeLineXY.PlayFromStart();
	TimeLineZ.SetPlayRate(PlayRate);
	TimeLineZ.PlayFromStart();
	
}

// Called every frame
void AParent_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeLineXY.TickTimeline(DeltaTime);
	TimeLineZ.TickTimeline(DeltaTime);
}

void AParent_Character::TimeLineXYUpdate(float XY)
{
	float Xposition = FMath::Lerp(StartLocation.X, EndLocation.X, XY);
	float Yposition = FMath::Lerp(StartLocation.Y, EndLocation.Y, XY);
	SetActorLocation(FVector(Xposition, Yposition, GetActorLocation().Z));
}

void AParent_Character::TimeLineZUpdate(float Z)
{
	float Zposition = FMath::Lerp(StartLocation.Z, ((EndLocation - StartLocation).Length()) / 2, Z);
	SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, Zposition));
}

void AParent_Character::TimelineXYFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("TimeLineXY Finished"));
}

void AParent_Character::TimelineZFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("TimeLineZ Finished"));
}

