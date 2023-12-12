// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameClear/GameClearWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "../Public/Core/InGameHUD.h"


void UGameClearWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ButtonTitleのOnClickedに「OnButtonTitleClicked」を関連づける
	ButtonContinue->OnClicked.AddUniqueDynamic(this, &UGameClearWidget::OnButtonContinueClicked);

	// ButtonTitleのOnClickedに「OnButtonTitleClicked」を関連づける
	ButtonTitle->OnClicked.AddUniqueDynamic(this, &UGameClearWidget::OnButtonTitleClicked);

	// ButtonQuitのOnClickedに「OnButtonQuitClicked」を関連づける
	ButtonQuit->OnClicked.AddUniqueDynamic(this, &UGameClearWidget::OnButtonQuitClicked);
}

void UGameClearWidget::OnButtonContinueClicked()
{
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// Gameを再開する
	HUD->ContinueGame();
}

void UGameClearWidget::OnButtonTitleClicked()
{
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// Pauseを解除する
	HUD->OpenLevel(FName(TEXT("MainMenu")));
}

void UGameClearWidget::OnButtonQuitClicked()
{
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// ゲームを終了する
	HUD->QuitGame();
}
