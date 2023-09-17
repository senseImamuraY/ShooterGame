// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Texture2D.h"
#include "Runtime/UMG/Public/Components/Image.h"
#include "CrosshairWidget.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UCrosshairWidget(const FObjectInitializer& ObjectInitializer);

	// �������֐��̐錾
	void InitializeWidget();

	void UpdateCrosshairPosition(float Deltatime);

protected:
	
	void NativeConstruct() override;

	void CalculateCrosshairSpread(float DeltaTime);

	void SetCrosshairTexture(UTexture2D* Textrure);

	UPROPERTY(EditDefaultsOnly, Category = "Texture")
	UTexture2D* CrosshairTop;

	UPROPERTY(EditDefaultsOnly, Category = "Texture")
	UTexture2D* CrosshairBottom;

	UPROPERTY(EditDefaultsOnly, Category = "Texture")
	UTexture2D* CrosshairLeft;

	UPROPERTY(EditDefaultsOnly, Category = "Texture")
	UTexture2D* CrosshairRight;

	// ���I�ɍ쐬����UImage�̎Q��
	UPROPERTY()
	UImage* CrosshairImageTop;

	UPROPERTY()
	UImage* CrosshairImageBottom;

	UPROPERTY()
	UImage* CrosshairImageLeft;

	UPROPERTY()
	UImage* CrosshairImageRight;

	UPROPERTY()
	class UCanvasPanel* CanvasPanel;

	class UCanvasPanelSlot* CanvasSlotTop;
	UCanvasPanelSlot* CanvasSlotBottom;
	UCanvasPanelSlot* CanvasSlotLeft;
	UCanvasPanelSlot* CanvasSlotRight;

private:
	class AShooterCharacter* ShooterCharacter;

	// �\�����̑傫��������
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	// �\�����̑��x
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	// �󒆂ł̏\����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	// �G�C�����̏\����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;

	// �e���̏\����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;
};
