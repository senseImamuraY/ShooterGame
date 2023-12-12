// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/UI/MainMenu/MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "../Public/Player/ShooterCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ButtonPlay��OnClicked�ɁuOnButtonPlayClicked�v���֘A�Â���
	ButtonPlay->OnClicked.AddUniqueDynamic(this, &UMainMenuWidget::OnButtonPlayClicked);

	// ButtonQuit��OnClicked�ɁuOnButtonQuitClicked�v���֘A�Â���
	ButtonQuit->OnClicked.AddUniqueDynamic(this, &UMainMenuWidget::OnButtonQuitClicked);
}

void UMainMenuWidget::OnButtonPlayClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("DefaultMap")));
}

void UMainMenuWidget::OnButtonQuitClicked()
{
	// �Q�[�����I������
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, false);
	}
}
