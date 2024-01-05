// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreCounter.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class SHOOTERGAME_API UScoreCounter : public UUserWidget
{
	GENERATED_BODY()

public:
    UScoreCounter(const FObjectInitializer& ObjectInitializer);

    void UpdateScore(int score);
    void UpdateComboCount();

protected:
    UTextBlock* ScoreText;

    virtual void NativeOnInitialized() override;

    UTextBlock* ComboCounterText;

    UFUNCTION(BlueprintCallable)
    void CheckComboTimeLimit();

    int ComboCounter;
    float LastKillTime;
    float ComboTimeLimit;
    
private:
    int TotalScore;
    int MaxComboCount;

public:
    UFUNCTION(BlueprintCallable)
    FORCEINLINE int GetTotalScore() { return TotalScore; }  
    
    UFUNCTION(BlueprintCallable)
    FORCEINLINE int GetMaxComboCount() { return MaxComboCount; }    
    
    UFUNCTION(BlueprintCallable)
    FORCEINLINE int GetComboCounter() { return ComboCounter; }
};
