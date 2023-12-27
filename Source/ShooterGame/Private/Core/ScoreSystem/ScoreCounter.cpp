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

	// �R���{�E�B���h�E���œG��|�������`�F�b�N
	if (LastKillTime > 0.f && (CurrentTime - LastKillTime) <= ComboTimeLimit)
	{
		// �R���{�J�E���^�𑝉�
		ComboCounter++;
		MaxComboCount = FMath::Max(MaxComboCount, ComboCounter);
	}
	else
	{
		// �R���{�J�E���^�����Z�b�g
		MaxComboCount = FMath::Max(MaxComboCount, ComboCounter);
		ComboCounter = 0;
	}

	// �Ō�ɓG��|�������Ԃ��X�V
	LastKillTime = CurrentTime;
	// �f�o�b�O���b�Z�[�W�̏o��
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Cyan, FString::Printf(TEXT("CurrentTime: %f, LastKillTime: %f, ComboCounter: %d, MaxComboCount: %d"), CurrentTime, LastKillTime, ComboCounter, MaxComboCount));
	}
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

