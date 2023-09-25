// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/Player/ShooterPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Core/InGameHUD.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h" 
#include "Components/InputComponent.h" // �ǉ�
#include "../Public/Player/ShooterCharacter.h"
#include "Animation/AnimInstance.h"
#include "../Public/Player/ShooterAnimInstance.h"

AShooterPlayerController::AShooterPlayerController()
{
	// Input Action�uIA_Pause�v��ǂݍ���
	PauseAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/ShooterGame/Input/Action/IA_Pause"));
	// Input Mapping Context�uIM_Controls�v��ǂݍ���
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

	// AInGameHUD�̃C���X�^���X���쐬
	InGameHUDInstance = GetWorld()->SpawnActor<AInGameHUD>();
	// ������Ԃł͔�\���ɂ���
	InGameHUDInstance->SetActorHiddenInGame(true);

}

void AShooterPlayerController::DispPause(const FInputActionValue& Value)
{
	// input��Value��bool�ɕϊ��ł���
	if (const bool V = Value.Get<bool>())
	{
		// HUD�̃C���X�^���X���擾
		AHUD* CurrentHUD = GetHUD();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("CalledNum: %d"), ++CalledNum));

		//// HUD�̃C���X�^���X�����O�ɏo��
		//GEngine->AddOnScreenDebugMessage(4, 5.f, FColor::Red, FString::Printf(TEXT("CalledNum: %d"), CalledNum++) );
		//GEngine->AddOnScreenDebugMessage(4, 5.f, FColor::Red, FString::Printf(TEXT("Current HUD: %s"), *CurrentHUD->GetName()));

		//// �L���X�g���Ċm�F
		//GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Red, FString::Printf(TEXT("Is it AInGameHUD?: %s"), Cast<AInGameHUD>(CurrentHUD) ? TEXT("True") : TEXT("False")));		// AInGameHUD��\��
		InGameHUDInstance->SetActorHiddenInGame(false);
		// Value�̓��e����ʂɕ\��
		if (AInGameHUD* HUD = Cast<AInGameHUD>(GetHUD()))
		{
			GEngine->AddOnScreenDebugMessage(7, 5.f, FColor::Green, FString::Printf(TEXT("Value: %s"), V ? TEXT("True") : TEXT("False")));

			// Pause���j���[�̕\��/��\�������s����
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
		// AInGameHUD���\��
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

	// Input Mapping Context��o�^����
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
