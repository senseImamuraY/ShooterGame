// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "ScoreCounter.generated.h"

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

protected:
    UTextBlock* ScoreText;

    virtual void NativeOnInitialized() override;

private:
    int TotalScore;

public:
    

};
