// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InGameHUD.generated.h"

class UCrosshairWidget;
class UAudioComponent;
class USoundCue;

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AInGameHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	AInGameHUD();

	// Pause���j���[�̕\��/��\��
	void DispPause(const bool bIsPause);

	// Level���J��
	void OpenLevel(const FName LevelName);

	// Game���I������
	void QuitGame();

	virtual void Tick(float DeltaTime) override;

	void DispGameClear();

	void DispGameOver();

	// Game���ĊJ����
	void ContinueGame();

protected:
	virtual void BeginPlay() override;

private:
	// PauseWidget��ێ�����ϐ�
	UUserWidget* PauseWidget;

	UUserWidget* GameClearWidget;

	UUserWidget* GameOverWidget;

	UPROPERTY()
	UCrosshairWidget* CrosshairWidgetInstance;

	// �Q�[���N���A�Ɠ����ɃQ�[���I�[�o�[�ɂȂ�Ȃ��悤�ɂ���
	bool bIsGameClear;
	bool bIsGameOver;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BGM, meta = (AllowPrivateAccess = "true"))
	USoundCue* GameSound;

	UAudioComponent* BGMComponent;

public:
	FORCEINLINE UUserWidget* GetGameClearWidget() { return GameClearWidget; }
	FORCEINLINE bool GetbIsGameOver() { return bIsGameOver; }
};
