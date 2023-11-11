// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ScoreSystem/ScoreCounter.h"

UScoreCounter::UScoreCounter(const FObjectInitializer& ObjectInitializer) :
	UUserWidget(ObjectInitializer),
	ScoreText(nullptr),
	TotalScore(0)
{
}

void UScoreCounter::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UTextBlock* pTextBlock = Cast<UTextBlock>(GetWidgetFromName("ScoreText"));
	ensure(pTextBlock != nullptr);
	ScoreText = pTextBlock;

	UpdateScore(0);
}


void UScoreCounter::UpdateScore(int score)
{
	TotalScore += score;
	FString str = FString::FromInt(TotalScore);
	ensure(ScoreText != nullptr);

	if (ScoreText)
	{
		ScoreText->SetText(FText::FromString(str));
	}
}

