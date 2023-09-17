// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/UI/MainMenu/MainMenuGameMode.h"
#include "../Public/UI/MainMenu/MainMenuHUD.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	HUDClass = AMainMenuHUD::StaticClass();
}
