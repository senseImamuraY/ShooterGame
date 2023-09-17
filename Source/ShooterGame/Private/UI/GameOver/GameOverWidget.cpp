// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameOver/GameOverWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "../Public/Core/InGameHUD.h"


void UGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ButtonContinueのOnClickedに「OnButtonContinueClicked」を関連づける
	ButtonContinue->OnClicked.AddUniqueDynamic(this, &UGameOverWidget::OnButtonContinueClicked);

	// ButtonTitleのOnClickedに「OnButtonTitleClicked」を関連づける
	ButtonTitle->OnClicked.AddUniqueDynamic(this, &UGameOverWidget::OnButtonTitleClicked);

	// ButtonQuitのOnClickedに「OnButtonQuitClicked」を関連づける
	ButtonQuit->OnClicked.AddUniqueDynamic(this, &UGameOverWidget::OnButtonQuitClicked);
}

void UGameOverWidget::OnButtonContinueClicked()
{
	// PlayerControllerを取得する
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// InGameHUDクラスを取得する
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// Gameを再開する
	HUD->ContinueGame();
}

void UGameOverWidget::OnButtonTitleClicked()
{
	// PlayerControllerを取得する
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// InGameHUDクラスを取得する
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// Pauseを解除する
	HUD->OpenLevel(FName(TEXT("MainMenu")));
}

void UGameOverWidget::OnButtonQuitClicked()
{
	// PlayerControllerを取得する
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// InGameHUDクラスを取得する
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// ゲームを終了する
	HUD->QuitGame();
}