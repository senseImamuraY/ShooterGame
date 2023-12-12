// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameOver/GameOverWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "../Public/Core/InGameHUD.h"


void UGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ButtonContinue��OnClicked�ɁuOnButtonContinueClicked�v���֘A�Â���
	ButtonContinue->OnClicked.AddUniqueDynamic(this, &UGameOverWidget::OnButtonContinueClicked);

	// ButtonTitle��OnClicked�ɁuOnButtonTitleClicked�v���֘A�Â���
	ButtonTitle->OnClicked.AddUniqueDynamic(this, &UGameOverWidget::OnButtonTitleClicked);

	// ButtonQuit��OnClicked�ɁuOnButtonQuitClicked�v���֘A�Â���
	ButtonQuit->OnClicked.AddUniqueDynamic(this, &UGameOverWidget::OnButtonQuitClicked);
}

void UGameOverWidget::OnButtonContinueClicked()
{
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// Game���ĊJ����
	HUD->ContinueGame();
}

void UGameOverWidget::OnButtonTitleClicked()
{
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// Pause����������
	HUD->OpenLevel(FName(TEXT("MainMenu")));
}

void UGameOverWidget::OnButtonQuitClicked()
{
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// �Q�[�����I������
	HUD->QuitGame();
}