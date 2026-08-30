#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Players.generated.h"

UENUM(BlueprintType)
enum class EPlayersStates : uint8
{
	None,
	PlayerTurn,
	Grabbing,
	Attacking,
	EnemyTurn,
	Max UMETA(Hidden)
};

UCLASS()
class TRPG_GAME_API APlayers : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayers();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* Scene;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	EPlayersStates CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float RotationSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	ACameraActor* LevelCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float YPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float XPos;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
