#include "EnemyAction.h"
#include "GridManager.h"
#include "HoveredAction.h"
#include "Pawns.h"

int UEnemyAction::GetTileDistance(int A, int B) const
{
	int _colA = A / GridRef->Y;
	int _rowA = A % GridRef->Y;
	int _colB = B / GridRef->Y;
	int _rowB = B % GridRef->Y;
	return FMath::Max(FMath::Abs(_colA - _colB), FMath::Abs(_rowA - _rowB));
}

FAIMoveEvaluation UEnemyAction::FindBestMove(const TArray<TPair<int, APawns*>>& _enemyPawns, const TArray<TPair<int, APawns*>>& _playerPawns)
{
	FAIMoveEvaluation _best;
	if (!GridRef) return _best;

	for (const TPair<int, APawns*>& _enemy : _enemyPawns)
	{
		int _pawnIndex = _enemy.Key;
		APawns* _pawn = _enemy.Value;
		if (!_pawn || _pawn->PawnStruct.PawnStats.Health <= 0) continue;

		EPawnType _pawnType = _pawn->PawnStruct.PawnType;
		GridRef->currentPawnType = _pawnType;

		GridRef->SetNeigbourh(_pawnIndex, _pawnType);
		TArray<int> _moveOptions = GridRef->TileShowed;
		GridRef->RemoveAllPossible();

		TArray<int> _destinations;
		_destinations.Add(_pawnIndex);
		for (int MoveTile : _moveOptions)
		{
			_destinations.AddUnique(MoveTile);
		}

		for (int Dest : _destinations)
		{
			bool _isOwnTile = (Dest == _pawnIndex);
			bool _isStalemateCapture = false;

			if (!_isOwnTile)
			{
				if (Dest < 0 || Dest >= GridRef->X * GridRef->Y) continue;

				ETileType _destType = GridRef->FindTileType(Dest);
				if (_destType == ETileType::Occuped)
				{
					for (const TPair<int, APawns*>& PlayerElem : _playerPawns)
					{
						if (PlayerElem.Key == Dest && PlayerElem.Value && PlayerElem.Value->PawnStruct.PawnStats.Health <= 0)
						{
							_isStalemateCapture = true;
							break;
						}
					}
					if (!_isStalemateCapture) continue;
				}
				else if (_destType != ETileType::Normal)
				{
					continue;
				}
			}

			GridRef->ShowAttackRange(Dest, _pawn);
			TArray<int> _attackOptions = GridRef->TileInRangeForAttack;
			GridRef->RemoveAllPossible();

			bool _isKingAttack = (_pawnType == EPawnType::King);
			bool _isQueenAttack = (_pawnType == EPawnType::Queen);

			int _bestAttackTile = -1;
			int _totalAttackScore = 0;

			if (_isKingAttack)
			{
				for (int _attackTile : _attackOptions)
				{
					if (_attackTile < 0 || _attackTile >= GridRef->X * GridRef->Y) continue;

					for (const TPair<int, APawns*>& _player : _playerPawns)
					{
						if (_player.Key != _attackTile) continue;

						APawns* Target = _player.Value;
						if (!Target || Target->PawnStruct.PawnStats.Health <= 0) continue;

						int TargetScore = 1000;
						if (Target->PawnStruct.PawnStats.Health <= _pawn->PawnStruct.PawnStats.Attack)
						{
							TargetScore += 500;
						}
						_totalAttackScore += TargetScore;

						if (_bestAttackTile == -1) _bestAttackTile = _attackTile;
					}
				}
			}
			else if (_isQueenAttack)
			{
				int _bestCenterScore = 0;
				int _bestCenter = -1;

				for (int _center : _attackOptions)
				{
					if (_center < 0 || _center >= GridRef->X * GridRef->Y) continue;

					TArray<int> Cross = { _center, _center - 1, _center + 1, _center - GridRef->Y, _center + GridRef->Y };
					int _centerScore = 0;

					for (int _cross : Cross)
					{
						if (!_attackOptions.Contains(_cross)) continue;

						for (const TPair<int, APawns*>& _player : _playerPawns)
						{
							if (_player.Key != _cross) continue;

							APawns* Target = _player.Value;
							if (!Target || Target->PawnStruct.PawnStats.Health <= 0) continue;

							int _targetScore = 1000;
							if (Target->PawnStruct.PawnStats.Health <= _pawn->PawnStruct.PawnStats.Attack)
							{
								_targetScore += 500;
							}
							_centerScore += _targetScore;
						}
					}

					if (_centerScore > _bestCenterScore)
					{
						_bestCenterScore = _centerScore;
						_bestCenter = _center;
					}
				}

				if (_bestCenter != -1)
				{
					_bestAttackTile = _bestCenter;
					_totalAttackScore = _bestCenterScore;
				}
			}
			else
			{
				int _bestAttackScore = -1;
				for (int _attackTile : _attackOptions)
				{
					if (_attackTile < 0 || _attackTile >= GridRef->X * GridRef->Y) continue;

					for (const TPair<int, APawns*>& _player : _playerPawns)
					{
						if (_player.Key != _attackTile) continue;

						APawns* _target = _player.Value;
						if (!_target || _target->PawnStruct.PawnStats.Health <= 0) continue;

						int _attackScore = 1000;
						if (_target->PawnStruct.PawnStats.Health <= _pawn->PawnStruct.PawnStats.Attack)
						{
							_attackScore += 500;
						}

						if (_attackScore > _bestAttackScore)
						{
							_bestAttackScore = _attackScore;
							_bestAttackTile = _attackTile;
						}
					}
				}
				_totalAttackScore = _bestAttackScore;
			}

			int _baseScore = _isStalemateCapture ? 2000 : 0;
			bool _wouldPromote = false;

			if (!_isOwnTile && _pawnType == EPawnType::Pawn && !_isStalemateCapture)
			{
				_wouldPromote = GridRef->CheckForPromotion(_pawn, Dest);
				if (_wouldPromote) _baseScore = 1200;
			}

			int _score;

			if (_bestAttackTile != -1)
			{
				_score = _baseScore + _totalAttackScore;
			}
			else if (_isStalemateCapture || _wouldPromote)
			{
				_score = _baseScore;
			}
			else
			{
				if (_isOwnTile) continue;

				int _bestPlayerDistance = TNumericLimits<int>::Max();
				for (const TPair<int, APawns*>& PlayerElem : _playerPawns)
				{
					if (!PlayerElem.Value || PlayerElem.Value->PawnStruct.PawnStats.Health <= 0) continue;
					int Dist = GetTileDistance(Dest, PlayerElem.Key);
					_bestPlayerDistance = FMath::Min(_bestPlayerDistance, Dist);
				}
				if (_bestPlayerDistance == TNumericLimits<int>::Max()) continue;
				_score = -_bestPlayerDistance * 10;
			}

			if (_score > _best.score)
			{
				_best.score = _score;
				_best.PawnIndex = _pawnIndex;
				_best.Pawn = _pawn;
				_best.moveDestination = _isOwnTile ? -1 : Dest;
				_best.attackTargetTile = _bestAttackTile;
				_best.isStalemateCapture = _isStalemateCapture;
			}
		}
	}

	return _best;
}

void UEnemyAction::ResetAction()
{
	hasPendingAttack = false;
	pendingAttackPawnIndex = -1;
	pendingAttackPawn = nullptr;
	endingAttackTile = -1;
}

bool UEnemyAction::PlayAction(const TArray<TPair<int, APawns*>>& EnemyPawns, const TArray<TPair<int, APawns*>>& PlayerPawns)
{
	if (!GridRef) return false;

	if (hasPendingAttack)
	{
		hasPendingAttack = false;

		GridRef->currentPawnType = pendingAttackPawn->PawnStruct.PawnType;
		GridRef->ShowAttackRange(pendingAttackPawnIndex, pendingAttackPawn);
		GridRef->CurrentIndex = endingAttackTile;

		if (pendingAttackPawn->PawnStruct.PawnType == EPawnType::Queen)
		{
			GridRef->SetQueenAttackCross(endingAttackTile);
		}

		if (GridRef->AttackValidation())
		{
			GridRef->Attack(pendingAttackPawn);
		}
		GridRef->RemoveAllPossible();

		return true;
	}

	FAIMoveEvaluation _best = FindBestMove(EnemyPawns, PlayerPawns);

	if (_best.PawnIndex == -1 || !_best.Pawn) return false;

	GridRef->currentPawnType = _best.Pawn->PawnStruct.PawnType;

	if (_best.moveDestination != -1)
	{
		GridRef->SetNeigbourh(_best.PawnIndex, _best.Pawn->PawnStruct.PawnType);
		GridRef->CurrentIndex = _best.moveDestination;

		if (GridRef->MoveValidation(_best.moveDestination))
		{
			GridRef->Moving(_best.moveDestination, _best.Pawn);
			GridRef->MovePawnVisually(_best.Pawn, _best.moveDestination);

			if (GridRef->CheckForPromotion(_best.Pawn, _best.moveDestination))
			{
				GridRef->PromotePawn(_best.Pawn, EPawnType::Queen);
				_best.Pawn->UpdateVisualOnPromotionIA();
			}

			if (_best.attackTargetTile != -1)
			{
				hasPendingAttack = true;
				pendingAttackPawnIndex = _best.moveDestination;
				pendingAttackPawn = _best.Pawn;
				endingAttackTile = _best.attackTargetTile;
			}
		}
		GridRef->RemoveAllPossible();
		return true;
	}

	if (_best.attackTargetTile != -1)
	{
		GridRef->ShowAttackRange(_best.PawnIndex, _best.Pawn);
		GridRef->CurrentIndex = _best.attackTargetTile;

		if (_best.Pawn->PawnStruct.PawnType == EPawnType::Queen)
		{
			GridRef->SetQueenAttackCross(_best.attackTargetTile);
		}

		if (GridRef->AttackValidation())
		{
			GridRef->Attack(_best.Pawn);
		}
		GridRef->RemoveAllPossible();
		return true;
	}

	return false;
}