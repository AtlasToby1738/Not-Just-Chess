#include "PathfindingAction.h"
#include "GridManager.h"
#include <cstdlib>
#include <algorithm>

TArray<int> UPathfindingAction::FindPath(int Target, int Start)
{
	if (!GridRef) return TArray<int> {0};
	if (!GridRef->isIndexValid(GridRef->CurrentIndex)) return TArray<int> {0};

	InitGrid();

	TArray<int> Path;
	startIndex.Key = Start;
	startIndex.Value = FNodeData(0, 0, 0, -1);
	target.Key = Target;

	if (target.Key == startIndex.Key) return Path;
	
	OPEN.Emplace(startIndex.Key, startIndex.Value);

	//Loop complete
	while (!flag && OPEN.Num() > 0)
	{
		auto thisNode = OPEN.begin();
		current.Key = thisNode.Key();
		current.Value = thisNode.Value();

		for (TPair<int, FNodeData>& index : OPEN)
		{
			if (index.Value.f_cost < current.Value.f_cost)
			{
				current.Key = index.Key;
				current.Value = index.Value;
			}
		}

		OPEN.Remove(current.Key);
		CLOSED.Emplace(current.Key, current.Value);

		if (current.Key == target.Key)
		{
			flag = true;
		}

		NEIGHBOURS = GetAllNeighbours(current.Key, current.Value.g_cost);

		for (TPair<int, FNodeData> neighbourToCheck : NEIGHBOURS)
		{
			int neighbourKey = neighbourToCheck.Key;
			FNodeData neihgbourData = neighbourToCheck.Value;

			if (GridRef->FindTileType(neighbourKey) == ETileType::Blocked || CLOSED.Contains(neighbourToCheck.Key))
			{
				continue;
			}

			if (neihgbourData.h_cost + neihgbourData.g_cost < current.Value.f_cost || !OPEN.Contains(neighbourToCheck.Key))
			{
				neihgbourData.f_cost = neihgbourData.g_cost + neihgbourData.h_cost;
				neihgbourData.parent_index = current.Key;

				if (!OPEN.Contains(neighbourToCheck.Key))
					OPEN.Add(neighbourKey, neihgbourData);
				else
					OPEN[neighbourKey] = neihgbourData;
			}
		}
	}

	if (flag)
	{
		int cursor = target.Key;
		int min = 0;
		int max = GridRef->X * GridRef->Y;

		while (cursor != startIndex.Key && min < max)
		{
			if (!CLOSED.Contains(target.Key) || CLOSED[target.Key].parent_index == -1) break;
			Path.Insert(cursor, 0);
			cursor = CLOSED[cursor].parent_index;
			min++;
		}
		Path.Insert(startIndex.Key, 0);
	}

	return Path;
}


TMap<int, FNodeData> UPathfindingAction::GetAllNeighbours(int NodeToCheck, int parentGCost)
{
	TMap<int, FNodeData> NEWNEIGHBOURS;

	int col = NodeToCheck / GridRef->Y;
	int row = NodeToCheck % GridRef->Y;

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if (i == 0 && j == 0) continue;

			int newCol = col + i;
			int newRow = row + j;

			if (newCol < 0 || newCol >= GridRef->X) continue;
			if (newRow < 0 || newRow >= GridRef->Y) continue;

			int neighbourIndex = newCol * GridRef->Y + newRow;
			NEWNEIGHBOURS.Emplace(neighbourIndex, GetPositionOfTheTile(startIndex.Key, target.Key, neighbourIndex, parentGCost));
		}
	}

	return NEWNEIGHBOURS;
}

FNodeData UPathfindingAction::GetPositionOfTheTile(int Start, int Target, int nodeIndex, int parentGCost)
{
	int dx_g = abs((nodeIndex / GridRef->Y) - (Start / GridRef->Y));
	int dy_g = abs((nodeIndex % GridRef->Y) - (Start % GridRef->Y));
	bool isDiagonal = (dx_g > 0 && dy_g > 0);
	int g = parentGCost + (isDiagonal ? 14 : 10);

	int dx_h = abs((nodeIndex / GridRef->Y) - (Target / GridRef->Y));
	int dy_h = abs((nodeIndex % GridRef->Y) - (Target % GridRef->Y));
	int h = 14 * std::min(dx_h, dy_h) + 10 * abs(dx_h - dy_h);

	return FNodeData(g, h, g + h, -1);
}

void UPathfindingAction::RightClick(int Index)
{
	InitGrid();
}

void UPathfindingAction::InitGrid()
{
	for (auto neigh : CLOSED)
	{
		GridRef->RemoveStateToTile(ETileState::Way, neigh.Key);
	}

	//Inisialisation + Reset
	OPEN.Empty();
	CLOSED.Empty();
	NEIGHBOURS.Empty();

	flag = false;

	startIndex.Key = 0;
	startIndex.Value = FNodeData(0, 0, 0, -1);
}
