// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CountDownWidget.generated.h"

class UTextBlock;
class USoundCue;
/**
 * 
 */
UCLASS()
class SHOOTERGAME_API UCountDownWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "CountDown")
    void UpdateCountDown(float DeltaTime);

protected:
    void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* MinutesText;

    float Minutes;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SecondsText;

    float Seconds;
    
    UPROPERTY(BlueprintReadWrite, Category = "CountDown")
    float TimeRemaining;
    
    bool bShouldCountDown;

    FORCEINLINE void StartTimeLimitCountDown() { bShouldCountDown = true; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Goal, meta = (AllowPrivateAccess = "true"))
    USoundCue* ClearSound;

public:
    UFUNCTION(BlueprintPure, Category = "CountDown")
    float GetMinutes() { return Minutes; }

    UFUNCTION(BlueprintPure, Category = "CountDown")
    float GetSeconds() { return Seconds; }
};
