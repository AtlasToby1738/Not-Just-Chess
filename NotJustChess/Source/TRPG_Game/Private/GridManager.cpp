#include "GridManager.h"
#include <Components/InstancedStaticMeshComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Components/PrimitiveComponent.h>
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include "EnemyAction.h"
#include "PlayerAction.h"

#define ECC_Grid ECC_GameTraceChannel1
#define ECC_Player ECC_GameTraceChannel2

// Sets default values
AGridManager::AGridManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	ISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISM"));
	ISM->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	ISM->SetCollisionResponseToChannel(ECollisionChannel::ECC_Grid, ECollisionResponse::ECR_Block);
	ISM->SetCollisionResponseToChannel(ECollisionChannel::ECC_Player, ECollisionResponse::ECR_Block);
	ISM->NumCustomDataFloats = 4;
}

void AGridManager::PromotePawn(APawns* _pawn, EPawnType _newType)
{
	if (!_pawn) return;
	_pawn->PawnStruct.PawnType = _newType;
	currentPawnType = _newType;
	UE_LOG(LogTemp, Warning, TEXT("Pion promu en %d"), (int)_newType);
}

bool AGridManager::CheckForPromotion(APawns* Pawn, int DestinationIndex)
{
	if (!Pawn || Pawn->PawnStruct.PawnType != EPawnType::Pawn) return false;

	// Attention : avec un index = Row * Y + Col, la division par Y donne la LIGNE (Row), pas la colonne
	int Row = DestinationIndex / Y;
	int Col = DestinationIndex % Y;

	UE_LOG(LogTemp, Warning, TEXT("CheckForPromotion - Index: %d, X: %d, Y: %d, Row calculee: %d, Col calculee: %d, Team: %d"),
		DestinationIndex, X, Y, Row, Col, (int)Pawn->PawnStruct.PawnTeam);
	UE_LOG(LogTemp, Warning, TEXT("Deplacement pion - StartIndex: %d, DestinationIndex envoye: %d"), Pawn->Index, DestinationIndex);

	if (Pawn->PawnStruct.PawnTeam == ETeam::Ally)
	{
		return Row == X - 1;
	}
	else
	{
		return Row == 0;
	}
}

void AGridManager::PlayEnemyTurn()
{
	ProcessDownedPawns();
	ClearAllHoverStates();
	if (!EnemyAI) return;

	EnemyAI->ResetAction();
	ActionPointsRemaining = BaseActionPoints;
	PlayNextEnemyAction();
}

void AGridManager::ProcessDownedPawns()
{
	TArray<int> ToForceKill;

	for (TPair<int, int>& Elem : DownedPawnTurnCounters)
	{
		if (!PawnMap.Contains(Elem.Key)) continue;

		Elem.Value++;

		int TurnsRemaining = FMath::Max(0, MaxDownedTurns - Elem.Value);
		OnDownedPawnCountdownUpdated(PawnMap[Elem.Key], TurnsRemaining);

		if (Elem.Value >= MaxDownedTurns)
		{
			ToForceKill.Add(Elem.Key);
		}
	}

	for (int Index : ToForceKill)
	{
		if (PawnMap.Contains(Index))
		{
			APawns* DeadPawn = PawnMap[Index];
			UE_LOG(LogTemp, Warning, TEXT("Pion %d succombe a ses blessures"), Index);
			DownedPawnTurnCounters.Remove(Index);
			PawnMap.Remove(Index);
			DeadPawn->Destroy();
		}
	}
}

void AGridManager::SetQueenAttackCross(int Center)
{
	AddStateToTile(ETileState::PreFire, Center);
	if (TileInRangeForAttack.Contains(Center - 1))
		AddStateToTile(ETileState::PreFire, Center - 1);
	if (TileInRangeForAttack.Contains(Center + 1))
		AddStateToTile(ETileState::PreFire, Center + 1);
	if (TileInRangeForAttack.Contains(Center + Y))
		AddStateToTile(ETileState::PreFire, Center + Y);
	if (TileInRangeForAttack.Contains(Center - Y))
		AddStateToTile(ETileState::PreFire, Center - Y);
}

void AGridManager::PlayNextEnemyAction()
{
	if (ActionPointsRemaining <= 0)
	{
		EndEnemyTurn();
		return;
	}

	ActionPointsRemaining--;
	CountEnemyPA(ActionPointsRemaining);

	TArray<TPair<int, APawns*>> EnemyPawns;
	TArray<TPair<int, APawns*>> PlayerPawns;

	for (const TPair<int, APawns*>& Elem : PawnMap)
	{
		if (!Elem.Value) continue;

		if (Elem.Value->PawnStruct.PawnTeam == ETeam::Opponent)
		{
			EnemyPawns.Add(Elem);
		}
		else
		{
			PlayerPawns.Add(Elem);
		}
	}

	bool bActionPlayed = EnemyAI->PlayAction(EnemyPawns, PlayerPawns);

	if (CheckForEnd())
	{
		return;
	}

	if (!bActionPlayed)
	{
		EndEnemyTurn();
		return;
	}

	GetWorldTimerManager().SetTimer(EnemyActionTimerHandle, this, &AGridManager::PlayNextEnemyAction, EnemyActionDelay, false);

}

void AGridManager::SetNeigbourh(int _currentIndex, EPawnType _pawnType)
{
	SelectedCase = _currentIndex;
	TileShowed.Empty();
	UE_LOG(LogTemp, Warning, TEXT("The integer value is: %d"), _currentIndex);
	switch (_pawnType)
	{
	case EPawnType::None:
		break;
	case EPawnType::Pawn:
		PawnCheck(_currentIndex);
		break;
	case EPawnType::King:
		KingCheck(_currentIndex);
		break;
	case EPawnType::Knight:
		KnightCheck(_currentIndex);
		break;
	case EPawnType::Tower:
	{
		TowerCheck(_currentIndex);
	}
	break;
	case EPawnType::Bishop:
	{
		BishopCheck(_currentIndex);
	}
	break;
	case EPawnType::Queen:
	{
		QueenCheck(_currentIndex);
	}
	break;
	case EPawnType::Max:
		break;
	default:
		break;
	}
	if (!TileShowed.IsEmpty())
	{
		for (int i = 0; i < TileShowed.Num(); i++)
		{
			int Tile = TileShowed[i];

			if (TileMap.Contains(Tile) &&PawnMap.Contains(Tile) && PawnMap[Tile]->PawnStruct.PawnStats.Health <= 0)
			{
				RemoveStateToTile(ETileState::Occuped, Tile);
			}

			AddStateToTile(ETileState::Accessible, Tile);
		}
	}
	return;
}

bool AGridManager::MoveValidation(int _indexToCheck)
{
	if (TileMap[_indexToCheck].TypeArray == ETileType::Blocked || !TileShowed.Contains(_indexToCheck)) { UE_LOG(LogTemp, Warning, TEXT("TheCase is Bloqued or not in Range: %d"), _indexToCheck); return false; }

	else if (!CheckStalemate(_indexToCheck)) { UE_LOG(LogTemp, Warning, TEXT("The Index is Occuped and can't get taken: %d"), _indexToCheck); return false; }

	// ICI COMMENT AVOIR LE SELECTEDTYLEINDEX (soit avec l'action button select, soit avec l'acteur et sont int "voir bp_test")
	else
	{
		AddTypeToTile(ETileType::Occuped, _indexToCheck);
		return true;
	}
}

bool AGridManager::AttackValidation()
{
	UE_LOG(LogTemp, Warning, TEXT("--- PawnMap dump ---"));
	for (const TPair<int, APawns*>& Elem : PawnMap)
	{
		if (!Elem.Value) continue;
		UE_LOG(LogTemp, Warning, TEXT("  Index %d - Type: %d, Team: %d, HP: %d"),
			Elem.Key,
			(int)Elem.Value->PawnStruct.PawnType,
			(int)Elem.Value->PawnStruct.PawnTeam,
			Elem.Value->PawnStruct.PawnStats.Health);
	}
	UE_LOG(LogTemp, Warning, TEXT("--- SelectedCase: %d, CurrentIndex: %d ---"), SelectedCase, CurrentIndex);

	if (currentPawnType == EPawnType::King)
	{
		for (int _tile : TileInRangeForAttack)
		{
			if (PawnMap.Contains(_tile) && PawnMap.Contains(SelectedCase))
			{
				bool bDifferentTeam = PawnMap[_tile]->PawnStruct.PawnTeam != PawnMap[SelectedCase]->PawnStruct.PawnTeam;
				bool bTargetAlive = PawnMap[_tile]->PawnStruct.PawnStats.Health > 0;
				if (bDifferentTeam && bTargetAlive)
				{
					UE_LOG(LogTemp, Warning, TEXT("King peut attaquer la case %d"), _tile);
					return true;
				}
			}
		}
	}


	if (currentPawnType == EPawnType::Queen)
	{
		TArray<int> Cross = { CurrentIndex, CurrentIndex - 1, CurrentIndex + 1, CurrentIndex - Y, CurrentIndex + Y };
		UE_LOG(LogTemp, Warning, TEXT("Queen check - CurrentIndex: %d, SelectedCase: %d"), CurrentIndex, SelectedCase);

		for (int _tile : Cross)
		{
			bool bInRange = TileInRangeForAttack.Contains(_tile);
			UE_LOG(LogTemp, Warning, TEXT("  Cross tile %d - InRange: %d"), _tile, bInRange);

			if (!bInRange) continue;

			bool bTileHasPawn = PawnMap.Contains(_tile);
			bool bSelectedHasPawn = PawnMap.Contains(SelectedCase);
			UE_LOG(LogTemp, Warning, TEXT("  Tile %d - HasPawn: %d, SelectedHasPawn: %d"), _tile, bTileHasPawn, bSelectedHasPawn);

			if (bTileHasPawn && bSelectedHasPawn)
			{
				bool bDifferentTeam = PawnMap[_tile]->PawnStruct.PawnTeam != PawnMap[SelectedCase]->PawnStruct.PawnTeam;
				bool bTargetAlive = PawnMap[_tile]->PawnStruct.PawnStats.Health > 0;
				UE_LOG(LogTemp, Warning, TEXT("  Tile %d - DifferentTeam: %d, TargetAlive: %d"), _tile, bDifferentTeam, bTargetAlive);

				if (bDifferentTeam && bTargetAlive)
				{
					return true;
				}
			}
		}
	}

	if (PawnMap.Contains(CurrentIndex) && PawnMap.Contains(SelectedCase))
	{
		bool bIsOccupied = TileMap[CurrentIndex].TypeArray == ETileType::Occuped;
		bool bIsVulnerable = TileMap[CurrentIndex].StateArray.Contains(ETileState::Vulnerable);
		bool bDifferentTeam = PawnMap[CurrentIndex]->PawnStruct.PawnTeam != PawnMap[SelectedCase]->PawnStruct.PawnTeam;
		bool bTargetAlive = PawnMap[CurrentIndex]->PawnStruct.PawnStats.Health > 0;

		switch (currentPawnType)
		{
		case EPawnType::Pawn:
		{
			if (bIsOccupied && bIsVulnerable && bDifferentTeam && bTargetAlive)
			{
				return true;
			}
			break;
		}

		case EPawnType::Knight:
		{
			if (bIsOccupied && bIsVulnerable && bDifferentTeam && bTargetAlive)
			{
				return true;
			}
			break;
		}

		case EPawnType::Tower:
		{
			if (bIsOccupied && bIsVulnerable && bDifferentTeam && bTargetAlive)
			{
				return true;
			}
			break;
		}

		case EPawnType::Bishop:
		{
			if (bIsOccupied && bIsVulnerable && bDifferentTeam && bTargetAlive)
			{
				return true;
			}
			break;
		}
		}

		UE_LOG(LogTemp, Warning, TEXT("CurrentIndex == SelectedCase ? %d"), CurrentIndex == SelectedCase);

		UE_LOG(LogTemp, Warning, TEXT("Attacker (SelectedCase %d) - Team: %d, Health: %d"),
			SelectedCase, (int)PawnMap[SelectedCase]->PawnStruct.PawnTeam, PawnMap[SelectedCase]->PawnStruct.PawnStats.Health);

		UE_LOG(LogTemp, Warning, TEXT("Target (CurrentIndex %d) - Team: %d, Health: %d"),
			CurrentIndex, (int)PawnMap[CurrentIndex]->PawnStruct.PawnTeam, PawnMap[CurrentIndex]->PawnStruct.PawnStats.Health);

		UE_LOG(LogTemp, Warning, TEXT("Occupied: %d | Vulnerable: %d | DifferentTeam: %d | TargetAlive: %d"),
			bIsOccupied, bIsVulnerable, bDifferentTeam, bTargetAlive);

		UE_LOG(LogTemp, Warning, TEXT("Attacker ptr: %p | Target ptr: %p"),
			PawnMap[SelectedCase], PawnMap[CurrentIndex]);
		return false;

	}
	UE_LOG(LogTemp, Warning, TEXT("AttackValidation - CurrentIndex: %d (contains: %d), SelectedCase: %d (contains: %d)"),
		CurrentIndex, PawnMap.Contains(CurrentIndex),
		SelectedCase, PawnMap.Contains(SelectedCase));

	return false;
}

TArray<APawns*> AGridManager::Attack(APawns* _pawn)
{
	UE_LOG(LogTemp, Warning, TEXT("--- Attack() PawnMap dump ---"));
	for (const TPair<int, APawns*>& Elem : PawnMap)
	{
		if (!Elem.Value) continue;
		UE_LOG(LogTemp, Warning, TEXT("  Index %d - Type: %d, Team: %d, HP: %d"),
			Elem.Key,
			(int)Elem.Value->PawnStruct.PawnType,
			(int)Elem.Value->PawnStruct.PawnTeam,
			Elem.Value->PawnStruct.PawnStats.Health);
	}
	UE_LOG(LogTemp, Warning, TEXT("--- Attack() - CurrentIndex: %d, SelectedCase: %d, _pawn Type: %d ---"),
		CurrentIndex, SelectedCase, (int)_pawn->PawnStruct.PawnType);

	TArray<APawns*> Targets;

	switch (_pawn->PawnStruct.PawnType)
	{

	case EPawnType::King:
	{
		for (int _tile : TileInRangeForAttack)
		{
			if (PawnMap.Contains(_tile))
			{
				bool bDifferentTeam = PawnMap[_tile]->PawnStruct.PawnTeam != _pawn->PawnStruct.PawnTeam;
				bool bTargetAlive = PawnMap[_tile]->PawnStruct.PawnStats.Health > 0;
				if (bDifferentTeam && bTargetAlive)
				{
					Targets.Emplace(PawnMap[_tile]);
				}
			}
		}
		break;
	}
	case EPawnType::Queen:
	{
		for (int _tile : TileInRangeForAttack)
		{
			if (TileMap.Contains(_tile) && TileMap[_tile].StateArray.Contains(ETileState::PreFire) && PawnMap.Contains(_tile))
			{
				bool bDifferentTeam = PawnMap[_tile]->PawnStruct.PawnTeam != _pawn->PawnStruct.PawnTeam;
				bool bTargetAlive = PawnMap[_tile]->PawnStruct.PawnStats.Health > 0;
				if (bDifferentTeam && bTargetAlive)
				{
					Targets.Emplace(PawnMap[_tile]);
				}
			}
		}
		break;
	}
	case EPawnType::Pawn:
	{
		if (PawnMap.Contains(CurrentIndex))
		{
			Targets.Emplace(PawnMap[CurrentIndex]);
		}
		break;
	}

	case EPawnType::Knight:
	{
		if (PawnMap.Contains(CurrentIndex))
		{
			Targets.Emplace(PawnMap[CurrentIndex]);
		}
		break;
	}

	case EPawnType::Tower:
	{
		if (PawnMap.Contains(CurrentIndex))
		{
			Targets.Emplace(PawnMap[CurrentIndex]);
		}
		break;
	}

	case EPawnType::Bishop:
	{
		if (PawnMap.Contains(CurrentIndex))
		{
			Targets.Emplace(PawnMap[CurrentIndex]);
		}
		break;
	}
	}

	if (!Targets.IsEmpty())
	{
		int KOCount = 0;
		UE_LOG(LogTemp, Warning, TEXT("Attack() : %d cible(s) trouve(s)"), Targets.Num());

		for (APawns* _target : Targets)
		{
			bool bWasAlive = _target->PawnStruct.PawnStats.Health > 0;

			UE_LOG(LogTemp, Warning, TEXT("Avant dégâts : %d HP"), _target->PawnStruct.PawnStats.Health);
			_target->TakeDamage(_pawn->PawnStruct.PawnStats.Attack);
			_target->UpdateLifeHUD();
			UE_LOG(LogTemp, Warning, TEXT("Après dégâts : %d HP"), _target->PawnStruct.PawnStats.Health);

			if (bWasAlive && _target->PawnStruct.PawnStats.Health <= 0)
			{
				KOCount++;

				int TargetIndex = -1;
				for (const TPair<int, APawns*>& _pawn : PawnMap)
				{
					if (_pawn.Value == _target) { TargetIndex = _pawn.Key; break; }
				}
				if (TargetIndex != -1 && !DownedPawnTurnCounters.Contains(TargetIndex))
				{
					DownedPawnTurnCounters.Add(TargetIndex, 0);
				}
			}
		}

		if (KOCount > 0)
		{
			BonusActionsRemaining += KOCount;
			ActionPointsRemaining += KOCount;
			OnBonusTurnGranted(KOCount);
			LevelUpPawn(_pawn, KOCount);
		}
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack() : Targets est vide !"));
	}

	RemoveStateToTile(ETileState::Selected, SelectedCase);
	RemoveStateToTile(ETileState::Hovered, CurrentIndex);
	RemoveAllPossible();

	return Targets;
}

bool AGridManager::CheckStalemate(int _currentIndex)
{
	if (TileMap[_currentIndex].TypeArray == ETileType::Occuped)
	{
		if (PawnMap.Contains(_currentIndex) && PawnMap.Contains(SelectedCase))
		{
			UE_LOG(LogTemp, Warning, TEXT("The Index is Occuped and can't get taken: %d has %d HP Left"), PreviousIndex, PawnMap[_currentIndex]->PawnStruct.PawnStats.Health);

			if (PawnMap[_currentIndex]->PawnStruct.PawnTeam != PawnMap[SelectedCase]->PawnStruct.PawnTeam && PawnMap[_currentIndex]->PawnStruct.PawnStats.Health <= 0)
			{
				Stalemate(PawnMap[_currentIndex]);
				return true;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CheckStalemate - SelectedCase invalide: %d (contains: %d)"), SelectedCase, PawnMap.Contains(SelectedCase));
		}
		return false;
	}
	return true;
}

void AGridManager::Stalemate(APawns* _pawnTaken)
{
	UE_LOG(LogTemp, Error, TEXT("STALEMATE: Removing index %d from PawnMap"), CurrentIndex);
	DownedPawnTurnCounters.Remove(CurrentIndex);
	PawnMap.Remove(CurrentIndex);
	_pawnTaken->Destroy();
}

int AGridManager::GetTurnsBeforeDeath(int TileIndex) const
{
	if (!DownedPawnTurnCounters.Contains(TileIndex)) return -1;
	return FMath::Max(0, MaxDownedTurns - DownedPawnTurnCounters[TileIndex]);
}

void AGridManager::Moving(int _currentIndex, APawns* _pawn)
{
	RemoveTypeToTile(ETileType::Occuped, SelectedCase);
	RemoveStateToTile(ETileState::Selected, SelectedCase);
	RemoveStateToTile(ETileState::Hovered, _currentIndex);
	RemoveAllPossible();
	PawnMap.Remove(SelectedCase);
	PawnMap.Emplace(_currentIndex, _pawn);
	PlayMouveSound();

	UE_LOG(LogTemp, Warning, TEXT("MOVING: Removing %d, Adding %d, Pawn ptr: %p"), SelectedCase, _currentIndex, _pawn);

	SelectedCase = CurrentIndex;
}

void AGridManager::ClearAllHoverStates()
{
	for (TPair<int, FTileData>& _tile : TileMap)
	{
		if (_tile.Value.StateArray.Contains(ETileState::Hovered))
		{
			RemoveStateToTile(ETileState::Hovered, _tile.Key);
		}
	}
}

void AGridManager::ShowAttackRange(int _currentIndex, APawns* _pawn)
{
	SelectedCase = _currentIndex;
	//OnBonusTurnGranted
	TileInRangeForAttack.Empty();

	switch (_pawn->PawnStruct.PawnType)
	{
	case EPawnType::Pawn:
	{
		PawnAttackRange(_currentIndex, _pawn->PawnStruct.PawnTeam);
		break;
	}
	case EPawnType::King:
	{
		KingAttackRange(_currentIndex);
		for (int i = 0; i < TileInRangeForAttack.Num(); i++)
		{
			AddStateToTile(ETileState::PreFire, TileInRangeForAttack[i]);
		}
		break;
	}
	case EPawnType::Knight:
	{
		KnightAttackRange(_currentIndex);

		break;
	}
	case EPawnType::Tower:
	{
		TowerAttackRange(_currentIndex);
		break;
	}
	case EPawnType::Bishop:
	{
		BishopAttackRange(_currentIndex);
		break;
	}
	case EPawnType::Queen:
	{
		QueenAttackRange(_currentIndex);
		FString TileList;
		for (int _tile : TileInRangeForAttack)
		{
			TileList += FString::Printf(TEXT("%d "), _tile);
		}
		UE_LOG(LogTemp, Warning, TEXT("Queen %d - Range: %s"), _currentIndex, *TileList);
		break;
		break;
	}
	}

	TArray<int> UniqueTiles;
	for (int _tile : TileInRangeForAttack)
	{
		UniqueTiles.AddUnique(_tile);
	}
	TileInRangeForAttack = UniqueTiles;

	if (!TileInRangeForAttack.IsEmpty())
	{
		for (int i = 0; i < TileInRangeForAttack.Num(); i++)
		{
			AddStateToTile(ETileState::Vulnerable, TileInRangeForAttack[i]);
		}
	}
}

void AGridManager::SpawnPawns(int _tileIndex, APawns* _pawn)
{
	AddTypeToTile(ETileType::Occuped, _tileIndex);
	PawnMap.Emplace(_tileIndex, _pawn);

	UE_LOG(LogTemp, Warning, TEXT("SPAWN: Index %d, Pawn ptr: %p"), _tileIndex, _pawn);

}

void AGridManager::AddStateToTile(ETileState stateToAdd, int InstanceIndex)
{
	if (TileMap.Contains(InstanceIndex))
	{
		TileMap[InstanceIndex].StateArray.Emplace(stateToAdd);
		UpdateTileVisuals(InstanceIndex);
	}
}

void AGridManager::RemoveStateToTile(ETileState stateToRemove, int InstanceIndex)
{
	if (TileMap.Contains(InstanceIndex) && TileMap[InstanceIndex].StateArray.Contains(stateToRemove)) {
		TileMap[InstanceIndex].StateArray.Remove(stateToRemove);
		UpdateTileVisuals(InstanceIndex);
	}
}

void AGridManager::RemoveAllPossible()
{
	for (int _tile : TileShowed)
	{
		RemoveStateToTile(ETileState::Accessible, _tile);
	}

	for (int _tile : TileInRangeForAttack)
	{
		RemoveStateToTile(ETileState::Vulnerable, _tile);
		if (TileMap.Contains(_tile) && TileMap[_tile].StateArray.Contains(ETileState::PreFire))
			RemoveStateToTile(ETileState::PreFire, _tile);
	}

	if (TileMap.Contains(PreviousIndex) && TileMap[PreviousIndex].StateArray.Contains(ETileState::Hovered))
		RemoveStateToTile(ETileState::Hovered, PreviousIndex);
}

void AGridManager::AddTypeToTile(ETileType typeToAdd, int InstanceIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("The Tile %d as ben blocked"), InstanceIndex);

	if (TileMap.Contains(InstanceIndex))
	{
		TileMap[InstanceIndex].TypeArray = typeToAdd;

		switch (typeToAdd)
		{
		case ETileType::Blocked:AddStateToTile(ETileState::Occuped, InstanceIndex);
			break;
		case ETileType::Occuped:AddStateToTile(ETileState::Occuped, InstanceIndex);
		default:
			break;
		}

		UpdateTileVisuals(InstanceIndex);
	}
}

void AGridManager::RemoveTypeToTile(ETileType _typeToRemove, int _InstanceIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("The Tile %d is now neutral"), _InstanceIndex);

	if (TileMap.Contains(_InstanceIndex))
	{
		TileMap[_InstanceIndex].TypeArray = ETileType::Normal;

		switch (_typeToRemove)
		{
		case ETileType::Blocked:RemoveStateToTile(ETileState::Occuped, _InstanceIndex);
			break;
		case ETileType::Occuped:RemoveStateToTile(ETileState::Occuped, _InstanceIndex);
		default:
			break;
		}

		UpdateTileVisuals(_InstanceIndex);
	}
}

ETileType AGridManager::FindTileType(int index)
{
	return TileMap[index].TypeArray;
}

FLinearColor AGridManager::GetColorFromStates(TArray<ETileState> ArrayToCheck)
{

	ETileState lastState = ETileState::None;

	for (ETileState State : ArrayToCheck)
	{
		if (State < lastState)
		{
			lastState = State;
		}
	}

	if (ColorMap.Contains(lastState))
	{
		return FLinearColor(ColorMap[lastState]);
	}

	else
		return FLinearColor(0, 0, 0, 0);
}

void AGridManager::UpdateTileVisuals(int InstanceIndex)
{
	if (TileMap.Contains(InstanceIndex))
	{
		ChangeColor(InstanceIndex, GetColorFromStates(TileMap[InstanceIndex].StateArray));
	}
}

void AGridManager::OnConstruction(const FTransform& Transform)
{

	Super::OnConstruction(Transform);

	ISM->ClearInstances();
	ISM->RegisterComponent();
	ISM->SetStaticMesh(Mesh);
	ISM->SetMaterial(0, Material);

	int index = 0;

	for (int i = 0; i < X; i++)
	{
		for (int j = 0; j < Y; j++)
		{
			FVector Pos(i * TileSize, j * TileSize, 0.0f);
			FTransform Trans = FTransform(FRotator(0, 0, 0), Pos, FVector(TileSize / 100, TileSize / 100, TileSize / 100));
			ISM->AddInstance(Trans);
			TileMap.Emplace(index, FTileData(TArray<ETileState> {ETileState::None}, ETileType::Normal));
			index++;
		}
	}
}

bool AGridManager::isIndexValid(int index)
{
	return index != -999;
}

void AGridManager::ChangeColor(int InstanceIndex, FLinearColor color)
{
	ISM->SetCustomDataValue(InstanceIndex, 0, color.R);
	ISM->SetCustomDataValue(InstanceIndex, 1, color.G);
	ISM->SetCustomDataValue(InstanceIndex, 2, color.B);
	ISM->SetCustomDataValue(InstanceIndex, 3, color.A);
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	EnemyAI = NewObject<UEnemyAction>(this);
	EnemyAI->Initialize(this);

	if (PlayerController)
	{
		PlayerController->bShowMouseCursor = true;
		PlayerController->bEnableClickEvents = true;
		PlayerController->bEnableMouseOverEvents = true;
	}

	EnableInput(PlayerController);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Mapping Context must be added inside Blueprint"));
		}
	}

	if (UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EIC->BindAction(SelectAction, ETriggerEvent::Started, this, &AGridManager::OnSelect);
		EIC->BindAction(UnSelectAction, ETriggerEvent::Started, this, &AGridManager::OnUnSelect);
	}
}

void AGridManager::OnSelect(const FInputActionValue& Value)
{
	if (!CurrentAction) return;
	if (!CurrentAction->IsValidLowLevel()) return;

	int clickedIndex = CurrentIndex;
	SelectedTileIndex = clickedIndex;
	CurrentAction->LeftClick(clickedIndex);
}

void AGridManager::OnUnSelect(const FInputActionValue& Value)
{
	if (!CurrentAction) return;
	if (!CurrentAction->IsValidLowLevel()) return;

	CurrentAction->RightClick(CurrentIndex);
}

void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (PlayerController)
	{
		bool bHit = PlayerController->GetHitResultUnderCursor(ECC_Grid, false, Hit);
		if (bHit && Hit.Item != CurrentIndex)
		{
			PreviousIndex = CurrentIndex;
			CurrentIndex = Hit.Item;
		}
	}

	if (CurrentAction && CurrentAction->IsValidLowLevel())
	{
		CurrentAction->ExecuteAction(CurrentIndex);
	}
}

bool AGridManager::CheckForEnd()
{
	int _playerPawns = 0;
	int _opponentPawns = 0;
	bool _playerKingAlive = false;
	bool _opponentKingAlive = false;

	for (const TPair<int, APawns*>& _pawn : PawnMap)
	{
		if (!_pawn.Value) continue;

		if (_pawn.Value->PawnStruct.PawnTeam == ETeam::Ally)
		{
			_playerPawns++;
		}
		else
		{
			_opponentPawns++;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("%d player, %d opponant"), _playerPawns, _opponentPawns);

	if (_playerPawns == 1 && _opponentPawns == 1)
	{
		EndTheGame(EGameResult::Draw, EEndGameReason::Draw);
		return true;
	}

	const int MINPAWNDIFF = 2;

	if (_playerPawns == 1 && _opponentPawns >= 1 + MINPAWNDIFF)
	{
		EndTheGame(EGameResult::OpponentWin, EEndGameReason::MaterialAdvantage);
		return true;
	}
	if (_opponentPawns == 1 && _playerPawns >= 1 + MINPAWNDIFF)
	{
		EndTheGame(EGameResult::PlayerWin, EEndGameReason::MaterialAdvantage);
		return true;
	}
	
	return false;
}
