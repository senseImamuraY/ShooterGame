// Copyright Epic Games, Inc. All Rights Reserved.


#include "../Public/Core/ShooterGameGameModeBase.h"
#include "Core/InGameHUD.h"
#include "./Player/ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"

AShooterGameGameModeBase::AShooterGameGameModeBase() 
{
	HUDClass = AInGameHUD::StaticClass();
	PlayerControllerClass = AShooterPlayerController::StaticClass();
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

