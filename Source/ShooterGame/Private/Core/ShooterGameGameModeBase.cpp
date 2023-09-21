// Copyright Epic Games, Inc. All Rights Reserved.


#include "../Public/Core/ShooterGameGameModeBase.h"
#include "Core/InGameHUD.h"
#include "./Player/ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "../Public/Enemies/Enemy.h"
#include "../Public/Environments/Goal.h"

AShooterGameGameModeBase::AShooterGameGameModeBase() 
{
	HUDClass = AInGameHUD::StaticClass();
	PlayerControllerClass = AShooterPlayerController::StaticClass();

	static ConstructorHelpers::FObjectFinder<UClass> EnemyBlueprint(TEXT("/Game/ShooterGame/Blueprints/Enemies/BP_Enemy.BP_Enemy_C"));
	if (EnemyBlueprint.Succeeded())
	{
		EnemyClass = EnemyBlueprint.Object;
	}
}

void AShooterGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// Set a timer to spawn an enemy every 5 seconds
	GetWorldTimerManager().SetTimer(SpawnEnemyTimerHandle, this, &AShooterGameGameModeBase::SpawnEnemy, 5.0f, true);
	GetWorldTimerManager().SetTimer(GoalTimerHandle, this, &AShooterGameGameModeBase::SpawnGoal, 10.0f, false);

	// AGoalクラスのインスタンスを生成
	Goal = GetWorld()->SpawnActor<AGoal>();
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
	//// Define the spawn parameters
	//FActorSpawnParameters SpawnParams;
	//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	//// Spawn the enemy at a specific location and rotation
	//GetWorld()->SpawnActor<AEnemy>(AEnemy::StaticClass(), FVector(0, 0, 100), FRotator(0, 0, 0), SpawnParams);
	// Blueprintのアセットパス
	
    if (EnemyClass)
    {
        // Define the spawn parameters
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		// ステージの中心位置
		FVector StageCenter(0.0f, 0.0f, 0.0f);

		// ステージを円に見立てたときの、中心からの距離（半径）。やや大きめにとる。
		float Radius = 6000.0f;

		// ランダムな角度を0から360度の間で生成
		float RandomAngle = FMath::RandRange(0.0f, 360.0f);

		// 角度を使用してxおよびyのオフセットを計算。回転する際はUEの座標に合わせる。
		float OffsetX = Radius * FMath::Sin(FMath::DegreesToRadians(RandomAngle));  
		float OffsetY = Radius * FMath::Cos(FMath::DegreesToRadians(RandomAngle));

		// 新しいスポーン位置を計算
		FVector SpawnLocation = StageCenter + FVector(OffsetX, OffsetY, 0.0f);

		// 敵を新しいスポーン位置でスポーン
		GetWorld()->SpawnActor<AEnemy>(EnemyClass, SpawnLocation, FRotator(0, 0, 0), SpawnParams);

        // Spawn the enemy using the Blueprint class
        //GetWorld()->SpawnActor<AEnemy>(EnemyClass, FVector(5200.f, 5200.f, 200.f), FRotator(0.f, 0.f, 0.f), SpawnParams);
    }
}

void AShooterGameGameModeBase::SpawnGoal()
{
	Goal->Spawn();
}




