// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/InGameHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../Public/Player/ShooterPlayerController.h"
#include "../Public/UI/Player/CrosshairWidget.h"
#include "CanvasItem.h"
#include "Engine/Canvas.h"

AInGameHUD::AInGameHUD()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AInGameHUD::BeginPlay()
{
	Super::BeginPlay();

	// WidgetBlueprintのClassを取得する
	FString PauseWidgetPath = TEXT("/Game/ShooterGame/Blueprints/Widgets/BPW_Pause.BPW_Pause_C");
	TSubclassOf<UUserWidget> PauseWidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*PauseWidgetPath)).LoadSynchronous();

	FString GameClearWidgetPath = TEXT("/Game/ShooterGame/Blueprints/Widgets/BPW_GameClear.BPW_GameClear_C");
	TSubclassOf<UUserWidget> GameClearWidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*GameClearWidgetPath)).LoadSynchronous();
	
	FString GameOverWidgetPath = TEXT("/Game/ShooterGame/Blueprints/Widgets/BPW_GameOver.BPW_GameOver_C");
	TSubclassOf<UUserWidget> GameOverWidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*GameOverWidgetPath)).LoadSynchronous();

	// PlayerControllerを取得する
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (GameOverWidgetClass && GameClearWidgetClass && PauseWidgetClass && PlayerController)
	{
		const int GameClearWidgetZOrder = 1;
		const int GameOverWidgetZOrder = 2;
		const int PauseWidgetZOrder = 3;

		PauseWidget = UWidgetBlueprintLibrary::Create(GetWorld(), PauseWidgetClass, PlayerController);

		// Pauseメニューを折りたたみ状態にする
		PauseWidget->SetVisibility(ESlateVisibility::Collapsed);
		PauseWidget->AddToViewport(PauseWidgetZOrder);

		GameClearWidget = UWidgetBlueprintLibrary::Create(GetWorld(), GameClearWidgetClass, PlayerController);
		GameClearWidget->SetVisibility(ESlateVisibility::Collapsed);
		GameClearWidget->AddToViewport(GameClearWidgetZOrder);

		GameOverWidget = UWidgetBlueprintLibrary::Create(GetWorld(), GameOverWidgetClass, PlayerController);
		GameOverWidget->SetVisibility(ESlateVisibility::Collapsed);
		GameOverWidget->AddToViewport(GameOverWidgetZOrder);
	}

	CrosshairWidgetInstance = CreateWidget<UCrosshairWidget>(GetWorld());

	if (CrosshairWidgetInstance)
	{
		CrosshairWidgetInstance->AddToViewport();
	}

	CrosshairWidgetInstance->InitializeWidget();
}

void AInGameHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CrosshairWidgetInstance->UpdateCrosshairPosition(DeltaTime);
}

void AInGameHUD::DispGameClear()
{
	GameClearWidget->SetVisibility(ESlateVisibility::Visible);

	APlayerController* PlayerController = GetOwningPlayerController();
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerController, GameClearWidget, EMouseLockMode::DoNotLock, false);
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	PlayerController->SetShowMouseCursor(true);
}

void AInGameHUD::DispGameOver()
{
	GameOverWidget->SetVisibility(ESlateVisibility::Visible);

	APlayerController* PlayerController = GetOwningPlayerController();
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerController, GameOverWidget, EMouseLockMode::DoNotLock, false);
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	PlayerController->SetShowMouseCursor(true);
}

void AInGameHUD::ContinueGame()
{
	// 現在のLevelNameを取得する
	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());

	// 現在のLevelを開きなおす
	UGameplayStatics::OpenLevel(GetWorld(), FName(*CurrentLevelName));
}


void AInGameHUD::DispPause(const bool bIsPause)
{
	// PlayerControllerを取得する
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (bIsPause)
	{
		// Pauseメニューを表示する

		// Pauseメニューを表示する
		PauseWidget->SetVisibility(ESlateVisibility::Visible);

		// UIモードに設定する
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerController, PauseWidget, EMouseLockMode::DoNotLock, false);

		// GameをPause状態にする
		UGameplayStatics::SetGamePaused(GetWorld(), true);

		// Mouseカーソルを表示する
		PlayerController->SetShowMouseCursor(true);
	}
	else
	{
		// Pause状態を解除する

		// GameのPause状態を解除する
		UGameplayStatics::SetGamePaused(GetWorld(), false);

		// Mouseカーソルを非表示にする
		PlayerController->SetShowMouseCursor(false);

		// GameのInput状態に戻す
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController, false);

		// Pauseメニューを折りたたみ状態にする
		PauseWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AInGameHUD::OpenLevel(const FName LevelName)
{
	// LevelをLoadする
	UGameplayStatics::OpenLevel(GetWorld(), LevelName);
}

void AInGameHUD::QuitGame()
{
	// ゲームを終了する
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayerController(), EQuitPreference::Quit, false);
}


