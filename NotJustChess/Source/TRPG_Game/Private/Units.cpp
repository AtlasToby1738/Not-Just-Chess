#include "Units.h"
#include "UnitInterface.h"
#include "Engine/DataTable.h"
#include "UnitDataAsset.h"

AUnits::AUnits()
{
	PrimaryActorTick.bCanEverTick = true;
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMesh");
	RootComponent = SkeletalMesh;

}

void AUnits::BeginPlay()
{
	Super::BeginPlay();

	PlayAnim();
	
}

void AUnits::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Pour la DataTable
	if (!dataTable) { UE_LOG(LogTemp, Error, TEXT("Not found DataTable")); return;}

	FSUnit* Row = dataTable->FindRow<FSUnit>(FName("mage"), TEXT(""));

	if (Row)
	{
		ThisType = Row->Type;
	}

	// Pour la DataAsset
	//ThisType = dataAsset->UnitType;

	switch (ThisType)
	{
	case EUnitTypes::MAGE:
		SkeletalMesh->SetSkeletalMeshAsset(AnimMeshREf.MageMesh);
		SkeletalMesh->SetAnimInstanceClass(AnimMeshREf.MageAnimClass);
		break;
	case EUnitTypes::RANGER:
		SkeletalMesh->SetSkeletalMeshAsset(AnimMeshREf.RangerMesh);
		SkeletalMesh->SetAnimInstanceClass(AnimMeshREf.RangerAnimClass);
		break;
	}
}

void AUnits::PlayAnim()
{
	AnimState = (EUnitAnimState)(((uint8)AnimState + 1) % 6);
	
	IUnitInterface::Execute_SetUnitAnimState(SkeletalMesh->GetAnimInstance(), AnimState);

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AUnits::PlayAnim, 3, false);

}

void AUnits::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}