// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyPool.h"
#include "./Enemies/Enemy.h"
#include "Engine/Engine.h"
#include "./Enemies/ShooterEnemy.h"
#include "../Public/Weapon/Weapon.h"

// Sets default values
UEnemyPool::UEnemyPool() :
	SpawnCounter(1)
{
	static ConstructorHelpers::FObjectFinder<UClass> GhostEnemyBlueprint(TEXT("/Game/ShooterGame/Blueprints/Enemies/BP_GhostEnemy.BP_GhostEnemy_C"));
	static ConstructorHelpers::FObjectFinder<UClass> ShooterEnemyBlueprint(TEXT("/Game/ShooterGame/Blueprints/Enemies/ShooterEnemyAI/BP_ShooterEnemy.BP_ShooterEnemy_C"));
	
	if (GhostEnemyBlueprint.Succeeded())
	{
		EnemyClasses.Add(GhostEnemyBlueprint.Object);
		GhostEnemyClass = GhostEnemyBlueprint.Object;
	}

	if (ShooterEnemyBlueprint.Succeeded())
	{
		EnemyClasses.Add(ShooterEnemyBlueprint.Object);
		ShooterEnemyClass = ShooterEnemyBlueprint.Object;
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

		AShooterEnemy* ShooterEnemy = Cast<AShooterEnemy>(EnemyToReturn);

		if (ShooterEnemy) 
		{
			ShooterEnemy->GetEquippedWeapon()->SetActorHiddenInGame(false);
		}

		return EnemyToReturn;
	}

	return nullptr; // 利用可能な敵がない場合
}


void UEnemyPool::ReturnEnemy(AEnemy* enemy)
{
	if (enemy)
	{
		enemy->SetActorEnableCollision(false); // コリジョンを無効にする
		enemy->SetActorTickEnabled(false);
		enemy->SetActorHiddenInGame(true);
		AvailableEnemies.Push(enemy);
	}
}

FVector UEnemyPool::GetRandomLocation()
{
	// ステージの中心位置
	FVector StageCenter = FVector(0.0f, 0.0f, 88.0f);

	// ステージを円に見立てたときの、中心からの距離（半径）。やや大きめにとる。
	float Radius = 4500.0f;

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
	// 7回に1回ShooterEnemyを選択
	UClass* SelectedClass = nullptr;

	if (SpawnCounter % 7 == 0 && ShooterEnemyClass != nullptr)
	{
		SelectedClass = ShooterEnemyClass;
	}
	else if (GhostEnemyClass != nullptr)
	{
		// それ以外の場合はGhostEnemyを選択
		SelectedClass = GhostEnemyClass;
	}

	SpawnCounter++;

	if (SelectedClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		// 新しいスポーン位置を計算
		FVector SpawnLocation = GetRandomLocation();
		FRotator SpawnRotation = FRotator(0, 0, 0);

		// 敵を新しいスポーン位置でスポーン
		return WorldReference->SpawnActor<AEnemy>(SelectedClass, SpawnLocation, SpawnRotation, SpawnParams);
	}
	else
	{
		return nullptr;
	}
}

