// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CountDown/CountDownWidget.h"
#include "Components/TextBlock.h"
#include "../Public/Core/InGameHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "../Public/Player/ShooterCharacter.h"
#include "../Public/UI/GameClear/GameClearWidget.h"


void UCountDownWidget::NativeConstruct()
{
    Super::NativeConstruct();

    TimeRemaining = 90.f;
    Minutes = FMath::FloorToInt(TimeRemaining / 60.0f);
    Seconds = FMath::FloorToInt(TimeRemaining) % 60;
    bShouldCountDown = false;

    // TimeLimitのカウントダウンをスタートさせるためのタイマーを設定
    float StartDelay = 3.0f;
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UCountDownWidget::StartTimeLimitCountDown, StartDelay, false);
}

void UCountDownWidget::SetTotalScore(int score)
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (!PlayerController) return;

    AHUD* HUD = PlayerController->GetHUD();
    AInGameHUD* InGameHUD = Cast<AInGameHUD>(HUD);
    if (!InGameHUD) return;

    UUserWidget* UserWidget = InGameHUD->GetGameClearWidget();
    UGameClearWidget* GameClearWidget = Cast<UGameClearWidget>(UserWidget);

    GameClearWidget->SetTotalScore(score);
}

void UCountDownWidget::SetMaxComboCount(int count)
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (!PlayerController) return;

    AHUD* HUD = PlayerController->GetHUD();
    AInGameHUD* InGameHUD = Cast<AInGameHUD>(HUD);
    if (!InGameHUD) return;

    UUserWidget* UserWidget = InGameHUD->GetGameClearWidget();
    UGameClearWidget* GameClearWidget = Cast<UGameClearWidget>(UserWidget);

    GameClearWidget->SetMaxComboCount(count);
}

void UCountDownWidget::UpdateCountDown(float DeltaTime)
{
    if (!bShouldCountDown) return;

    // ゲームがポーズされている場合はカウントダウンを進行させない
    if (UGameplayStatics::IsGamePaused(GetWorld()))
    {
        return;
    }

    TimeRemaining -= DeltaTime;
    
    Minutes = FMath::FloorToInt(TimeRemaining / 60.0f);
    Seconds = FMath::FloorToInt(TimeRemaining) % 60;

    if (TimeRemaining > 0.f) return;
    TimeRemaining = 0.f;

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (!PlayerController) return;

    AHUD* HUD = PlayerController->GetHUD();
    AInGameHUD* InGameHUD = Cast<AInGameHUD>(HUD);
    if (!InGameHUD) return;  

    AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(PlayerController->GetPawn());
    if (!ShooterCharacter) return;

    if (ClearSound)
    {
        UAudioComponent* AudioComp = UGameplayStatics::SpawnSound2D(GetWorld(), ClearSound);
        if (AudioComp)
        {
            AudioComp->bIsUISound = true; // 一時停止中でも再生されるように設定
        }
        UGameplayStatics::PlaySoundAtLocation(this, ClearSound, ShooterCharacter->GetActorLocation());
    }

    InGameHUD->DispGameClear();
    UUserWidget* UserWidget = InGameHUD->GetGameClearWidget();
    UGameClearWidget* GameClearWidget = Cast<UGameClearWidget>(UserWidget);

    bShouldCountDown = false;
}


