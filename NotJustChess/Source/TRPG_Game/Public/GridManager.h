#pragma once

#include "Pawns.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

UENUM(BlueprintType)
enum class ETileState : uint8
{
	Selected,
	Hovered,
	Way,
	PreFire,
	Vulnerable,
	Occuped,
	Accessible,
	None UMETA(DisplayName = "Not Hovered"),
	MAX UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(ETileState, ETileState::MAX);

UENUM(BlueprintType)
enum class EGameResult : uint8
{
	PlayerWin,
	OpponentWin,
	Draw,
};

UENUM(BlueprintType)
enum class EEndGameReason : uint8
{
	KingDefeated,
	MaterialAdvantage,
	Draw,
};

UENUM(BlueprintType)
enum class ETileType : uint8
{
	Occuped,
	Blocked,
	Normal,
	Max UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FTileData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<ETileState> StateArray;

	UPROPERTY()
	ETileType TypeArray = ETileType::Normal;

	FTileData() {}

	FTileData(TArray<ETileState> _states, ETileType _type) : StateArray(_states), TypeArray(_type) {}
};

UCLASS()
class TRPG_GAME_API AGridManager : public AActor
{
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

	AGridManager();
	virtual void Tick(float DeltaTime) override;


	// Check for the end of the game *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

	UFUNCTION(BlueprintCallable, Category = "Victory")
	bool CheckForEnd();

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable ,Category = "Victory")
	void EndTheGame(EGameResult Result, EEndGameReason Reason);

	// Bonus Turn *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

	UPROPERTY(BlueprintReadWrite, Category = "Bonus")
	int BonusActionsRemaining = 0;

	UFUNCTION(BlueprintImplementableEvent, Category = "Bonus")
	void OnBonusTurnGranted(int _amount);

	// LevelUP Pawns *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

	UFUNCTION(BlueprintImplementableEvent, Category = "LevelUP")
	void LevelUpPawn(APawns* _pawn, int _level);

	// Downed Pawns *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

	UFUNCTION(BlueprintCallable, Category = "KO")
	int GetTurnsBeforeDeath(int _tileIndex) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "KO")
	void OnDownedPawnCountdownUpdated(APawns* _pawn, int _turnsRemaining);
	 
	UFUNCTION(BlueprintCallable, Category = "KO")
	void ProcessDownedPawns();

	// Opponant *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

	UFUNCTION(BlueprintCallable, Category = "Opponant")
	void PlayEnemyTurn();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Opponant")
	void CountEnemyPA(int _pa);

	UFUNCTION(BlueprintImplementableEvent, Category = "Opponant")
	void EndEnemyTurn();

	UFUNCTION(BlueprintCallable, Category = "Opponant")
	void SetQueenAttackCross(int Center);

	UFUNCTION(BlueprintImplementableEvent, Category = "Opponant")
	void MovePawnVisually(APawns* _pawn, int _index);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Opponant")
	class UEnemyAction* EnemyAI;

	void PlayNextEnemyAction();

	UPROPERTY(BlueprintReadOnly, Category = "Opponant")
	int ActionPointsRemaining = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Opponant")
	int BaseActionPoints = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Opponant")
	float EnemyActionDelay = 2.0f;

	// Promotion *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

	UFUNCTION(BlueprintCallable, Category = "Promotion")
	bool CheckForPromotion(APawns* _pawn, int _destinationIndex);

	UFUNCTION(BlueprintCallable, Category = "Promotion")
	void PromotePawn(APawns* _pawn, EPawnType _newType);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Promotion")
	void OnPawnReadyToPromote(APawns* _pawn);

	// General stuff *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Fire")
	EPawnType currentPawnType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Action")
	class UPlayerAction* CurrentAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Action")
	class UHoveredAction* HoveredAction;

	bool isIndexValid(int index);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Index")
	int CurrentIndex = -999;

	UFUNCTION(BlueprintCallable)
	void SetStartIndex(int _start) { SelectedTileIndex = _start; UE_LOG(LogTemp, Warning, TEXT("Start is set : %d"), _start); };

	int PreviousIndex = CurrentIndex;
	int SelectedCase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tab")
	int X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tab")
	int Y;

	// Action *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

	UFUNCTION(BlueprintCallable)
	void SetNeigbourh(int _currentIndex, EPawnType _pawnType);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void ClearAllHoverStates();

	UFUNCTION(BlueprintCallable)
	void AddStateToTile(ETileState stateToAdd, int InstanceIndex);

	UFUNCTION(BlueprintCallable)
	void RemoveStateToTile(ETileState stateToRemove, int InstanceIndex);

	UFUNCTION(BlueprintCallable)
	void RemoveAllPossible();

	UFUNCTION(BlueprintCallable)
	void Cancle()
	{
		RemoveAllPossible();
		RemoveStateToTile(ETileState::Hovered, CurrentIndex);
		RemoveStateToTile(ETileState::Selected, SelectedCase);
	}

	UFUNCTION(BlueprintCallable)
	void AddTypeToTile(ETileType typeToAdd, int InstanceIndex);

	UFUNCTION(BlueprintCallable)
	void RemoveTypeToTile(ETileType _typeToRemove, int _InstanceIndex);

	ETileType FindTileType(int index);

	UFUNCTION(BlueprintCallable)
	bool MoveValidation(int _indexToCheck);
	UFUNCTION(BlueprintCallable)
	bool AttackValidation();

	UFUNCTION(BlueprintCallable)
	TArray<APawns*> Attack(APawns* _pawn);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileCheck")
	TArray<int> TileShowed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TArray<int> TileInRangeForAttack;

	UFUNCTION(BlueprintCallable)
	void Moving(int _currentIndex, APawns* _pawn);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Sound")
	void PlayMouveSound();

	UFUNCTION(BlueprintCallable)
	void ShowAttackRange(int _currentIndex, APawns* _pawn);

	UFUNCTION(BlueprintCallable)
	void SpawnPawns(int _tileIndex, APawns* _pawn);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Bonus Turn
	TMap<int, int> DownedPawnTurnCounters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	int MaxDownedTurns = 3;

	// Opponant *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
	FTimerHandle EnemyActionTimerHandle;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input System")
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input System")
	class UInputAction* SelectAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input System")
	class UInputAction* UnSelectAction;

	UFUNCTION()
	void OnConstruction(const FTransform& Transform) override;

	void UnHoverTile(int index);

	void ChangeColor(int InstanceIndex, FLinearColor color);

	void OnSelect(const struct FInputActionValue& Value);

	void OnUnSelect(const struct FInputActionValue& Value);

	UPROPERTY()
	TMap <int, FTileData> TileMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors")
	TMap <ETileState, FLinearColor> ColorMap;

	FLinearColor GetColorFromStates(TArray<ETileState> ArrayToCheck);

	void UpdateTileVisuals(int InstanceIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UInstancedStaticMeshComponent* ISM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UMaterialInterface* Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tab")
	float TileSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tab")
	APlayerController* PlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tab")
	FHitResult Hit;
	int SelectedTileIndex;

	// Check des différents déplacement
	void PawnCheck(int _currentIndex)
	{
		int _direction = Y;
		if (PawnMap.Contains(_currentIndex) && PawnMap[_currentIndex]->PawnStruct.PawnTeam == ETeam::Opponent)
		{
			_direction = -Y;
		}

		int _forwardTile = _currentIndex + _direction;
		if (_forwardTile >= 0 && _forwardTile < X * Y)
		{
			TileShowed.Emplace(_forwardTile);

			if (_forwardTile % Y != 0)
			{
				int DiagLeft = _forwardTile - 1;
				if (PawnMap.Contains(DiagLeft) && PawnMap[DiagLeft]->PawnStruct.PawnStats.Health <= 0)
				{
					TileShowed.Emplace(DiagLeft);
				}
			}

			if ((_forwardTile + 1) % Y != 0)
			{
				int DiagRight = _forwardTile + 1;
				if (PawnMap.Contains(DiagRight) && PawnMap[DiagRight]->PawnStruct.PawnStats.Health <= 0)
				{
					TileShowed.Emplace(DiagRight);
				}
			}
		}
	}
	void KingCheck(int _currentIndex)
	{
		if ((_currentIndex) > Y)
		{
			if ((_currentIndex - Y) % Y != 0)
			{
				TileShowed.Emplace(_currentIndex - Y - 1);
			}

			TileShowed.Emplace(_currentIndex - Y);

			if ((_currentIndex - Y + 1) % Y != 0)
			{
				TileShowed.Emplace(_currentIndex - Y + 1);
			}

		}

		if ((_currentIndex + Y) < X * Y)
		{
			if ((_currentIndex + Y) % Y != 0)
			{
				TileShowed.Emplace(_currentIndex + Y - 1);
			}

			TileShowed.Emplace(_currentIndex + Y);

			if ((_currentIndex + Y + 1) % Y != 0)
			{
				TileShowed.Emplace(_currentIndex + Y + 1);
			}
		}

		if ((_currentIndex + 1) % Y != 0)
		{
			TileShowed.Emplace(_currentIndex + 1);
		}
		//Si c'est vrai on check pas a droite
		if ((_currentIndex) % Y != 0)
		{
			TileShowed.Emplace(_currentIndex - 1);
		}
	}
	void KnightCheck(int _currentIndex)
	{
		// Ici je devrai faire un check pour Y-2 avant Y-1 car la meme si les case -1 -(2*Y) sont conté comme bonne mais en réalité elles sont en dehors du plateaux. Mais vu que ca ne cause pas d'erreur je préfère garder comme ca
		if ((_currentIndex - Y) > 0)
		{
			if ((_currentIndex - 1) % Y != 0 && (_currentIndex) % Y != 0)
			{
				TileShowed.Emplace(_currentIndex - 2 - Y);
			}
			if ((_currentIndex) % Y != 0)
			{
				TileShowed.Emplace(_currentIndex - 1 - (2 * Y));
			}

			if ((_currentIndex + 2) % Y != 0 && (_currentIndex + 1) % Y != 0)
			{
				TileShowed.Emplace(_currentIndex + 2 - Y);

			}
			if ((_currentIndex + 1) % Y != 0)
			{
				TileShowed.Emplace(_currentIndex + 1 - (2 * Y));
			}
		}

		if ((_currentIndex + Y) < X * Y)
		{
			if ((_currentIndex - 1) % Y != 0 && (_currentIndex) % Y != 0)
			{
				TileShowed.Emplace(_currentIndex - 2 + Y);
			}
			if ((_currentIndex) % Y != 0)
			{
				TileShowed.Emplace(_currentIndex - 1 + (2 * Y));
			}

			if ((_currentIndex + 2) % Y != 0 && (_currentIndex + 1) % Y != 0)
			{
				TileShowed.Emplace(_currentIndex + 2 + Y);
			}
			if ((_currentIndex + 1) % Y != 0)
			{
				TileShowed.Emplace(_currentIndex + 1 + (2 * Y));
			}
		}
	}
	void TowerCheck(int _currentIndex)
	{
		int _tempY = _currentIndex % Y;
		int _tempX = _currentIndex / Y;

		for (int i = 1; i <= _tempY; i++)
		{
			int Tile = _currentIndex - i;
			ETileType Type = TileMap[Tile].TypeArray;
			if (Type == ETileType::Blocked) break;
			TileShowed.Emplace(Tile);
			if (Type == ETileType::Occuped) break;
		}

		for (int i = 1; i < Y - _tempY; i++)
		{
			int Tile = _currentIndex + i;
			ETileType Type = TileMap[Tile].TypeArray;
			if (Type == ETileType::Blocked) break;
			TileShowed.Emplace(Tile);
			if (Type == ETileType::Occuped) break;
		}

		for (int i = 1; i <= _tempX; i++)
		{
			int Tile = _currentIndex - (Y * i);
			ETileType Type = TileMap[Tile].TypeArray;
			if (Type == ETileType::Blocked) break;
			TileShowed.Emplace(Tile);
			if (Type == ETileType::Occuped) break;
		}

		for (int i = 1; i < X - _tempX; i++)
		{
			int Tile = _currentIndex + (Y * i);
			ETileType Type = TileMap[Tile].TypeArray;
			if (Type == ETileType::Blocked) break;
			TileShowed.Emplace(Tile);
			if (Type == ETileType::Occuped) break;
		}
	}
	void BishopCheck(int _currentIndex)
	{
		int _tempY = _currentIndex % Y;
		int _tempX = _currentIndex / Y;

		for (int i = 1; _tempX - i >= 0 && _tempY - i >= 0; i++)
		{
			int Tile = _currentIndex - (Y * i) - i;
			ETileType Type = TileMap[Tile].TypeArray;
			if (Type == ETileType::Blocked) break;
			TileShowed.Emplace(Tile);
			if (Type == ETileType::Occuped) break;
		}

		for (int i = 1; _tempX - i >= 0 && _tempY + i < Y; i++)
		{
			int Tile = _currentIndex - (Y * i) + i;
			ETileType Type = TileMap[Tile].TypeArray;
			if (Type == ETileType::Blocked) break;
			TileShowed.Emplace(Tile);
			if (Type == ETileType::Occuped) break;
		}

		for (int i = 1; _tempX + i < X && _tempY - i >= 0; i++)
		{
			int Tile = _currentIndex + (Y * i) - i;
			ETileType Type = TileMap[Tile].TypeArray;
			if (Type == ETileType::Blocked) break;
			TileShowed.Emplace(Tile);
			if (Type == ETileType::Occuped) break;
		}

		for (int i = 1; _tempX + i < X && _tempY + i < Y; i++)
		{
			int Tile = _currentIndex + (Y * i) + i;
			ETileType Type = TileMap[Tile].TypeArray;
			if (Type == ETileType::Blocked) break;
			TileShowed.Emplace(Tile);
			if (Type == ETileType::Occuped) break;
		}
	}
	void QueenCheck(int _currentIndex)
	{
		KingCheck(_currentIndex);
		TowerCheck(_currentIndex);
		BishopCheck(_currentIndex);
	}

	// Check les différentes range d'attaque
	void PawnAttackRange(int _currentIndex, ETeam _team)
	{
		int _direction = (_team == ETeam::Opponent) ? -Y : Y;
		int _forwardTile = _currentIndex + _direction;

		if (_forwardTile >= 0 && _forwardTile < X * Y)
		{
			TileInRangeForAttack.Emplace(_forwardTile);

			if ((_currentIndex + 1) % Y != 0)
			{
				TileInRangeForAttack.Emplace(_forwardTile + 1);
			}
			if ((_currentIndex) % Y != 0)
			{
				TileInRangeForAttack.Emplace(_forwardTile - 1);
			}
		}
	}
	void KingAttackRange(int _currentIndex)
	{
		if ((_currentIndex) >= Y)
		{
			if ((_currentIndex - Y) % Y != 0)
			{
				TileInRangeForAttack.Emplace(_currentIndex - Y - 1);
			}

			TileInRangeForAttack.Emplace(_currentIndex - Y);

			if ((_currentIndex - Y + 1) % Y != 0)
			{
				TileInRangeForAttack.Emplace(_currentIndex - Y + 1);
			}

		}

		if ((_currentIndex + Y) < X * Y)
		{
			if ((_currentIndex + Y) % Y != 0)
			{
				TileInRangeForAttack.Emplace(_currentIndex + Y - 1);
			}

			TileInRangeForAttack.Emplace(_currentIndex + Y);

			if ((_currentIndex + Y + 1) % Y != 0)
			{
				TileInRangeForAttack.Emplace(_currentIndex + Y + 1);
			}
		}

		if ((_currentIndex + 1) % Y != 0)
		{
			TileInRangeForAttack.Emplace(_currentIndex + 1);
		}
		//Si c'est vrai on check pas a droite
		if ((_currentIndex) % Y != 0)
		{
			TileInRangeForAttack.Emplace(_currentIndex - 1);
		}
	}
	void KnightAttackRange(int _currentIndex)
	{
		if ((_currentIndex + 1) % Y != 0)
		{
			TileInRangeForAttack.Emplace(_currentIndex + 1);

			if ((_currentIndex + 2) % Y != 0)
			{
				TileInRangeForAttack.Emplace(_currentIndex + 2);
			}
		}
		//Si c'est vrai on check pas a droite
		if ((_currentIndex) % Y != 0)
		{
			TileInRangeForAttack.Emplace(_currentIndex - 1);

			if ((_currentIndex - 1) % Y != 0)
			{
				TileInRangeForAttack.Emplace(_currentIndex - 2);
			}
		}

		if ((_currentIndex - Y) >= 0)
		{
			TileInRangeForAttack.Emplace(_currentIndex - Y);

			if ((_currentIndex) % Y != 0)
			{
				TileInRangeForAttack.Emplace(_currentIndex - 1 - (2 * Y));
				TileInRangeForAttack.Emplace(_currentIndex - 1 - Y);
				TileInRangeForAttack.Emplace(_currentIndex - (2 * Y));

				if ((_currentIndex - 1) % Y != 0)
				{
					TileInRangeForAttack.Emplace(_currentIndex - 2 - Y);
				}
			}

			if ((_currentIndex + 1) % Y != 0)
			{
				TileInRangeForAttack.Emplace(_currentIndex + 1 - (2 * Y));
				TileInRangeForAttack.Emplace(_currentIndex + 1 - Y);
				TileInRangeForAttack.Emplace(_currentIndex - (2 * Y));

				if ((_currentIndex + 2) % Y != 0)
				{
					TileInRangeForAttack.Emplace(_currentIndex + 2 - Y);
				}
			}
		}

		if ((_currentIndex + Y) < X * Y)
		{
			TileInRangeForAttack.Emplace(_currentIndex + Y);

			if ((_currentIndex) % Y != 0)
			{
				TileInRangeForAttack.Emplace(_currentIndex - 1 + (2 * Y));
				TileInRangeForAttack.Emplace(_currentIndex - 1 + Y);
				TileInRangeForAttack.Emplace(_currentIndex + (2 * Y));

				if ((_currentIndex - 1) % Y != 0)
				{
					TileInRangeForAttack.Emplace(_currentIndex - 2 + Y);
				}
			}

			if ((_currentIndex + 1) % Y != 0)
			{
				TileInRangeForAttack.Emplace(_currentIndex + 1 + (2 * Y));
				TileInRangeForAttack.Emplace(_currentIndex + 1 + Y);
				TileInRangeForAttack.Emplace(_currentIndex + (2 * Y));

				if ((_currentIndex + 2) % Y != 0)
				{
					TileInRangeForAttack.Emplace(_currentIndex + 2 + Y);

				}
			}
		}
	}
	void TowerAttackRange(int _currentIndex)
	{
		int _tempY = _currentIndex % Y;
		int _tempX = _currentIndex / Y;

		for (int i = 1; i <= _tempY; i++)
		{
			int Tile = _currentIndex - i;
			ETileType Type = TileMap[Tile].TypeArray;
			if (Type == ETileType::Blocked) break;
			TileInRangeForAttack.Emplace(Tile);
			if (Type == ETileType::Occuped) break;
		}

		for (int i = 1; i < Y - _tempY; i++)
		{
			int Tile = _currentIndex + i;
			ETileType Type = TileMap[Tile].TypeArray;
			if (Type == ETileType::Blocked) break;
			TileInRangeForAttack.Emplace(Tile);
			if (Type == ETileType::Occuped) break;
		}

		for (int i = 1; i <= _tempX; i++)
		{
			int Tile = _currentIndex - (Y * i);
			ETileType Type = TileMap[Tile].TypeArray;
			if (Type == ETileType::Blocked) break;
			TileInRangeForAttack.Emplace(Tile);
			if (Type == ETileType::Occuped) break;
		}

		for (int i = 1; i < X - _tempX; i++)
		{
			int Tile = _currentIndex + (Y * i);
			ETileType Type = TileMap[Tile].TypeArray;
			if (Type == ETileType::Blocked) break;
			TileInRangeForAttack.Emplace(Tile);
			if (Type == ETileType::Occuped) break;
		}
	}
	void BishopAttackRange(int _currentIndex)
	{
		if ((_currentIndex + 1) % Y != 0)
		{
			TileInRangeForAttack.Emplace(_currentIndex + 1);
		}
		//Si c'est vrai on check pas a droite
		if ((_currentIndex) % Y != 0)
		{
			TileInRangeForAttack.Emplace(_currentIndex - 1);
		}

		if ((_currentIndex - Y) >= 0)
		{
			TileInRangeForAttack.Emplace(_currentIndex - Y);

			if ((_currentIndex) % Y != 0)
			{
				TileInRangeForAttack.Emplace(_currentIndex - 1 - (2 * Y));

				if ((_currentIndex - 1) % Y != 0)
				{
					TileInRangeForAttack.Emplace(_currentIndex - 2 - Y);
				}
			}

			if ((_currentIndex + 1) % Y != 0)
			{
				TileInRangeForAttack.Emplace(_currentIndex + 1 - (2 * Y));

				if ((_currentIndex + 2) % Y != 0)
				{
					TileInRangeForAttack.Emplace(_currentIndex + 2 - Y);
				}
			}
		}

		if ((_currentIndex + Y) < X * Y)
		{
			TileInRangeForAttack.Emplace(_currentIndex + Y);

			if ((_currentIndex) % Y != 0)
			{
				TileInRangeForAttack.Emplace(_currentIndex - 1 + (2 * Y));

				if ((_currentIndex - 1) % Y != 0)
				{
					TileInRangeForAttack.Emplace(_currentIndex - 2 + Y);
				}
			}

			if ((_currentIndex + 1) % Y != 0)
			{
				TileInRangeForAttack.Emplace(_currentIndex + 1 + (2 * Y));

				if ((_currentIndex + 2) % Y != 0)
				{
					TileInRangeForAttack.Emplace(_currentIndex + 2 + Y);
				}
			}
		}
	}
	void QueenAttackRange(int _currentIndex)
	{
		KingAttackRange(_currentIndex);
		KnightAttackRange(_currentIndex);
		int _tempX = _currentIndex / Y;
		int _tempY = _currentIndex % Y;
		if (_tempY + 2 < Y)
		{
			TileInRangeForAttack.Emplace(_currentIndex + 2);
			if (_tempY + 3 < Y)
			{
				TileInRangeForAttack.Emplace(_currentIndex + 3);
			}
		}
		if (_tempY - 2 >= 0)
		{
			TileInRangeForAttack.Emplace(_currentIndex - 2);
			if (_tempY - 3 >= 0)
			{
				TileInRangeForAttack.Emplace(_currentIndex - 3);
			}
		}
		if (_tempX + 2 < X)
		{
			TileInRangeForAttack.Emplace(_currentIndex + (2 * Y));
			if (_tempX + 3 < X)
			{
				TileInRangeForAttack.Emplace(_currentIndex + (3 * Y));
			}
		}
		if (_tempX - 2 >= 0)
		{
			TileInRangeForAttack.Emplace(_currentIndex - (2 * Y));
			if (_tempX - 3 >= 0)
			{
				TileInRangeForAttack.Emplace(_currentIndex - (3 * Y));
			}
		}
	}

	bool CheckStalemate(int _currentIndex);
	void Stalemate(APawns* _pawnTaken);

	TMap <int, APawns*> PawnMap;

};