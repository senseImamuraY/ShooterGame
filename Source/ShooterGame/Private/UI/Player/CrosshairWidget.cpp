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
	const FVector2D CenterAnchors(0.5f, 0.5f);
	const FVector2D CenterAlignment(0.5f, 0.5f);
	const FVector2D ImageSize(128.f, 128.f);
	const FString BaseTexturePath = TEXT("/Game/ShooterGame/Textures/Crosshairs/Crosshair_Quad_9_");

	// UImage�̐ݒ���s���w���p�[�֐�
	auto SetupCanvasSlot = [&](UCanvasPanelSlot* CanvasSlot, const FVector2D& Position) {
		if (CanvasSlot)
		{
			CanvasSlot->SetAnchors(FAnchors(CenterAnchors.X, CenterAnchors.Y)); // �C��
			CanvasSlot->SetAlignment(CenterAlignment);
			CanvasSlot->SetPosition(Position);
			CanvasSlot->SetSize(ImageSize);
		}
	};

	// UImage��CanvasPanel�̎q�Ƃ��Ēǉ�
	CanvasSlotTop = CanvasPanel->AddChildToCanvas(CrosshairImageTop);
	CanvasSlotBottom = CanvasPanel->AddChildToCanvas(CrosshairImageBottom);
	CanvasSlotLeft = CanvasPanel->AddChildToCanvas(CrosshairImageLeft);
	CanvasSlotRight = CanvasPanel->AddChildToCanvas(CrosshairImageRight);

	// UImage�̈ʒu��ݒ�
	SetupCanvasSlot(CanvasSlotTop, FVector2D(0.f, -50.f));
	SetupCanvasSlot(CanvasSlotBottom, FVector2D(0.f, 50.f));
	SetupCanvasSlot(CanvasSlotLeft, FVector2D(-50.f, 0.f));
	SetupCanvasSlot(CanvasSlotRight, FVector2D(50.f, 0.f));

	// �e�N�X�`���̓ǂݍ��݂Ɛݒ���s���w���p�[�֐�
	auto SetupCrosshairImage = [&](UImage* Image, const FString& Direction) {
		UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *(BaseTexturePath + Direction)));

		if (Texture)
		{
			Image->SetBrushFromTexture(Texture);
		}
	};

	// �e�N�X�`���̓ǂݍ��݂Ɛݒ�
	SetupCrosshairImage(CrosshairImageTop, TEXT("Top.Crosshair_Quad_9_Top"));
	SetupCrosshairImage(CrosshairImageBottom, TEXT("Bottom.Crosshair_Quad_9_Bottom"));
	SetupCrosshairImage(CrosshairImageLeft, TEXT("Left.Crosshair_Quad_9_Left"));
	SetupCrosshairImage(CrosshairImageRight, TEXT("Right.Crosshair_Quad_9_Right"));
}


void UCrosshairWidget::UpdateCrosshairPosition(float Deltatime)
{
	// CalculateCrosshairSpread�֐����Ăяo����CrosshairSpreadMultiplier���X�V
	CalculateCrosshairSpread(Deltatime);

	const FVector2D BaseOffsetTop = FVector2D(0.f, -50.f);
	const FVector2D BaseOffsetBottom = FVector2D(0.f, 50.f);
	const FVector2D BaseOffsetLeft = FVector2D(-50.f, 0.f);
	const FVector2D BaseOffsetRight = FVector2D(50.f, 0.f);

	// �S�Ă�Position��ύX

	// ���݂̈ʒu��CrosshairSpreadMultiplier���|���ĐV�����ʒu���v�Z
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

	// ���x�ɂ��\��������
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
		WalkSpeedRange,
		VelocityMultiplierRange,
		Velocity.Size());

	// �󒆂ɂ���Ƃ��̏\�������v�Z
	CrosshairInAirFactor = FMath::FInterpTo(
		CrosshairInAirFactor,
		ShooterCharacter->GetCharacterMovement()->IsFalling() ? LargeSpread : NoneSpread,
		DeltaTime,
		ShooterCharacter->GetCharacterMovement()->IsFalling() ? InterpSpeedSmall : InterpSpeedMedium);

	// �G�C�����̌v�Z
	CrosshairAimFactor = FMath::FInterpTo(
		CrosshairAimFactor,
		ShooterCharacter->GetAiming() ? MediumSpread : NoneSpread,
		DeltaTime,
		InterpSpeedMedium);

	// �ˌ���̌v�Z
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

