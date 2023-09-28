// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/UI/MainMenu/MainMenuHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


void AMainMenuHUD::BeginPlay()
{
	// WidgetBlueprint��Class���擾����
	FString Path = TEXT("/Game/ShooterGame/Blueprints/Widgets/BPW_MainMenu.BPW_MainMenu_C");
	TSubclassOf<UUserWidget> WidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*Path)).LoadSynchronous();
	
	// PlayerController���擾����
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (WidgetClass && PlayerController)
	{
		// Widget���쐬����
		UUserWidget* UserWidget = UWidgetBlueprintLibrary::Create(GetWorld(), WidgetClass, PlayerController);

		float UserWidgetZOrder = 0;

		// Viewport�ɒǉ�����
		UserWidget->AddToViewport(UserWidgetZOrder);

		// MouseCursor��\������
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, UserWidget, EMouseLockMode::DoNotLock, true, false);
		PlayerController->SetShowMouseCursor(true);
	}
}
