#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Pawns.generated.h"

UENUM(BlueprintType)
enum class EPawnType : uint8
{
	None,
	Pawn,
	King,
	Knight,
	Tower,
	Bishop,
	Queen,
	Max UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ETeam : uint8
{
	None,
	Ally,
	Opponent,
	Max UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FPawnStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int Attack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int Level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int Size;

	FPawnStats() {}
	FPawnStats(int _attack, int _maxHealth, int _level, int _size) : Attack(_attack), Health(_maxHealth), MaxHealth(_maxHealth), Level(_level), Size(_size) {}
};

USTRUCT(BlueprintType)
struct FPawnsStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VALUE")
	EPawnType PawnType = EPawnType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VALUE")
	ETeam PawnTeam = ETeam::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VALUE")
	FName PawnName = "???";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VALUE")
	FPawnStats PawnStats;

	FPawnsStruct() {}
	FPawnsStruct(EPawnType(_type), ETeam(_team), FName(_name), FPawnStats(_stats)) : PawnType(_type), PawnTeam(_team), PawnName(_name), PawnStats(_stats) {}

};

UCLASS()
class TRPG_GAME_API APawns : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APawns();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* StaticMesh;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void UpdateLifeHUD();

	UFUNCTION(BlueprintImplementableEvent, Category = "Promotion")
	void UpdateVisualOnPromotionIA();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Index")
	int Index = -999;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FPawnsStruct PawnStruct;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void TakeDamage(float _damage);

	UFUNCTION(BlueprintCallable)
	bool CheckForRange(int InstanceIndex, int SizeY, int SizeX);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
