// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/Player/ShooterPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Core/InGameHUD.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h" 
#include "Components/InputComponent.h" // 追加
#include "../Public/Player/ShooterCharacter.h"
#include "Animation/AnimInstance.h"
#include "../Public/Player/ShooterAnimInstance.h"

AShooterPlayerController::AShooterPlayerController()
{
	// Input Action「IA_Pause」を読み込む
	PauseAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/ShooterGame/Input/Action/IA_Pause"));
	// Input Mapping Context「IM_Controls」を読み込む
	DefaultMappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/ShooterGame/Input/Action/IM_Controls"));

	static ConstructorHelpers::FClassFinder<UUserWidget> AmmoCountWidgetClass(TEXT("/Game/ShooterGame/Blueprints/Widgets/ShooterHUDOverlay.ShooterHUDOverlay_C"));
	if (AmmoCountWidgetClass.Succeeded())
	{
		HUDOverlayClass = AmmoCountWidgetClass.Class;
	}
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDOverlayClass)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayClass);
		if (HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}

	// AInGameHUDのインスタンスを作成
	InGameHUDInstance = GetWorld()->SpawnActor<AInGameHUD>();
	// 初期状態では非表示にする
	InGameHUDInstance->SetActorHiddenInGame(true);

}

void AShooterPlayerController::DispPause(const FInputActionValue& Value)
{
	// inputのValueはboolに変換できる
	if (const bool V = Value.Get<bool>())
	{
		// HUDのインスタンスを取得
		AHUD* CurrentHUD = GetHUD();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("CalledNum: %d"), ++CalledNum));

		//// HUDのインスタンスをログに出力
		//GEngine->AddOnScreenDebugMessage(4, 5.f, FColor::Red, FString::Printf(TEXT("CalledNum: %d"), CalledNum++) );
		//GEngine->AddOnScreenDebugMessage(4, 5.f, FColor::Red, FString::Printf(TEXT("Current HUD: %s"), *CurrentHUD->GetName()));

		//// キャストして確認
		//GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Red, FString::Printf(TEXT("Is it AInGameHUD?: %s"), Cast<AInGameHUD>(CurrentHUD) ? TEXT("True") : TEXT("False")));		// AInGameHUDを表示
		InGameHUDInstance->SetActorHiddenInGame(false);
		// Valueの内容を画面に表示
		if (AInGameHUD* HUD = Cast<AInGameHUD>(GetHUD()))
		{
			GEngine->AddOnScreenDebugMessage(7, 5.f, FColor::Green, FString::Printf(TEXT("Value: %s"), V ? TEXT("True") : TEXT("False")));

			// Pauseメニューの表示/非表示を実行する
			//HUD->DispPause(true);
			HUD->DispPause(!UGameplayStatics::IsGamePaused(GetWorld()));
		}
		//bool PauseValue = UGameplayStatics::IsGamePaused(GetWorld());
		//GEngine->AddOnScreenDebugMessage(5, 5.f, FColor::Red, FString::Printf(TEXT("UGameplayStatics::IsGamePaused(GetWorld()): %s"), PauseValue ? TEXT("True") : TEXT("False")));

		////InGameHUDInstance->DispPause(true);

		//InGameHUDInstance->DispPause(!UGameplayStatics::IsGamePaused(GetWorld()));

		//bool PauseValueAfter = UGameplayStatics::IsGamePaused(GetWorld());
		//GEngine->AddOnScreenDebugMessage(6, 5.f, FColor::Red, FString::Printf(TEXT("UGameplayStatics::IsGamePausedAfter(GetWorld()): %s"), PauseValueAfter ? TEXT("True") : TEXT("False")));

	}
	else
	{
		// AInGameHUDを非表示
		InGameHUDInstance->SetActorHiddenInGame(true);
	}
}

void AShooterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	EnableInput(controller);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent)) {
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::DispPause);
	}

	// Input Mapping Contextを登録する
	if (APlayerController* PlayerController = Cast<APlayerController>(controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AShooterPlayerController::SetPlayerEnabledState(bool bPlayerEnabled)
{
	if (bPlayerEnabled)
	{
		GetPawn()->EnableInput(this);
	}
	else
	{
		GetPawn()->DisableInput(this);
	}
}
