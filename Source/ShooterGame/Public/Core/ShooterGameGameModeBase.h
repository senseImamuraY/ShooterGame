// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShooterGameGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShooterGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AShooterGameGameModeBase();
	
	void KillPlayer();

	void RestartGame();

	virtual void BeginPlay() override;

private:
	FTimerHandle SpawnEnemyTimerHandle;

	void SpawnEnemy();
	
	class UClass* EnemyClass;

	FTimerHandle GoalTimerHandle;

	void SpawnGoal();

	class AGoal* Goal;

};
