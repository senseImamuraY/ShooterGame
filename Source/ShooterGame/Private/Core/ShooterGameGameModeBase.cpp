// Copyright Epic Games, Inc. All Rights Reserved.


#include "../Public/Core/ShooterGameGameModeBase.h"
#include "Core/InGameHUD.h"
#include "./Player/ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "../Public/Enemies/Enemy.h"


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

        // Spawn the enemy using the Blueprint class
        GetWorld()->SpawnActor<AEnemy>(EnemyClass, FVector(0, 0, 100), FRotator(0, 0, 0), SpawnParams);
    }
}


