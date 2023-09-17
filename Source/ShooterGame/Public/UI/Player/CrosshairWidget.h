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

	// 初期化関数の宣言
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

	// 動的に作成するUImageの参照
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

	// 十字線の大きさを決定
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	// 十字線の速度
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	// 空中での十字線
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	// エイム中の十字線
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;

	// 銃撃の十字線
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;
};
