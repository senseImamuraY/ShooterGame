// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/Player/ShooterAnimInstance.h"
#include "../Public/Player/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "../Public/Weapon/Weapon.h"
#include "Camera/CameraComponent.h"


UShooterAnimInstance::UShooterAnimInstance() :
	Speed(0.f),
	bIsInAir(false),
	bIsAccelerating(false),
	MovementOffsetYaw(0.f),
	LastMovementOffsetYaw(0.f),
	LastMovementOffsetRoll(0.f),
	bAiming(false),
	CharacterRotation(FRotator(0.f)),
	CharacterRotationLastFrame(FRotator(0.f)),
	CharacterYawWhenTurningInPlace(0.f),
	CharacterYawLastFrameWhenTurningInPlace(0.f),
	YawDelta(0.f),
	RollDelta(0.f),
	RootYawOffset(0.f),
	Pitch(0.f),
	CharacterRollLastFrameWhenTurningInPlace(0.f),
	CharacterRollWhenTurningInPlace(0.f),
	RootRollOffset(0.f),
	bReloading(false),
	OffsetState(EOffsetState::EOS_Hip),
	RecoilWeight(1.0),
	bTurningInPlace(false)
{
}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	if (ShooterCharacter)
	{
		bCrouching = ShooterCharacter->GetCrouching();
		bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;

		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();

		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
		
		if (ShooterCharacter->GetIsWallRunning())
		{
			MovementRotation = UKismetMathLibrary::MakeRotFromZ(ShooterCharacter->GetVelocity());
			MovementOffsetRoll = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Roll;
		}
		else
		{
			// �ړ������ƌ����Ă�������̊p�x�̍�
			MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
		}

		if (ShooterCharacter->GetIsWallRunning())
		{
			if (ShooterCharacter->GetVelocity().Size() > 0.f)
			{
				LastMovementOffsetRoll = MovementOffsetRoll;
			}
		}
		else
		{
			if (ShooterCharacter->GetVelocity().Size() > 0.f)
			{
				LastMovementOffsetYaw = MovementOffsetYaw;
			}
		}

		bAiming = ShooterCharacter->GetAiming();

		if (bReloading)
		{
			OffsetState = EOffsetState::EOS_Reloading;
		}
		else if (bIsInAir)
		{
			OffsetState = EOffsetState::EOS_InAir;
		}
		else if (ShooterCharacter->GetAiming())
		{
			OffsetState = EOffsetState::EOS_Aiming;
		}
		else
		{
			OffsetState = EOffsetState::EOS_Hip;
		}

		if (ShooterCharacter->GetEquippedWeapon())
		{
			EquippedWeaponType = ShooterCharacter->GetEquippedWeapon()->GetWeaponType();
		}
	}

	TurnInPlace();
	Lean(DeltaTime);
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr) return;

	if (ShooterCharacter->GetIsWallRunning())
	{
		// �Ǒ��肵�Ă��鎞��Pitch�̒l�����߂�

		UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();

		FVector NormalizedCharacterToCamera = (Camera->GetComponentLocation() - ShooterCharacter->GetActorLocation()).GetSafeNormal();
		FVector ForwardVector = ShooterCharacter->GetActorForwardVector();
		FVector BackwardVector = -ForwardVector;

		float PitchDotProduct;
		PitchDotProduct = FVector::DotProduct(NormalizedCharacterToCamera, BackwardVector);
		
		float AngleRadians = acosf(FMath::Clamp(PitchDotProduct, -1.0f, 1.0f));
		float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);
		
		FVector UpVector = ShooterCharacter->GetActorUpVector() + ShooterCharacter->GetActorLocation();
		FVector DownVector = ShooterCharacter->GetActorUpVector() * -1.0f + ShooterCharacter->GetActorLocation();
		FVector CameraToUp = UpVector - Camera->GetComponentLocation();
		FVector CameraToDown = DownVector - Camera->GetComponentLocation();

		// ���̒����ɂ���Đ����𔻒肵�A����������
		bool bIsDownLonger = CameraToUp.Size() < CameraToDown.Size();

		if (bIsDownLonger)
		{
			float AngleOffset = 20.f; // ���h����ǂ����邽�߂ɐ��l�𒲐�
			AngleDegrees = AngleDegrees * -1 + AngleOffset;
		}
		else
		{
			float LerpFactor = 0.8f; // ���h����ǂ����邽�߂ɐ��l�𒲐�
			AngleDegrees = FMath::Lerp(AngleDegrees, AngleDegrees * 2.0f, LerpFactor);
		}

		Pitch = AngleDegrees;

		// �Ǒ��肵�Ă��鎞��Roll�̒l�����߂�

		if (Speed > 0)
		{
			// �L�����N�^�[���ړ����Ă���Ƃ��́AOffset��0�ɂ���
			RootRollOffset = 0.f;
			CharacterRollWhenTurningInPlace = ShooterCharacter->GetBaseAimRotation().Roll;
			CharacterRollLastFrameWhenTurningInPlace = CharacterRollWhenTurningInPlace;
			RotationCurveLastFrame = 0.f;
			RotationCurve = 0.f;
		}
		else
		{
			CharacterRollLastFrameWhenTurningInPlace = CharacterRollWhenTurningInPlace;
			CharacterRollWhenTurningInPlace = ShooterCharacter->GetBaseAimRotation().Roll;
			const float TurnInPlaceRollDelta{ CharacterRollWhenTurningInPlace - CharacterRollLastFrameWhenTurningInPlace };

			// (-180, 180)�̊Ԃ�clamp�����
			RootRollOffset = UKismetMathLibrary::NormalizeAxis(RootRollOffset - TurnInPlaceRollDelta);

			// ��]����1.0�A�����łȂ��Ȃ�0.0
			// ��]�̏������̂�Blueprint���Ŏ���
			// RootRollOffset��90�ȏ�or-90�ȉ��Ȃ�turning��true�ɂȂ�
			const float Turning{ GetCurveValue(TEXT("Turning")) };
			if (Turning > 0)
			{
				bTurningInPlace = true;
				RotationCurveLastFrame = RotationCurve;
				RotationCurve = GetCurveValue(TEXT("Rotation"));
				const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

				// RootRollOffset > 0�Ȃ獶��]�A RootYawOffset < 0 �Ȃ�E��]
				RootRollOffset > 0 ? RootRollOffset -= DeltaRotation : RootRollOffset += DeltaRotation;

				// �l���}���ɕω������ꍇ�̒����i��: 180����-160�ɂȂ����ꍇ�j
				const float ABSRootRollOffset{ FMath::Abs(RootRollOffset) };
				if (ABSRootRollOffset > 90.f)
				{
					const float RollExcess{ ABSRootRollOffset - 90.f };
					RootRollOffset > 0 ? RootRollOffset -= RollExcess : RootRollOffset += RollExcess;
				}
			}
			else
			{
				bTurningInPlace = false;
			}
		}
	}
	else
	{
		Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;

		if (Speed > 0 || bIsInAir)
		{
			// �L�����N�^�[���ړ����Ă���Ƃ��́AOffset��0�ɂ���
			RootYawOffset = 0.f;
			CharacterYawWhenTurningInPlace = ShooterCharacter->GetActorRotation().Yaw;
			CharacterYawLastFrameWhenTurningInPlace = CharacterYawWhenTurningInPlace;
			RotationCurveLastFrame = 0.f;
			RotationCurve = 0.f;
		}
		else
		{
			CharacterYawLastFrameWhenTurningInPlace = CharacterYawWhenTurningInPlace;
			CharacterYawWhenTurningInPlace = ShooterCharacter->GetActorRotation().Yaw;
			const float TurnInPlaceYawDelta{ CharacterYawWhenTurningInPlace - CharacterYawLastFrameWhenTurningInPlace };


			// (-180, 180)�̊Ԃ�clamp�����
			RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TurnInPlaceYawDelta);

			// ��]����1.0�A�����łȂ��Ȃ�0.0
			// ��]�̏������̂�Blueprint���Ŏ���
			// RootYawOffset��90�ȏ�or-90�ȉ��Ȃ�turning��true�ɂȂ�
			const float Turning{ GetCurveValue(TEXT("Turning")) };
			if (Turning > 0)
			{
				bTurningInPlace = true;
				RotationCurveLastFrame = RotationCurve;
				RotationCurve = GetCurveValue(TEXT("Rotation"));
				const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

				// RootYawOffset > 0�Ȃ獶��]�A RootYawOffset < 0 �Ȃ�E��]
				RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

				// �l���}���ɕω������ꍇ�̒����i��: 180����-160�ɂȂ����ꍇ�j
				const float ABSRootYawOffset{ FMath::Abs(RootYawOffset) };
				if (ABSRootYawOffset > 90.f)
				{
					const float YawExcess{ ABSRootYawOffset - 90.f };
					RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
				}
			}
			else
			{
				bTurningInPlace = false;
			}
		}
	}

	// �A�j���[�V�����̌��h����ǂ����邽�߂ɁA�����𒲐�
	const float FullRecoil = 1.f;
	const float MediumRecoil = 0.5f;
	const float LightRecoil = 0.1f;
	const float NoRecoil = 0.f;

	if (bTurningInPlace)
	{
		RecoilWeight = bReloading ? FullRecoil : NoRecoil;
	}
	else
	{
		if (bCrouching)
		{
			RecoilWeight = bReloading ? FullRecoil : LightRecoil;
		}
		else
		{
			RecoilWeight = (bAiming || bReloading) ? FullRecoil : MediumRecoil;
		}
	}
}

void UShooterAnimInstance::Lean(float DeltaTime)
{
	if (ShooterCharacter == nullptr) return;
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ShooterCharacter->GetActorRotation();

	// Yaw�E�Ǒ��蒆��Roll�̒l���}���ɕς��(-180����180�ɐ؂�ւ��)���ƂɑΏ����邽�߂ɐ��K���𗘗p
	const FRotator Delta{ UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame) };

	if (ShooterCharacter->GetIsWallRunning())
	{
		const float Target = Delta.Roll / DeltaTime;
		const float InterpSpeed = 1.f;
		const float InterpMin = -85.f;
		const float InterpMax = 85.f;
		const float Interp{ FMath::FInterpTo(RollDelta, Target, DeltaTime, InterpSpeed) };

		RollDelta = FMath::Clamp(Interp, InterpMin, InterpMax);
	}
	else
	{
		const float Target = Delta.Yaw / DeltaTime;
		const float InterpSpeed = 1.f;
		const float InterpMin = -85.f;
		const float InterpMax = 85.f;
		const float Interp{ FMath::FInterpTo(YawDelta, Target, DeltaTime, InterpSpeed) };

		YawDelta = FMath::Clamp(Interp, InterpMin, InterpMax);
	}
}
