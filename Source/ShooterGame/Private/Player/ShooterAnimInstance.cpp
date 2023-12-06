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
			// 移動方向と向いている方向の角度の差
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
		// 壁走りしている時のPitchの値を求める

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

		// この長さによって正負を判定し、処理をする
		bool bIsDownLonger = CameraToUp.Size() < CameraToDown.Size();

		if (bIsDownLonger)
		{
			float AngleOffset = 20.f; // 見栄えを良くするために数値を調整
			AngleDegrees = AngleDegrees * -1 + AngleOffset;
		}
		else
		{
			float LerpFactor = 0.8f; // 見栄えを良くするために数値を調整
			AngleDegrees = FMath::Lerp(AngleDegrees, AngleDegrees * 2.0f, LerpFactor);
		}

		Pitch = AngleDegrees;

		// 壁走りしている時のRollの値を求める

		if (Speed > 0)
		{
			// キャラクターが移動しているときは、Offsetを0にする
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

			// (-180, 180)の間にclampされる
			RootRollOffset = UKismetMathLibrary::NormalizeAxis(RootRollOffset - TurnInPlaceRollDelta);

			// 回転中は1.0、そうでないなら0.0
			// 回転の処理自体はBlueprint側で実装
			// RootRollOffsetが90以上or-90以下ならturningがtrueになる
			const float Turning{ GetCurveValue(TEXT("Turning")) };
			if (Turning > 0)
			{
				bTurningInPlace = true;
				RotationCurveLastFrame = RotationCurve;
				RotationCurve = GetCurveValue(TEXT("Rotation"));
				const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

				// RootRollOffset > 0なら左回転、 RootYawOffset < 0 なら右回転
				RootRollOffset > 0 ? RootRollOffset -= DeltaRotation : RootRollOffset += DeltaRotation;

				// 値が急激に変化した場合の調整（例: 180から-160になった場合）
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
			// キャラクターが移動しているときは、Offsetを0にする
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

void UShooterAnimInstance::Lean(float DeltaTime)
{
	if (ShooterCharacter == nullptr) return;
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ShooterCharacter->GetActorRotation();

	// Yaw・壁走り中のRollの値が急激に変わる(-180から180に切り替わる)ことに対処するために正規化を利用
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
