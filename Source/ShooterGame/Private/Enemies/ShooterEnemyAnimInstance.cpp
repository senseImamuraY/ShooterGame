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

		// 移動方向と向いている方向の角度の差
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
		// キャラクターが移動しているときは、Offsetを0にする
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

		// (-180, 180)の間にclampされる
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TurnInPlaceYawDelta);

		// 回転中は1.0、そうでないなら0.0
		// 回転の処理自体はBlueprint側で実装
		// RootYawOffsetが90以上or-90以下ならturningがtrueになる
		const float Turning{ GetCurveValue(TEXT("Turning")) };
		if (Turning > 0)
		{
			bTurningInPlace = true;
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

			// RootYawOffset > 0なら左回転、 RootYawOffset < 0 なら右回転
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

			// 値が急激に変化した場合の調整（例: 180から-160になった場合）
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

	// アニメーションの見栄えを良くするために、反動を調整
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

	// Yawの値が急激に変わる(-180から180に切り替わる)ことに対処するために正規化を利用
	const FRotator Delta{ UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame) };

	const float Target = Delta.Yaw / DeltaTime;
	const float InterpSpeed = 1.f;
	const float InterpMin = -85.f;
	const float InterpMax = 85.f;
	const float Interp{ FMath::FInterpTo(YawDelta, Target, DeltaTime, InterpSpeed) };

	YawDelta = FMath::Clamp(Interp, InterpMin, InterpMax);
}
