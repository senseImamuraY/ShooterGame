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
	//	// ���݂�LevelName���擾����
	//const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());

	//// ���݂�Level���J���Ȃ���
	//UGameplayStatics::OpenLevel(GetWorld(), FName(*CurrentLevelName));
	RestartGame();
}

void AShooterGameGameModeBase::RestartGame()
{
	// PlayerController���擾����
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// InGameHUD�N���X���擾����
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// �Q�[���I�[�o�[��ʂ�\������
	HUD->DispGameOver();
}

