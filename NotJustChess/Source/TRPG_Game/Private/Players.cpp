#include "Players.h"

// Sets default values
APlayers::APlayers()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = Scene;
}

// Called when the game starts or when spawned
void APlayers::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayers::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayers::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

