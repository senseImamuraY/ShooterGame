// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/ShooterEnemyAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "./Enemies/ShooterEnemy.h"

UShooterEnemyAnimInstance::UShooterEnemyAnimInstance() :
	Speed(0.f),
	bIsInAir(false),
	bIsAccelerating(false),
	MovementOffsetYaw(0.f),
	LastMovementOffsetYaw(0.f),
	bAiming(false),
	CharacterRotation(FRotator(0.f)),
	CharacterRotationLastFrame(FRotator(0.f)),
	CharacterYawWhenTurningInPlace(0.f),
	CharacterYawLastFrameWhenTurningInPlace(0.f),
	YawDelta(0.f),
	RootYawOffset(0.f),
	Pitch(0.f),
	bReloading(false),
	OffsetState(EEnemyOffsetState::EOS_Hip),
	RecoilWeight(1.0),
	bTurningInPlace(false)
{
}

void UShooterEnemyAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterEnemy == nullptr)
	{
		ShooterEnemy = Cast<AShooterEnemy>(TryGetPawnOwner());
	}
	if (ShooterEnemy)
	{
		FVector Velocity{ ShooterEnemy->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();

		FRotator AimRotation = ShooterEnemy->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterEnemy->GetVelocity());

		// �ړ������ƌ����Ă�������̊p�x�̍�
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		if (ShooterEnemy->GetVelocity().Size() > 0.f)
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}

		bAiming = true;

		if (bReloading)
		{
			OffsetState = EEnemyOffsetState::EOS_Reloading;
		}
		else if (bIsInAir)
		{
			OffsetState = EEnemyOffsetState::EOS_InAir;
		}
		else if (bAiming)
		{
			OffsetState = EEnemyOffsetState::EOS_Aiming;
		}
		else
		{
			OffsetState = EEnemyOffsetState::EOS_Hip;
		}
	}

	if (ShooterEnemy) return;

	TurnInPlace();
	Lean(DeltaTime);
}

void UShooterEnemyAnimInstance::NativeInitializeAnimation()
{
	ShooterEnemy = Cast<AShooterEnemy>(TryGetPawnOwner());
}

void UShooterEnemyAnimInstance::TurnInPlace()
{
	if (ShooterEnemy == nullptr) return;

	Pitch = ShooterEnemy->GetBaseAimRotation().Pitch;

	if (Speed > 0 || bIsInAir)
	{
		// �L�����N�^�[���ړ����Ă���Ƃ��́AOffset��0�ɂ���
		RootYawOffset = 0.f;
		CharacterYawWhenTurningInPlace = ShooterEnemy->GetActorRotation().Yaw;
		CharacterYawLastFrameWhenTurningInPlace = CharacterYawWhenTurningInPlace;
		RotationCurveLastFrame = 0.f;
		RotationCurve = 0.f;
	}
	else
	{
		CharacterYawLastFrameWhenTurningInPlace = CharacterYawWhenTurningInPlace;
		CharacterYawWhenTurningInPlace = ShooterEnemy->GetActorRotation().Yaw;
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

void UShooterEnemyAnimInstance::Lean(float DeltaTime)
{
	if (ShooterEnemy == nullptr) return;
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ShooterEnemy->GetActorRotation();

	// Yaw�̒l���}���ɕς��(-180����180�ɐ؂�ւ��)���ƂɑΏ����邽�߂ɐ��K���𗘗p
	const FRotator Delta{ UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame) };

	const float Target = Delta.Yaw / DeltaTime;
	const float InterpSpeed = 1.f;
	const float InterpMin = -85.f;
	const float InterpMax = 85.f;
	const float Interp{ FMath::FInterpTo(YawDelta, Target, DeltaTime, InterpSpeed) };

	YawDelta = FMath::Clamp(Interp, InterpMin, InterpMax);
}
