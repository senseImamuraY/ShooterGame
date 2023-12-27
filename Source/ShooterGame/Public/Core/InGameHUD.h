// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InGameHUD.generated.h"

class UCrosshairWidget;

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AInGameHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	AInGameHUD();

	// Pauseメニューの表示/非表示
	void DispPause(const bool bIsPause);

	// Levelを開く
	void OpenLevel(const FName LevelName);

	// Gameを終了する
	void QuitGame();

	virtual void Tick(float DeltaTime) override;

	void DispGameClear();

	void DispGameOver();

	// Gameを再開する
	void ContinueGame();

protected:
	virtual void BeginPlay() override;

private:
	// PauseWidgetを保持する変数
	UUserWidget* PauseWidget;

	UUserWidget* GameClearWidget;

	UUserWidget* GameOverWidget;

	UPROPERTY()
	UCrosshairWidget* CrosshairWidgetInstance;

public:
	FORCEINLINE UUserWidget* GetGameClearWidget() { return GameClearWidget; }
};
