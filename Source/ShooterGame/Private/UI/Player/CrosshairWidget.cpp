// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/CrosshairWidget.h"
#include "../Public/Player/ShooterCharacter.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CanvasPanel.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"

UCrosshairWidget::UCrosshairWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), 
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f)
{
	// WidgetTreeの初期化
	WidgetTree = ObjectInitializer.CreateDefaultSubobject<UWidgetTree>(this, TEXT("WidgetTree"));

	// UCanvasPanelをWidgetTreeを使用して作成
	CanvasPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("CanvasPanel"));

	// CanvasPanelをルートウィジェットとして設定
	WidgetTree->RootWidget = CanvasPanel;

	// UImageをWidgetTreeを使用して作成
	CrosshairImageTop = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("CrosshairImageTop"));
	CrosshairImageBottom = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("CrosshairImageBottom"));
	CrosshairImageLeft = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("CrosshairImageLeft"));
	CrosshairImageRight = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("CrosshairImageRight"));
}

void UCrosshairWidget::InitializeWidget()
{
	const FVector2D CenterAnchors(0.5f, 0.5f);
	const FVector2D CenterAlignment(0.5f, 0.5f);
	const FVector2D ImageSize(128.f, 128.f);
	const FString BaseTexturePath = TEXT("/Game/ShooterGame/Textures/Crosshairs/Crosshair_Quad_9_");

	// UImageの設定を行うヘルパー関数
	auto SetupCanvasSlot = [&](UCanvasPanelSlot* CanvasSlot, const FVector2D& Position) {
		if (CanvasSlot)
		{
			CanvasSlot->SetAnchors(FAnchors(CenterAnchors.X, CenterAnchors.Y)); // 修正
			CanvasSlot->SetAlignment(CenterAlignment);
			CanvasSlot->SetPosition(Position);
			CanvasSlot->SetSize(ImageSize);
		}
	};

	// UImageをCanvasPanelの子として追加
	CanvasSlotTop = CanvasPanel->AddChildToCanvas(CrosshairImageTop);
	CanvasSlotBottom = CanvasPanel->AddChildToCanvas(CrosshairImageBottom);
	CanvasSlotLeft = CanvasPanel->AddChildToCanvas(CrosshairImageLeft);
	CanvasSlotRight = CanvasPanel->AddChildToCanvas(CrosshairImageRight);

	// UImageの位置を設定
	SetupCanvasSlot(CanvasSlotTop, FVector2D(0.f, -50.f));
	SetupCanvasSlot(CanvasSlotBottom, FVector2D(0.f, 50.f));
	SetupCanvasSlot(CanvasSlotLeft, FVector2D(-50.f, 0.f));
	SetupCanvasSlot(CanvasSlotRight, FVector2D(50.f, 0.f));

	// テクスチャの読み込みと設定を行うヘルパー関数
	auto SetupCrosshairImage = [&](UImage* Image, const FString& Direction) {
		UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *(BaseTexturePath + Direction)));

		if (Texture)
		{
			Image->SetBrushFromTexture(Texture);
		}
	};

	// テクスチャの読み込みと設定
	SetupCrosshairImage(CrosshairImageTop, TEXT("Top.Crosshair_Quad_9_Top"));
	SetupCrosshairImage(CrosshairImageBottom, TEXT("Bottom.Crosshair_Quad_9_Bottom"));
	SetupCrosshairImage(CrosshairImageLeft, TEXT("Left.Crosshair_Quad_9_Left"));
	SetupCrosshairImage(CrosshairImageRight, TEXT("Right.Crosshair_Quad_9_Right"));
}


void UCrosshairWidget::UpdateCrosshairPosition(float Deltatime)
{
	// CalculateCrosshairSpread関数を呼び出してCrosshairSpreadMultiplierを更新
	CalculateCrosshairSpread(Deltatime);

	const FVector2D BaseOffsetTop = FVector2D(0.f, -50.f);
	const FVector2D BaseOffsetBottom = FVector2D(0.f, 50.f);
	const FVector2D BaseOffsetLeft = FVector2D(-50.f, 0.f);
	const FVector2D BaseOffsetRight = FVector2D(50.f, 0.f);

	// 全てのPositionを変更

	// 現在の位置にCrosshairSpreadMultiplierを掛けて新しい位置を計算
	FVector2D NewTopPosition = BaseOffsetTop * CrosshairSpreadMultiplier;
	CanvasSlotTop->SetPosition(NewTopPosition);

	FVector2D NewBottomPosition = BaseOffsetBottom * CrosshairSpreadMultiplier;
	CanvasSlotBottom->SetPosition(NewBottomPosition);

	FVector2D NewLeftPosition = BaseOffsetLeft * CrosshairSpreadMultiplier;
	CanvasSlotLeft->SetPosition(NewLeftPosition);

	FVector2D NewRightPosition = BaseOffsetRight * CrosshairSpreadMultiplier;
	CanvasSlotRight->SetPosition(NewRightPosition);
}

void UCrosshairWidget::NativeConstruct()
{
	UWorld* World = GetWorld();

	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			ShooterCharacter = Cast<AShooterCharacter>(PlayerController->GetPawn());
		}
	}
}

void UCrosshairWidget::CalculateCrosshairSpread(float DeltaTime)
{
	const FVector2D WalkSpeedRange{ 0.f, 600.f };
	const FVector2D VelocityMultiplierRange{ 0.f, 1.f };

	const float InterpSpeedSmall = 2.25f;
	const float InterpSpeedMedium = 30.f;
	const float InterpSpeedLarge = 60.f;

	const float BaseSpread = 0.5f;
	const float NoneSpread = 0.f;
	const float SmallSpread = 0.3f;
	const float MediumSpread = 0.6f;
	const float LargeSpread = 2.25f;

	FVector Velocity{ ShooterCharacter->GetVelocity() };
	Velocity.Z = 0.f;

	// 速度による十字線調整
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
		WalkSpeedRange,
		VelocityMultiplierRange,
		Velocity.Size());

	// 空中にいるときの十字線を計算
	CrosshairInAirFactor = FMath::FInterpTo(
		CrosshairInAirFactor,
		ShooterCharacter->GetCharacterMovement()->IsFalling() ? LargeSpread : NoneSpread,
		DeltaTime,
		ShooterCharacter->GetCharacterMovement()->IsFalling() ? InterpSpeedSmall : InterpSpeedMedium);

	// エイム中の計算
	CrosshairAimFactor = FMath::FInterpTo(
		CrosshairAimFactor,
		ShooterCharacter->GetAiming() ? MediumSpread : NoneSpread,
		DeltaTime,
		InterpSpeedMedium);

	// 射撃後の計算
	CrosshairShootingFactor = FMath::FInterpTo(
		CrosshairShootingFactor,
		ShooterCharacter->GetbFiringBullet() ? SmallSpread : NoneSpread,
		DeltaTime,
		InterpSpeedLarge);

	CrosshairSpreadMultiplier =
		BaseSpread +
		CrosshairVelocityFactor +
		CrosshairInAirFactor -
		CrosshairAimFactor +
		CrosshairShootingFactor;
}

