// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyPool.h"
#include "./Enemies/Enemy.h"
#include "Engine/Engine.h"


// Sets default values
UEnemyPool::UEnemyPool()
{
	static ConstructorHelpers::FObjectFinder<UClass> EnemyBlueprint(TEXT("/Game/ShooterGame/Blueprints/Enemies/BP_Enemy.BP_Enemy_C"));

	if (EnemyBlueprint.Succeeded())
	{
		EnemyClass = EnemyBlueprint.Object;
	}
}

void UEnemyPool::Initialize(UWorld* World)
{
	WorldReference = World;

	for (int32 i = 0; i < MaxEnemies; ++i)
	{
		// 敵を生成
		AEnemy* NewEnemy = RandomSpawn();

		if (NewEnemy)
		{
			NewEnemy->SetActorHiddenInGame(true); // 初期状態では非表示にする
			NewEnemy->SetActorEnableCollision(false); // 初期状態ではコリジョンを無効にする
			NewEnemy->SetActorTickEnabled(false);
			AvailableEnemies.Add(NewEnemy);
		}
	}
}

AEnemy* UEnemyPool::GetEnemy()
{
	if (AvailableEnemies.Num() > 0)
	{
		AEnemy* EnemyToReturn = AvailableEnemies.Pop();
		EnemyToReturn->SetActorHiddenInGame(false); // 敵を表示
		EnemyToReturn->SetActorEnableCollision(true); // コリジョンを有効にする
		EnemyToReturn->SetActorTickEnabled(true);
		EnemyToReturn->InitEnemyHealth();

		return EnemyToReturn;
	}

	return nullptr; // 利用可能な敵がない場合
}


void UEnemyPool::ReturnEnemy(AEnemy* enemy)
{
	if (enemy)
	{
		// 非表示にする処理を遅らせるためのタイマーを設定
		FTimerHandle TimerHandle;
		float DelayTime = 1.0f; // 1秒後に実行する

		enemy->SetActorEnableCollision(false); // コリジョンを無効にする
		enemy->SetActorTickEnabled(false);
		enemy->SetActorHiddenInGame(true); // 敵を非表示にする
		AvailableEnemies.Push(enemy);
	}
}

FVector UEnemyPool::GetRandomLocation()
{
	// ステージの中心位置
	FVector StageCenter = FVector(0.0f, 0.0f, 0.0f);

	// ステージを円に見立てたときの、中心からの距離（半径）。やや大きめにとる。
	float Radius = 6000.0f;

	// ランダムな角度を0から360度の間で生成
	float RandomAngle = FMath::RandRange(0.0f, 360.0f);

	// 角度を使用してxおよびyのオフセットを計算。回転する際はUEの座標に合わせる。
	float OffsetX = Radius * FMath::Sin(FMath::DegreesToRadians(RandomAngle));
	float OffsetY = Radius * FMath::Cos(FMath::DegreesToRadians(RandomAngle));
	float OffsetZ = 0.f;

	// 新しいスポーン位置を計算
	return StageCenter + FVector(OffsetX, OffsetY, OffsetZ);
}

AEnemy* UEnemyPool::RandomSpawn()
{
	if (EnemyClass)
	{
		// Define the spawn parameters
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		// 新しいスポーン位置を計算
		FVector SpawnLocation = GetRandomLocation();
		FRotator SpawnRotation = FRotator(0, 0, 0);

		// 敵を新しいスポーン位置でスポーン
		return WorldReference->SpawnActor<AEnemy>(EnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
	}
	else
	{
		return nullptr;
	}
}

