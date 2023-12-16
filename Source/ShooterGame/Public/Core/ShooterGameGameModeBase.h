// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShooterGameGameModeBase.generated.h"

class UClass;
class UEnemyPool;
class AGoal;
class ADirectionalLight;
class UItemPool;

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

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void StartGame();

private:
	FTimerHandle SpawnEnemyTimerHandle;
	void SpawnEnemy();
	UClass* EnemyClass;

	UPROPERTY()
	UEnemyPool* EnemyPoolInstance; 

	UPROPERTY()
	UItemPool* ItemPoolInstance; 

	UFUNCTION()
	void HandleEnemyDeath(class AEnemy* DeadEnemy);

	FTimerHandle GoalTimerHandle;
	void SpawnGoal();
	AGoal* Goal;

	float StartDelaty;
	void HandleGameStart();
	void EnablePlayer();
};
