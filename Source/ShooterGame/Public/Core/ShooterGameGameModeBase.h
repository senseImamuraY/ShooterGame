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
class AEnemy;
class AItem;
class USoundCue;

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
	UClass* EnemyClass;

	UPROPERTY()
	UEnemyPool* EnemyPoolInstance; 

	UPROPERTY()
	UItemPool* ItemPoolInstance; 

	UFUNCTION()
	void HandleEnemyDeath(AEnemy* DeadEnemy);

	float StartDelaty;
	void HandleGameStart();
	void EnablePlayer();

	UFUNCTION()
	void HandleItemReturn(AItem* Item);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BGM, meta = (AllowPrivateAccess = "true"))
	USoundCue* GameSound;
};
