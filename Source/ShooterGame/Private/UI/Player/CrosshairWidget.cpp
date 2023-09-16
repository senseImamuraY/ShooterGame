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
	// WidgetTree�̏�����
	WidgetTree = ObjectInitializer.CreateDefaultSubobject<UWidgetTree>(this, TEXT("WidgetTree"));

	// UCanvasPanel��WidgetTree���g�p���č쐬
	CanvasPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("CanvasPanel"));

	// CanvasPanel�����[�g�E�B�W�F�b�g�Ƃ��Đݒ�
	WidgetTree->RootWidget = CanvasPanel;

	// UImage��WidgetTree���g�p���č쐬
	CrosshairImageTop = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("CrosshairImageTop"));
	CrosshairImageBottom = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("CrosshairImageBottom"));
	CrosshairImageLeft = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("CrosshairImageLeft"));
	CrosshairImageRight = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("CrosshairImageRight"));
}

void UCrosshairWidget::InitializeWidget()
{
	// UImage��CanvasPanel�̎q�Ƃ��Ēǉ�
	 CanvasSlotTop = CanvasPanel->AddChildToCanvas(CrosshairImageTop);
	 CanvasSlotBottom = CanvasPanel->AddChildToCanvas(CrosshairImageBottom);
	 CanvasSlotLeft = CanvasPanel->AddChildToCanvas(CrosshairImageLeft);
	 CanvasSlotRight = CanvasPanel->AddChildToCanvas(CrosshairImageRight);

	if (CanvasSlotTop)
	{
		FVector2D ScreenSize(0.f);
	
		GEngine->GameViewport->GetViewportSize(ScreenSize);
		//ScreenSize /= 2.0f;
		// ��ʂɃe�L�X�g���o��
		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("Screen Size: %f x %f"), ScreenSize.X, ScreenSize.Y));

		//ScreenSize.Y -= 50;
		// UImage�̈ʒu��T�C�Y��ݒ�
		CanvasSlotTop->SetAnchors(FAnchors(0.5f, 0.5f)); // ��ʂ̒����ɃA���J�[��ݒ�
		CanvasSlotTop->SetAlignment(FVector2D(0.5f, 0.5f)); // �摜�̒������A���J�[�ʒu�ɍ��킹��
		//CanvasSlot->SetAlignment(ScreenSize); // �摜�̒������A���J�[�ʒu�ɍ��킹��
		CanvasSlotTop->SetPosition(FVector2D(0.f, -50.f));
		CanvasSlotTop->SetSize(FVector2D(128.f, 128.f)); // �摜�̃T�C�Y��ݒ�
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
	// CalculateCrosshairSpread�֐����Ăяo����CrosshairSpreadMultiplier���X�V
	CalculateCrosshairSpread(Deltatime);

	// �S�Ă�Position��ύX

	// ���݂̈ʒu��CrosshairSpreadMultiplier���|���ĐV�����ʒu���v�Z
	FVector2D NewTopPosition = FVector2D(0.f, -50.f) * 1.0f * CrosshairSpreadMultiplier;
	// �V�����ʒu��ݒ�
	CanvasSlotTop->SetPosition(NewTopPosition);

	// ���݂̈ʒu��CrosshairSpreadMultiplier���|���ĐV�����ʒu���v�Z
	FVector2D NewBottomPosition = FVector2D(0.f, 50.f) * 1.0f * CrosshairSpreadMultiplier;
	// �V�����ʒu��ݒ�
	CanvasSlotBottom->SetPosition(NewBottomPosition);

	// ���݂̈ʒu��CrosshairSpreadMultiplier���|���ĐV�����ʒu���v�Z
	FVector2D NewLeftPosition = FVector2D(-50.f, 0.f) * 1.0f * CrosshairSpreadMultiplier;
	// �V�����ʒu��ݒ�
	CanvasSlotLeft->SetPosition(NewLeftPosition);

	// ���݂̈ʒu��CrosshairSpreadMultiplier���|���ĐV�����ʒu���v�Z
	FVector2D NewRightPosition = FVector2D(50.f, 0.f) * 1.0f * CrosshairSpreadMultiplier;
	// �V�����ʒu��ݒ�
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

	// ���x�ɂ��\��������
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
		WalkSpeedRange,
		VelocityMultiplierRange,
		Velocity.Size());

	// �󒆂ɂ���Ƃ��̏\�������v�Z
	if (ShooterCharacter->GetCharacterMovement()->IsFalling())
	{
		// �󒆂ɂ���Ƃ��́A�\�������g������
		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor,
			2.25f,
			DeltaTime,
			2.25f);
	}
	else
	{
		// �n�ʂɂ���Ƃ��́A�\�������k������
		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor, 
			0.f,
			DeltaTime,
			30.f);
	}

	// �G�C�����̌v�Z
	if (ShooterCharacter->GetAiming())
	{
		// �G�C�����́A�\�������k������
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

	// �ˌ���0.05s�̊Ԃ̂�true�ɂȂ�
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
