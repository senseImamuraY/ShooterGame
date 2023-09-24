// Copyright Epic Games, Inc. All Rights Reserved.


#include "../Public/Core/ShooterGameGameModeBase.h"
#include "Core/InGameHUD.h"
#include "./Player/ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "../Public/Enemies/Enemy.h"
#include "../Public/Environments/Goal.h"
#include "../Public/Enemies/EnemyPool.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"


AShooterGameGameModeBase::AShooterGameGameModeBase() 
{
	HUDClass = AInGameHUD::StaticClass();
	PlayerControllerClass = AShooterPlayerController::StaticClass();
}

void AShooterGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// EnemyPoolのインスタンスを生成
	EnemyPoolInstance = NewObject<UEnemyPool>();

	// EnemyPoolを初期化
	if (EnemyPoolInstance)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("GetWorld() succeeded!"));
			EnemyPoolInstance->Initialize(World);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GetWorld() returned nullptr!"));
		}
	}

	// Set a timer to spawn an enemy every 5 seconds
	GetWorldTimerManager().SetTimer(SpawnEnemyTimerHandle, this, &AShooterGameGameModeBase::SpawnEnemy, 5.0f, true);
	GetWorldTimerManager().SetTimer(GoalTimerHandle, this, &AShooterGameGameModeBase::SpawnGoal, 10.0f, false);

	// AGoalクラスのインスタンスを生成
	Goal = GetWorld()->SpawnActor<AGoal>();

	// 例: AShooterGameGameModeBaseのBeginPlay関数内
	for (TActorIterator<AEnemy> It(GetWorld()); It; ++It)
	{
		AEnemy* Enemy = *It;
		if (Enemy)
		{
			// Enemyの名前を取得
			FString EnemyName = Enemy->GetName();

			// Enemyの位置を取得
			FVector EnemyLocation = Enemy->GetActorLocation();
			FString LocationString = FString::Printf(TEXT("X: %f, Y: %f, Z: %f"), EnemyLocation.X, EnemyLocation.Y, EnemyLocation.Z);

			// デバッグメッセージを表示
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Enemy Name: %s, Location: %s"), *EnemyName, *LocationString));
			}

			Enemy->OnEnemyDead.AddDynamic(this, &AShooterGameGameModeBase::HandleEnemyDeath);
		}
	}

	if (GetWorldTimerManager().IsTimerActive(SpawnEnemyTimerHandle))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("SpawnEnemyTimer is active!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SpawnEnemyTimer is not active!"));
	}

}

void AShooterGameGameModeBase::KillPlayer()
{
	//	// 現在のLevelNameを取得する
	//const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());

	//// 現在のLevelを開きなおす
	//UGameplayStatics::OpenLevel(GetWorld(), FName(*CurrentLevelName));
	RestartGame();
}

void AShooterGameGameModeBase::RestartGame()
{
	// PlayerControllerを取得する
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// InGameHUDクラスを取得する
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// ゲームオーバー画面を表示する
	HUD->DispGameOver();
}


void AShooterGameGameModeBase::SpawnEnemy()
{
	if (EnemyPoolInstance)
	{
		AEnemy* SpawnedEnemy = EnemyPoolInstance->GetEnemy();
		if (SpawnedEnemy)
		{
			// ... スポーン位置や回転の計算 ...
			FVector SpawnLocation = EnemyPoolInstance->GetRandomLocation();
			// 敵の位置と回転を設定
			SpawnedEnemy->SetActorLocationAndRotation(SpawnLocation, FRotator(0, 0, 0));

		}
	}
}

void AShooterGameGameModeBase::SpawnGoal()
{
	Goal->Spawn();
}

void AShooterGameGameModeBase::HandleEnemyDeath(AEnemy* DeadEnemy)
{
	// ここでEnemyPoolのReturnEnemy関数を呼び出して、DeadEnemyを返却するなどの処理を行う
	EnemyPoolInstance->ReturnEnemy(DeadEnemy);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("AEnemy::HandleEnemyDeath() is called."));
}


