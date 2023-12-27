// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameClearWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class SHOOTERGAME_API UGameClearWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// NativeConstruct
	void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	int GetTotalScore() { return TotalScore; }

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ButtonContinue;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ButtonTitle;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ButtonQuit;

	UFUNCTION()
	void OnButtonContinueClicked();

	UFUNCTION()
	void OnButtonTitleClicked();

	UFUNCTION()
	void OnButtonQuitClicked();

	UPROPERTY(meta = (BindWidget))
	int TotalScore;

public:
	FORCEINLINE void SetTotalScore(int score) { TotalScore = score; }
};
