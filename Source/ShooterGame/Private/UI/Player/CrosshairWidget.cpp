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
	// UImageをCanvasPanelの子として追加
	 CanvasSlotTop = CanvasPanel->AddChildToCanvas(CrosshairImageTop);
	 CanvasSlotBottom = CanvasPanel->AddChildToCanvas(CrosshairImageBottom);
	 CanvasSlotLeft = CanvasPanel->AddChildToCanvas(CrosshairImageLeft);
	 CanvasSlotRight = CanvasPanel->AddChildToCanvas(CrosshairImageRight);

	if (CanvasSlotTop)
	{
		FVector2D ScreenSize(0.f);
	
		GEngine->GameViewport->GetViewportSize(ScreenSize);
		//ScreenSize /= 2.0f;
		// 画面にテキストを出力
		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("Screen Size: %f x %f"), ScreenSize.X, ScreenSize.Y));

		//ScreenSize.Y -= 50;
		// UImageの位置やサイズを設定
		CanvasSlotTop->SetAnchors(FAnchors(0.5f, 0.5f)); // 画面の中央にアンカーを設定
		CanvasSlotTop->SetAlignment(FVector2D(0.5f, 0.5f)); // 画像の中央をアンカー位置に合わせる
		//CanvasSlot->SetAlignment(ScreenSize); // 画像の中央をアンカー位置に合わせる
		CanvasSlotTop->SetPosition(FVector2D(0.f, -50.f));
		CanvasSlotTop->SetSize(FVector2D(128.f, 128.f)); // 画像のサイズを設定
	}

	if (CanvasSlotBottom)
	{
		CanvasSlotBottom->SetAnchors(FAnchors(0.5f, 0.5f));
		CanvasSlotBottom->SetAlignment(FVector2D(0.5f, 0.5f));
		CanvasSlotBottom->SetPosition(FVector2D(0.f, 50.f));
		CanvasSlotBottom->SetSize(FVector2D(128.f, 128.f));
	}

	if (CanvasSlotLeft)
	{
		CanvasSlotLeft->SetAnchors(FAnchors(0.5f, 0.5f));
		CanvasSlotLeft->SetAlignment(FVector2D(0.5f, 0.5f));
		CanvasSlotLeft->SetPosition(FVector2D(-50.f, 0.f));
		CanvasSlotLeft->SetSize(FVector2D(128.f, 128.f));
	}

	if (CanvasSlotRight)
	{
		CanvasSlotRight->SetAnchors(FAnchors(0.5f, 0.5f));
		CanvasSlotRight->SetAlignment(FVector2D(0.5f, 0.5f));
		CanvasSlotRight->SetPosition(FVector2D(50.f, 0.f));
		CanvasSlotRight->SetSize(FVector2D(128.f, 128.f));
	}


	CrosshairTop = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/ShooterGame/Textures/Crosshairs/Crosshair_Quad_9_Top.Crosshair_Quad_9_Top")));
	if (CrosshairTop)
	{
		//SetCrosshairTexture(CrosshairTop);
		CrosshairImageTop->SetBrushFromTexture(CrosshairTop);
	}

	CrosshairBottom = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/ShooterGame/Textures/Crosshairs/Crosshair_Quad_9_Bottom.Crosshair_Quad_9_Bottom")));
	if (CrosshairBottom)
	{
		//SetCrosshairTexture(CrosshairBottom);
		CrosshairImageBottom->SetBrushFromTexture(CrosshairBottom);
	}

	CrosshairLeft = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/ShooterGame/Textures/Crosshairs/Crosshair_Quad_9_Left.Crosshair_Quad_9_Left")));
	if (CrosshairLeft)
	{
		//SetCrosshairTexture(CrosshairLeft);
		CrosshairImageLeft->SetBrushFromTexture(CrosshairLeft);
	}

	CrosshairRight = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/ShooterGame/Textures/Crosshairs/Crosshair_Quad_9_Right.Crosshair_Quad_9_Right")));
	if (CrosshairRight)
	{
		SetCrosshairTexture(CrosshairRight);
		CrosshairImageRight->SetBrushFromTexture(CrosshairRight);
	}
}

void UCrosshairWidget::UpdateCrosshairPosition(float Deltatime)
{
	// CalculateCrosshairSpread関数を呼び出してCrosshairSpreadMultiplierを更新
	CalculateCrosshairSpread(Deltatime);

	// 全てのPositionを変更

	// 現在の位置にCrosshairSpreadMultiplierを掛けて新しい位置を計算
	FVector2D NewTopPosition = FVector2D(0.f, -50.f) * 1.0f * CrosshairSpreadMultiplier;
	// 新しい位置を設定
	CanvasSlotTop->SetPosition(NewTopPosition);

	// 現在の位置にCrosshairSpreadMultiplierを掛けて新しい位置を計算
	FVector2D NewBottomPosition = FVector2D(0.f, 50.f) * 1.0f * CrosshairSpreadMultiplier;
	// 新しい位置を設定
	CanvasSlotBottom->SetPosition(NewBottomPosition);

	// 現在の位置にCrosshairSpreadMultiplierを掛けて新しい位置を計算
	FVector2D NewLeftPosition = FVector2D(-50.f, 0.f) * 1.0f * CrosshairSpreadMultiplier;
	// 新しい位置を設定
	CanvasSlotLeft->SetPosition(NewLeftPosition);

	// 現在の位置にCrosshairSpreadMultiplierを掛けて新しい位置を計算
	FVector2D NewRightPosition = FVector2D(50.f, 0.f) * 1.0f * CrosshairSpreadMultiplier;
	// 新しい位置を設定
	CanvasSlotRight->SetPosition(NewRightPosition);


}


void UCrosshairWidget::NativeConstruct()
{
	//ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
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

void UCrosshairWidget::SetCrosshairTexture(UTexture2D* Texture)
{
	//if (CrosshairImage)
	//{
	//	CrosshairImage->SetBrushFromTexture(Texture);
	//}
}

void UCrosshairWidget::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{ 0.f, 600.f };
	FVector2D VelocityMultiplierRange{ 0.f, 1.f };
	FVector Velocity{ ShooterCharacter->GetVelocity() };
	Velocity.Z = 0.f;

	// 速度による十字線調整
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
		WalkSpeedRange,
		VelocityMultiplierRange,
		Velocity.Size());

	// 空中にいるときの十字線を計算
	if (ShooterCharacter->GetCharacterMovement()->IsFalling())
	{
		// 空中にいるときは、十字線を拡張する
		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor,
			2.25f,
			DeltaTime,
			2.25f);
	}
	else
	{
		// 地面にいるときは、十字線を縮小する
		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor, 
			0.f,
			DeltaTime,
			30.f);
	}

	// エイム中の計算
	if (ShooterCharacter->GetAiming())
	{
		// エイム中は、十字線を縮小する
		CrosshairAimFactor = FMath::FInterpTo(
			CrosshairAimFactor,
			0.6f, 
			DeltaTime,
			30.f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(
			CrosshairAimFactor,
			0.f, 
			DeltaTime,
			30.f);
	}

	// 射撃後0.05sの間のみtrueになる
	if (ShooterCharacter->GetbFiringBullet())
	{
		CrosshairShootingFactor = FMath::FInterpTo(
			CrosshairShootingFactor, 
			0.3f,
			DeltaTime,
			60.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(
			CrosshairShootingFactor,
			0.f,
			DeltaTime,
			60.f);
	}

	CrosshairSpreadMultiplier = 
		0.5f +
		CrosshairVelocityFactor +
		CrosshairInAirFactor -
		CrosshairAimFactor + 
		CrosshairShootingFactor;
}
