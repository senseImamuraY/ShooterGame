// Copyright Epic Games, Inc. All Rights Reserved.


#include "../Public/Core/ShooterGameGameModeBase.h"
#include "Core/InGameHUD.h"
#include "./Player/ShooterPlayerController.h"
#include "./Player/ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "../Public/Enemies/Enemy.h"
#include "../Public/Enemies/EnemyPool.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "../Public/Items/ItemPool.h"
#include "../Public/Items/Item.h"
#include "../Public/Weapon/Weapon.h"


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

	// マップを全探索して、それぞれのHandleを適切なActorにバインド
	for (TActorIterator<AEnemy> It(GetWorld()); It; ++It)
	{
		AEnemy* Enemy = *It;

		if (Enemy)
		{
			Enemy->OnEnemyDead.AddDynamic(this, &AShooterGameGameModeBase::HandleEnemyDeath);
		}
	}

	for (TActorIterator<AItem> It(GetWorld()); It; ++It)
	{
		AItem* Item = *It;
		AWeapon* Weapon = Cast<AWeapon>(Item);
		if (Weapon) continue;

		if (Item)
		{
			Item->OnItemReturnRequested.AddDynamic(this, &AShooterGameGameModeBase::HandleItemReturn);
		}
	}

	float EnemySpawnInterval = 5.f;
	float TimeUntilGoalAppears = 60.f;

	GetWorldTimerManager().SetTimer(SpawnEnemyTimerHandle, this, &AShooterGameGameModeBase::SpawnEnemy, EnemySpawnInterval, true);
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
			// スポーン位置や回転の計算
			FVector SpawnLocation = EnemyPoolInstance->GetRandomLocation();
			FRotator SpawnRotation = FRotator(0, 0, 0);
			SpawnedEnemy->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
		}
	}
}

void AShooterGameGameModeBase::HandleGameStart()
{
	AShooterCharacter* Player = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	//StartGame();

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

void AShooterGameGameModeBase::HandleItemReturn(AItem* Item)
{
	ItemPoolInstance->ReturnItem(Item);
}

void AShooterGameGameModeBase::HandleEnemyDeath(AEnemy* DeadEnemy)
{
	// ここでEnemyPoolのReturnEnemy関数を呼び出して、DeadEnemyを返却するなどの処理を行う
	// また、同時にDeadEnemyがいた場所にアイテムをスポーンする
	EnemyPoolInstance->ReturnEnemy(DeadEnemy);

	AItem* SpawnedItem = ItemPoolInstance->GetItem(ItemPoolInstance->GetRandomItemClass());
	if (SpawnedItem)
	{
		FVector EnemyLocation = DeadEnemy->GetActorLocation();
		SpawnedItem->SetActorLocation(EnemyLocation);
	}
}


