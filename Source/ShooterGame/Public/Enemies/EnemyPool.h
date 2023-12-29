// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyPool.generated.h"

class AEnemy;
class UClass;
class UWorld;

UCLASS()
class SHOOTERGAME_API UEnemyPool : public UObject
{
	GENERATED_BODY()
	
public:	
	UEnemyPool();

	// 利用可能な敵のリスト
	UPROPERTY()
	TArray<AEnemy*> AvailableEnemies;

	// プールの最大サイズ
	static const int32 MaxEnemies = 15;

	// 初期化関数
	UFUNCTION(BlueprintCallable, Category = "EnemyPool")
	void Initialize(UWorld* World);

	// 利用可能な敵を取得
	UFUNCTION(BlueprintCallable, Category = "EnemyPool")
	AEnemy* GetEnemy();	

	// 敵をプールに返却
	UFUNCTION(BlueprintCallable, Category = "EnemyPool")
	void ReturnEnemy(AEnemy* enemy);

	// ランダムな敵のスポーン位置を返す
	UFUNCTION(BlueprintCallable, Category = "EnemyPool")
	FVector GetRandomLocation();

private:
	// GhostEnemyのBlueprintクラス
	TSubclassOf<AActor> GhostEnemyClass;

	// ShooterEnemyのBlueprintクラス
	TSubclassOf<AActor> ShooterEnemyClass;

	// KrakenEnemyのBlueprintクラス
	TSubclassOf<AActor> KrakenEnemyClass;

	AEnemy* RandomSpawn();

	TArray<UClass*> EnemyClasses;

	UWorld* WorldReference;

	// 10回に1回ShooterEnemyをスポーンさせる
	int32 SpawnCounter;
};
