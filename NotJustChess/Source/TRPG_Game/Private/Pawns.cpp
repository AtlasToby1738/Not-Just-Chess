#include "Pawns.h"

// Sets default values
APawns::APawns()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	RootComponent = StaticMesh;
}

// Called when the game starts or when spawned
void APawns::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APawns::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APawns::TakeDamage(float _damage)
{
	int32 _newLife = PawnStruct.PawnStats.Health -= _damage;
	PawnStruct.PawnStats.Health = FMath::Max(_newLife, 0);
}

bool APawns::CheckForRange(int InstanceIndex, int SizeY, int SizeX)
{
	return false;
}

// Called to bind functionality to input
void APawns::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

