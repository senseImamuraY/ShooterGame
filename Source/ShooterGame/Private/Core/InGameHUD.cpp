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

	//// Tick���L�����ǂ������m�F
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

	// WidgetBlueprint��Class���擾����
	FString PauseWidgetPath = TEXT("/Game/ShooterGame/Blueprints/Widgets/BPW_Pause.BPW_Pause_C");
	TSubclassOf<UUserWidget> PauseWidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*PauseWidgetPath)).LoadSynchronous();

	// PlayerController���擾����
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// WidgetClass��PlayerController���擾�ł��������肷��
	if (PauseWidgetClass && PlayerController)
	{
		
		PauseWidget = UWidgetBlueprintLibrary::Create(GetWorld(), PauseWidgetClass, PlayerController);

		// Pause���j���[��܂肽���ݏ�Ԃɂ���
		PauseWidget->SetVisibility(ESlateVisibility::Collapsed);

		// Viewport�ɒǉ�����
		PauseWidget->AddToViewport(1);
	}

	// UCrosshairWidget�̃C���X�^���X���쐬���܂��B
	CrosshairWidgetInstance = CreateWidget<UCrosshairWidget>(GetWorld());
	if (CrosshairWidgetInstance)
	{
		CrosshairWidgetInstance->AddToViewport();
	}

	CrosshairWidgetInstance->InitializeWidget();

	// Tick���L�����ǂ������m�F
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

	//// �X�N���[���T�C�Y���擾
	//FVector2D ScreenSize = FVector2D(Canvas->SizeX, Canvas->SizeY);

	//// ��Ƃ��āA�X�N���[���T�C�Y����ʂɏo��
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Screen Size: %f x %f"), ScreenSize.X, ScreenSize.Y));
}

void AInGameHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CrosshairWidgetInstance->UpdateCrosshairPosition(DeltaTime);
}


void AInGameHUD::DispPause(const bool IsPause)
{
	// PlayerController���擾����
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	//GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Cyan, FString::Printf(TEXT("IsPause: %s"), IsPause ? TEXT("True") : TEXT("False")));

	if (IsPause)
	{
		// Pause���j���[��\������

		// Pause���j���[��\������
		PauseWidget->SetVisibility(ESlateVisibility::Visible);

		// UI���[�h�ɐݒ肷��
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerController, PauseWidget, EMouseLockMode::DoNotLock, false);

		// Game��Pause��Ԃɂ���
		UGameplayStatics::SetGamePaused(GetWorld(), true);

		// Mouse�J�[�\����\������
		PlayerController->SetShowMouseCursor(true);
	}
	else
	{
		// Pause��Ԃ���������

		// Game��Pause��Ԃ���������
		UGameplayStatics::SetGamePaused(GetWorld(), false);

		// Mouse�J�[�\�����\���ɂ���
		PlayerController->SetShowMouseCursor(false);

		// Game��Input��Ԃɖ߂�
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController, false);

		// Pause���j���[��܂肽���ݏ�Ԃɂ���
		PauseWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AInGameHUD::OpenLevel(const FName LevelName)
{
	// Level��Load����
	UGameplayStatics::OpenLevel(GetWorld(), LevelName);
}

void AInGameHUD::QuitGame()
{
	// �Q�[�����I������
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayerController(), EQuitPreference::Quit, false);
}


