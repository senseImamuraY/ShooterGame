// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Pause/PauseWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "../Public/Core/InGameHUD.h"

void UPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ButtonResumeのOnClickedに「OnButtonResumeClicked」を関連づける
	ButtonResume->OnClicked.AddUniqueDynamic(this, &UPauseWidget::OnButtonResumeClicked);

	// ButtonTitleのOnClickedに「OnButtonTitleClicked」を関連づける
	ButtonTitle->OnClicked.AddUniqueDynamic(this, &UPauseWidget::OnButtonTitleClicked);

	// ButtonQuitのOnClickedに「OnButtonQuitClicked」を関連づける
	ButtonQuit->OnClicked.AddUniqueDynamic(this, &UPauseWidget::OnButtonQuitClicked);
}

void UPauseWidget::OnButtonResumeClicked()
{
	// PlayerControllerを取得する
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// InGameHUDクラスを取得する
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// Pauseを解除する
	HUD->DispPause(false);
}

void UPauseWidget::OnButtonQuitClicked()
{
	// PlayerControllerを取得する
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// InGameHUDクラスを取得する
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// ゲームを終了する
	HUD->QuitGame();
}

void UPauseWidget::OnButtonTitleClicked()
{
	// PlayerControllerを取得する
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// InGameHUDクラスを取得する
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// Pauseを解除する
	HUD->OpenLevel(FName(TEXT("MainMenu")));
}