// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InGameHUD.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AInGameHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	// Pause���j���[�̕\��/��\��
	void DispPause(const bool IsPause);

	// Level���J��
	void OpenLevel(const FName LevelName);

	// Game���I������
	void QuitGame();
protected:
	virtual void BeginPlay() override;

private:
	// PauseWidget��ێ�����ϐ�
	UUserWidget* PauseWidget;
};
