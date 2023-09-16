// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameClear/GameClearWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "../Public/Core/InGameHUD.h"


void UGameClearWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ButtonTitle��OnClicked�ɁuOnButtonTitleClicked�v���֘A�Â���
	ButtonContinue->OnClicked.AddUniqueDynamic(this, &UGameClearWidget::OnButtonContinueClicked);

	// ButtonTitle��OnClicked�ɁuOnButtonTitleClicked�v���֘A�Â���
	ButtonTitle->OnClicked.AddUniqueDynamic(this, &UGameClearWidget::OnButtonTitleClicked);

	// ButtonQuit��OnClicked�ɁuOnButtonQuitClicked�v���֘A�Â���
	ButtonQuit->OnClicked.AddUniqueDynamic(this, &UGameClearWidget::OnButtonQuitClicked);
}

void UGameClearWidget::OnButtonContinueClicked()
{
	// PlayerController���擾����
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// InGameHUD�N���X���擾����
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// Game���ĊJ����
	HUD->ContinueGame();
}

void UGameClearWidget::OnButtonTitleClicked()
{
	// PlayerController���擾����
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// InGameHUD�N���X���擾����
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// Pause����������
	HUD->OpenLevel(FName(TEXT("MainMenu")));
}

void UGameClearWidget::OnButtonQuitClicked()
{
	// PlayerController���擾����
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// InGameHUD�N���X���擾����
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// �Q�[�����I������
	HUD->QuitGame();
}
