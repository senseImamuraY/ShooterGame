// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ScoreSystem/ScoreCounter.h"
#include "Components/TextBlock.h"

UScoreCounter::UScoreCounter(const FObjectInitializer& ObjectInitializer) :
	UUserWidget(ObjectInitializer),
	ScoreText(nullptr),
	TotalScore(0),
	ComboCounter(0),
	MaxComboCount(0),
	ComboTimeLimit(10.f),
	LastKillTime(0.f)
{
}

void UScoreCounter::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UTextBlock* pTextBlock = Cast<UTextBlock>(GetWidgetFromName("ScoreText"));
	ensure(pTextBlock != nullptr);
	ScoreText = pTextBlock;

	UpdateScore(0);
	LastKillTime = GetWorld()->GetTimeSeconds();
}

void UScoreCounter::UpdateComboCount()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();

	// §ŒÀŽžŠÔˆÈ“à‚É“G‚ð“|‚¹‚Ä‚¢‚ê‚ÎƒRƒ“ƒ{”‚ð‘‰Á‚³‚¹‚é
	if (LastKillTime > 0.f && (CurrentTime - LastKillTime) <= ComboTimeLimit)
	{
		ComboCounter++;
		MaxComboCount = FMath::Max(MaxComboCount, ComboCounter);
	}
	else
	{
		MaxComboCount = FMath::Max(MaxComboCount, ComboCounter);
		ComboCounter = 0;
	}

	LastKillTime = CurrentTime;
}

void UScoreCounter::CheckComboTimeLimit()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();

	if (LastKillTime > 0.f && (CurrentTime - LastKillTime) > ComboTimeLimit)
	{
		MaxComboCount = FMath::Max(MaxComboCount, ComboCounter);
		ComboCounter = 0;
		LastKillTime = 0.f;
	}
}


void UScoreCounter::UpdateScore(int score)
{
	TotalScore += score + score * (ComboCounter * 0.3);
	FString str = FString::FromInt(TotalScore);
	ensure(ScoreText != nullptr);

	if (ScoreText)
	{
		ScoreText->SetText(FText::FromString(str));
	}
}

