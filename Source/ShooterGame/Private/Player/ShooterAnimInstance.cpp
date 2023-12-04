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
	bAiming(false),
	CharacterRotation(FRotator(0.f)),
	CharacterRotationLastFrame(FRotator(0.f)),
	CharacterYawWhenTurningInPlace(0.f),
	CharacterYawLastFrameWhenTurningInPlace(0.f),
	YawDelta(0.f),
	RootYawOffset(0.f),
	Pitch(0.f),
	Roll(0.f),
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
		
		// 移動方向と向いている方向の角度の差
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		if (ShooterCharacter->GetVelocity().Size() > 0.f)
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
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

	//if (ShooterCharacter) return;
	//if (ShooterCharacter && ShooterCharacter->GetIsWallRunning()) return;

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
		UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();

		// キャラクターの視線ベクトルを取得
		FVector NormalizedCharacterToCamera = (Camera->GetComponentLocation() - ShooterCharacter->GetActorLocation()).GetSafeNormal();


		//float DotProduct = FVector::DotProduct(
		//					ShooterCharacter->GetWallRunComponent()->GetHitWallNormal(),
		//					NormalizedCameraToCharacter);
		//GEngine->AddOnScreenDebugMessage(7, 5.f, FColor::Cyan, FString::Printf(TEXT("DotProduct: %f"), DotProduct));

		//float AngleRadians = acosf(DotProduct);
		//GEngine->AddOnScreenDebugMessage(8, 5.f, FColor::Cyan, FString::Printf(TEXT("AngleRadians: %f"), AngleRadians));	
		//float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);
	
		//GEngine->AddOnScreenDebugMessage(5, 5.f, FColor::Cyan, FString::Printf(TEXT("AngleDegrees: %f"), AngleDegrees));
		
		FVector ForwardVector = ShooterCharacter->GetActorForwardVector();
		
		FVector BackwardVector = -ForwardVector;

		//BackwardVector.X = FMath::Abs(BackwardVector.X);
		////BackwardVector.Y = FMath::Abs(BackwardVector.Y);
		//BackwardVector.Z = FMath::Abs(BackwardVector.Z);
		//
		//NormalizedCharacterToCamera.X = FMath::Abs(NormalizedCharacterToCamera.X);
		////NormalizedCharacterToCamera.Y = FMath::Abs(NormalizedCharacterToCamera.Y);
		//NormalizedCharacterToCamera.Z = FMath::Abs(NormalizedCharacterToCamera.Z);

		FVector CrossProduct = FVector::CrossProduct(BackwardVector, NormalizedCharacterToCamera);

		GEngine->AddOnScreenDebugMessage(11, 5.f, FColor::Black, FString::Printf(TEXT("NormalizedCharacterToCamera: %s"), *NormalizedCharacterToCamera.ToString()));
		GEngine->AddOnScreenDebugMessage(12, 5.f, FColor::Black, FString::Printf(TEXT("BackwardVector: %s"), *BackwardVector.ToString()));
		GEngine->AddOnScreenDebugMessage(13, 5.f, FColor::Black, FString::Printf(TEXT("CrossProduct: %s"), *CrossProduct.ToString()));

		float DotProduct;

		//if (CrossProduct.Z > 0)
		//{
		//	DotProduct = FVector::DotProduct(NormalizedCharacterToCamera, BackwardVector);
		//}
		//else
		{
			//DotProduct = FVector::DotProduct(BackwardVector, NormalizedCharacterToCamera);
			DotProduct = FVector::DotProduct(NormalizedCharacterToCamera, BackwardVector);
		}

		//float DotProduct = FVector::DotProduct(BackwardVector, NormalizedCharacterToCamera);
		GEngine->AddOnScreenDebugMessage(7, 5.f, FColor::Black, FString::Printf(TEXT("DotProduct: %f"), DotProduct));


		//float AngleRadians = acosf(DotProduct);
		float AngleRadians = acosf(FMath::Clamp(DotProduct, -1.0f, 1.0f));
		GEngine->AddOnScreenDebugMessage(8, 5.f, FColor::Black, FString::Printf(TEXT("AngleRadians: %f"), AngleRadians));
		//float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);
		
		//GEngine->AddOnScreenDebugMessage(9, 5.f, FColor::Black, FString::Printf(TEXT("AngleDegrees: %f"), AngleDegrees));
		
		float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);
		
		FVector UpVector = ShooterCharacter->GetActorUpVector() + ShooterCharacter->GetActorLocation();
		FVector DownVector = ShooterCharacter->GetActorUpVector() * -1.f + ShooterCharacter->GetActorLocation();

		FVector CameraToUp = UpVector - Camera->GetComponentLocation();
		FVector CameraToDown = DownVector - Camera->GetComponentLocation();

		bool IsDownLonger = CameraToUp.Size() < CameraToDown.Size();

		GEngine->AddOnScreenDebugMessage(21, 5.f, FColor::Red, FString::Printf(TEXT("UpVector: %s"), *UpVector.ToString()));
		GEngine->AddOnScreenDebugMessage(22, 5.f, FColor::Green, FString::Printf(TEXT("DownVector: %s"), *DownVector.ToString()));
		GEngine->AddOnScreenDebugMessage(23, 5.f, FColor::Blue, FString::Printf(TEXT("CameraToUp: %s, Size: %f"), *CameraToUp.ToString(), CameraToUp.Size()));
		GEngine->AddOnScreenDebugMessage(24, 5.f, FColor::Yellow, FString::Printf(TEXT("CameraToDown: %s, Size: %f"), *CameraToDown.ToString(), CameraToDown.Size()));
		GEngine->AddOnScreenDebugMessage(25, 5.f, FColor::Cyan, FString::Printf(TEXT("UpOrDown: %s"), IsDownLonger ? TEXT("True") : TEXT("False")));

		if (IsDownLonger)
		{
			// CameraToUp の方が長い
			GEngine->AddOnScreenDebugMessage(26, 5.f, FColor::Magenta, TEXT("CameraToUp is longer"));
		}
		else
		{
			// CameraToDown の方が長い、または両方同じ長さ
			GEngine->AddOnScreenDebugMessage(26, 5.f, FColor::Magenta, TEXT("CameraToDown is longer or same length"));
		}

		if (IsDownLonger)
		{
			AngleDegrees = AngleDegrees * -1;
		}
		else
		{
			// CameraToDown の方が長い、または両方同じ長さ
		}


		//if (CrossProduct.Z < 0)
		//{
		//	AngleDegrees = -AngleDegrees;
		//}

		GEngine->AddOnScreenDebugMessage(9, 5.f, FColor::Black, FString::Printf(TEXT("AngleDegrees: %f"), AngleDegrees));

		//// 前方ベクトルを水平面に投影（Z成分を0にする）
		//FVector ForwardVectorHorizontal = FVector(ForwardVector.X, ForwardVector.Y, 0.f).GetSafeNormal();

		//// 元のベクトルと投影されたベクトルとの間の角度を計算
		//float PitchAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(ForwardVector.GetSafeNormal(), ForwardVectorHorizontal)));

		//// 上向きか下向きかを判定して角度に符号を付ける
		//if (ForwardVector.Z < 0)
		//{
		//	PitchAngle *= -1;
		//}

		//// 結果をログに出力
		//GEngine->AddOnScreenDebugMessage(4, 50.f, FColor::Cyan, FString::Printf(TEXT("PitchAngle: %f"), PitchAngle));
		Pitch = AngleDegrees;
		//Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;
		//Roll = ShooterCharacter->GetBaseAimRotation().Roll;
	}
	else
	{
		Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;
		//Roll = ShooterCharacter->GetBaseAimRotation().Roll;
	}

	//if (Speed > 0 || bIsInAir)
	if (Speed > 0 || (bIsInAir || ShooterCharacter->GetIsWallRunning()))
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

	// RootYawOffsetの値を画面に表示
	GEngine->AddOnScreenDebugMessage(1, 50.f, FColor::Green, FString::Printf(TEXT("RootYawOffset: %f"), RootYawOffset));

	// bIsInAirの値を画面に表示（true/false）
	FString InAirText = bIsInAir ? TEXT("True") : TEXT("False");
	GEngine->AddOnScreenDebugMessage(2, 50.f, FColor::Red, FString::Printf(TEXT("Is In Air: %s"), *InAirText));

	GEngine->AddOnScreenDebugMessage(3, 50.f, FColor::Cyan, FString::Printf(TEXT("Pitch: %f"), Pitch));


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

	// Yawの値が急激に変わる(-180から180に切り替わる)ことに対処するために正規化を利用
	const FRotator Delta{ UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame) };

	const float Target = Delta.Yaw / DeltaTime;
	const float InterpSpeed = 1.f;
	const float InterpMin = -85.f;
	const float InterpMax = 85.f;
	const float Interp{ FMath::FInterpTo(YawDelta, Target, DeltaTime, InterpSpeed) };

	YawDelta = FMath::Clamp(Interp, InterpMin, InterpMax);
}
