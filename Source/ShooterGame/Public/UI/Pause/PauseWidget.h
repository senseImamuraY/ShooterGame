// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseWidget.generated.h"

class UButton;

UCLASS()
class SHOOTERGAME_API UPauseWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	// NativeConstruct
	void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	UButton* ButtonResume;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonTitle;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonQuit;

	// ButtonResume��OnClicked�C�x���g�Ɋ֘A�Â���
	UFUNCTION()
	void OnButtonResumeClicked();

	// ButtonTitle��OnClicked�C�x���g�Ɋ֘A�Â���
	UFUNCTION()
	void OnButtonTitleClicked();

	// ButtonQuit��OnClicked�C�x���g�Ɋ֘A�Â���
	UFUNCTION()
	void OnButtonQuitClicked();
};
