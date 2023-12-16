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
#include "../Public/Items/ItemPool.h"
#include "../Public/Items/Item.h"


AShooterGameGameModeBase::AShooterGameGameModeBase() :
	StartDelaty(3.f)
{
	HUDClass = AInGameHUD::StaticClass();
	PlayerControllerClass = AShooterPlayerController::StaticClass();
	PrimaryActorTick.bCanEverTick = true;
}

void AShooterGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	HandleGameStart();

	// Poolのインスタンスを生成
	EnemyPoolInstance = NewObject<UEnemyPool>();
	ItemPoolInstance = NewObject<UItemPool>();

	// Poolを初期化
	if (EnemyPoolInstance)
	{
		UWorld* World = GetWorld();

		if (World)
		{
			EnemyPoolInstance->Initialize(World);
		}
	}

	if (ItemPoolInstance)
	{
		UWorld* World = GetWorld();

		if (World)
		{
			ItemPoolInstance->Initialize(World);
		}
	}

	for (TActorIterator<AGoal> It(GetWorld()); It; ++It)
	{
		Goal = *It;

		if (Goal)
		{
			break;
		}
	}

	for (TActorIterator<AEnemy> It(GetWorld()); It; ++It)
	{
		AEnemy* Enemy = *It;

		if (Enemy)
		{
			Enemy->OnEnemyDead.AddDynamic(this, &AShooterGameGameModeBase::HandleEnemyDeath);
		}
	}

	//for (TActorIterator<AItem> It(GetWorld()); It; ++It)
	//{
	//	AItem* Item = *It;

	//	if (Item)
	//	{
	//		Item->OnItemReturnRequested.AddDynamic(this, &AShooterGameGameModeBase::HandleEnemyDeath);
	//	}
	//}


	float EnemySpawnInterval = 5.f;
	float TimeUntilGoalAppears = 60.f;

	// Set a timer to spawn an enemy every 5 seconds
	GetWorldTimerManager().SetTimer(SpawnEnemyTimerHandle, this, &AShooterGameGameModeBase::SpawnEnemy, EnemySpawnInterval, true);
	GetWorldTimerManager().SetTimer(GoalTimerHandle, this, &AShooterGameGameModeBase::SpawnGoal, TimeUntilGoalAppears, false);
}

void AShooterGameGameModeBase::KillPlayer()
{
	RestartGame();
}

void AShooterGameGameModeBase::RestartGame()
{
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
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
			FRotator SpawnRotation = FRotator(0, 0, 0);
			// 敵の位置と回転を設定
			SpawnedEnemy->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
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

	//ItemPoolInstance->GetItem(ItemPoolInstance->GetPotionClass());
	 // アイテムを取得
	AItem* SpawnedItem = ItemPoolInstance->GetItem(ItemPoolInstance->GetPotionClass());
	//AItem* SpawnedItem = ItemPoolInstance->GetItem(ItemPoolInstance->GetRandomItemClass());
	if (SpawnedItem)
	{
		// 敵の位置にアイテムをスポーン
		FVector EnemyLocation = DeadEnemy->GetActorLocation();
		SpawnedItem->SetActorLocation(EnemyLocation);

		//// 必要に応じて他のプロパティを設定
		//SpawnedItem->SetActorHiddenInGame(false);
		//SpawnedItem->SetActorEnableCollision(true);
		//SpawnedItem->SetActorTickEnabled(true);
			   // ログ出力
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Spawned Item: %s at Location: %s"), *SpawnedItem->GetName(), *EnemyLocation.ToString()));
		}
	}
	else
	{
		// アイテムが取得できなかった場合のログ出力
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to spawn item from ItemPool"));
		}
	}
}


