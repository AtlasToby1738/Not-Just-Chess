#pragma once

#include "CoreMinimal.h"
#include "PlayerAction.h"
#include "PathfindingAction.generated.h"

USTRUCT(BlueprintType)
struct FNodeData
{
	GENERATED_BODY()

	int g_cost = 0;
	int h_cost = 0;
	int f_cost = 0;
	int parent_index = -1;

	FNodeData() {}
	FNodeData(int g, int h, int f, int p) : g_cost(g), h_cost(h), f_cost(f), parent_index(p) {}

};

UCLASS()
class TRPG_GAME_API UPathfindingAction : public UPlayerAction
{
	GENERATED_BODY()
	
public:
	TArray<int> FindPath(int Target, int Start);
	void RightClick(int Index) override;

	TPair<int, FNodeData> startIndex;
	TPair<int, FNodeData> current;
	TPair<int, FNodeData> target;

	TMap<int, FNodeData> OPEN;
	TMap<int, FNodeData> CLOSED;
	TMap<int, FNodeData> NEIGHBOURS;

	bool flag = false;

	TMap<int, FNodeData> GetAllNeighbours(int NodeToCheck, int parentGCost);
	FNodeData GetPositionOfTheTile(int Start, int Target, int nodeIndex, int parentGCost);
	void InitGrid();

};
