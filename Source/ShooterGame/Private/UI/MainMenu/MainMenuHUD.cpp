// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/UI/MainMenu/MainMenuHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


void AMainMenuHUD::BeginPlay()
{
	FString Path = TEXT("/Game/ShooterGame/Blueprints/Widgets/BPW_MainMenu.BPW_MainMenu_C");
	TSubclassOf<UUserWidget> WidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*Path)).LoadSynchronous();
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (WidgetClass && PlayerController)
	{
		// Widget‚ðì¬‚µ‚ÄViewport‚É’Ç‰Á‚·‚é
		UUserWidget* UserWidget = UWidgetBlueprintLibrary::Create(GetWorld(), WidgetClass, PlayerController);
		float UserWidgetZOrder = 0;
		UserWidget->AddToViewport(UserWidgetZOrder);

		// MouseCursor‚ð•\Ž¦‚·‚é
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, UserWidget, EMouseLockMode::DoNotLock, true, false);
		PlayerController->SetShowMouseCursor(true);
	}
}
