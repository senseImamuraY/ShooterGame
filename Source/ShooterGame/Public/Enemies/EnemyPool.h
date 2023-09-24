// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyPool.generated.h"

UCLASS()
class SHOOTERGAME_API UEnemyPool : public UObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UEnemyPool();

	// 利用可能な敵のリスト
	UPROPERTY()
	TArray<class AEnemy*> AvailableEnemies;

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
	AEnemy* RandomSpawn();

	class UClass* EnemyClass;

	class UWorld* WorldReference;
};
