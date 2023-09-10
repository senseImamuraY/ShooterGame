// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/MainMenu/MainMenuHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


void AMainMenuHUD::BeginPlay()
{
	// WidgetBlueprint��Class���擾����
	FString Path = TEXT("/Game/ShooterGame/Blueprints/Widgets/BPW_MainMenu.BPW_MainMenu_C");
	TSubclassOf<UUserWidget> WidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*Path)).LoadSynchronous();
	UE_LOG(LogTemp, Display, TEXT("Title"));
	// PlayerController���擾����
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	//UE_LOG(LogTemp, Display, TEXT(PlayerController));
	// WidgetClass��PlayerController���擾�ł��������肷��
		// PlayerController�̏�����ʂɏo��
	if (PlayerController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("PlayerController is valid."));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PlayerController is NOT valid."));
	}

	// WidgetClass�̏�����ʂɏo��
	if (WidgetClass)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("WidgetClass is valid."));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("WidgetClass is NOT valid."));
	}
	if (WidgetClass && PlayerController)
	{
		// Widget���쐬����
		UUserWidget* UserWidget = UWidgetBlueprintLibrary::Create(GetWorld(), WidgetClass, PlayerController);

		// Viewport�ɒǉ�����
		UserWidget->AddToViewport(0);

		// MouseCursor��\������
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, UserWidget, EMouseLockMode::DoNotLock, true, false);
		PlayerController->SetShowMouseCursor(true);
	}
}
