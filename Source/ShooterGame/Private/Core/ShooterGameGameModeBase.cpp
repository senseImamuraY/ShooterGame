// Copyright Epic Games, Inc. All Rights Reserved.


#include "../Public/Core/ShooterGameGameModeBase.h"
#include "Core/InGameHUD.h"
#include "./Player/ShooterPlayerController.h"

AShooterGameGameModeBase::AShooterGameGameModeBase()
{
	HUDClass = AInGameHUD::StaticClass();
	PlayerControllerClass = AShooterPlayerController::StaticClass();
}

void AShooterGameGameModeBase::GameClear()
{
	UE_LOG(LogTemp, Display, TEXT("GameClear"));
}

