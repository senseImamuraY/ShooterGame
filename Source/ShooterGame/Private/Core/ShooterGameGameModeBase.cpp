// Copyright Epic Games, Inc. All Rights Reserved.


#include "../Public/Core/ShooterGameGameModeBase.h"
#include "Core/InGameHUD.h"
#include "./Player/ShooterPlayerController.h"
#include "./Player/ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "../Public/Enemies/Enemy.h"
#include "../Public/Environments/Goal.h"
#include "../Public/Enemies/EnemyPool.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"


AShooterGameGameModeBase::AShooterGameGameModeBase() :
	StartDelaty(3.f)
{
	HUDClass = AInGameHUD::StaticClass();
	PlayerControllerClass = AShooterPlayerController::StaticClass();
}

void AShooterGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	HandleGameStart();

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


	// Set a timer to spawn an enemy every 5 seconds
	GetWorldTimerManager().SetTimer(SpawnEnemyTimerHandle, this, &AShooterGameGameModeBase::SpawnEnemy, 5.0f, true);
	GetWorldTimerManager().SetTimer(GoalTimerHandle, this, &AShooterGameGameModeBase::SpawnGoal, 60.0f, false);

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

void AShooterGameGameModeBase::HandleGameStart()
{
	AShooterCharacter* Player = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	StartGame();

	if (PlayerController)
	{
		PlayerController->SetPlayerEnabledState(false);
		
		// 3秒後にEnablePlayer関数を呼び出す
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AShooterGameGameModeBase::EnablePlayer, StartDelaty, false);
	}
}

void AShooterGameGameModeBase::EnablePlayer()
{
	AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PlayerController)
	{
		PlayerController->SetPlayerEnabledState(true);
	}
}

void AShooterGameGameModeBase::HandleEnemyDeath(AEnemy* DeadEnemy)
{
	// ここでEnemyPoolのReturnEnemy関数を呼び出して、DeadEnemyを返却するなどの処理を行う
	EnemyPoolInstance->ReturnEnemy(DeadEnemy);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("AEnemy::HandleEnemyDeath() is called."));
}


