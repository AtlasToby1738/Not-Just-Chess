#pragma once
#include "CoreMinimal.h"
#include "PlayerAction.h"
#include "EnemyAction.generated.h"

class APawns;

struct FAIMoveEvaluation
{
	int PawnIndex = -1;
	APawns* Pawn = nullptr;
	int moveDestination = -1;
	int attackTargetTile = -1;
	bool isStalemateCapture = false;
	int score = -999999;
};

UCLASS()
class TRPG_GAME_API UEnemyAction : public UPlayerAction
{
	GENERATED_BODY()

public:
	bool PlayAction(const TArray<TPair<int, APawns*>>& _enemyPawns, const TArray<TPair<int, APawns*>>& _playerPawns);
	void ResetAction();

protected:
	FAIMoveEvaluation FindBestMove(const TArray<TPair<int, APawns*>>& _enemyPawns, const TArray<TPair<int, APawns*>>& _playerPawns);
	int GetTileDistance(int A, int B) const;

	bool hasPendingAttack = false;
	int pendingAttackPawnIndex = -1;
	APawns* pendingAttackPawn = nullptr;
	int endingAttackTile = -1;
};