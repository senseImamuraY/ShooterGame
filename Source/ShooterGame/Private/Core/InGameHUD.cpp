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

	//// Tickが有効かどうかを確認
	//if (IsActorTickEnabled())
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Tick is enabled for this actor."));
	//}
	//else
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Tick is NOT enabled for this actor."));
	//}
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

	// WidgetClassとPlayerControllerが取得できたか判定する
	if (GameOverWidgetClass && GameClearWidgetClass && PauseWidgetClass && PlayerController)
	{
		PauseWidget = UWidgetBlueprintLibrary::Create(GetWorld(), PauseWidgetClass, PlayerController);

		// Pauseメニューを折りたたみ状態にする
		PauseWidget->SetVisibility(ESlateVisibility::Collapsed);

		PauseWidget->AddToViewport(0);

		GameClearWidget = UWidgetBlueprintLibrary::Create(GetWorld(), GameClearWidgetClass, PlayerController);

		GameClearWidget->SetVisibility(ESlateVisibility::Collapsed);

		GameClearWidget->AddToViewport(1);

		GameOverWidget = UWidgetBlueprintLibrary::Create(GetWorld(), GameOverWidgetClass, PlayerController);

		GameOverWidget->SetVisibility(ESlateVisibility::Collapsed);

		GameOverWidget->AddToViewport(2);
	}

	// UCrosshairWidgetのインスタンスを作成します。
	CrosshairWidgetInstance = CreateWidget<UCrosshairWidget>(GetWorld());
	if (CrosshairWidgetInstance)
	{
		CrosshairWidgetInstance->AddToViewport();
	}

	CrosshairWidgetInstance->InitializeWidget();

	// Tickが有効かどうかを確認
	if (IsActorTickEnabled())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Tick is enabled for this actor."));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Tick is NOT enabled for this actor."));
	}
}

void AInGameHUD::DrawHUD()
{
	//Super::DrawHUD();

	//// スクリーンサイズを取得
	//FVector2D ScreenSize = FVector2D(Canvas->SizeX, Canvas->SizeY);

	//// 例として、スクリーンサイズを画面に出力
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Screen Size: %f x %f"), ScreenSize.X, ScreenSize.Y));
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


void AInGameHUD::DispPause(const bool IsPause)
{
	// PlayerControllerを取得する
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	//GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Cyan, FString::Printf(TEXT("IsPause: %s"), IsPause ? TEXT("True") : TEXT("False")));

	if (IsPause)
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


