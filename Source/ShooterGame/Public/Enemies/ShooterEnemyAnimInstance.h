// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterEnemyAnimInstance.generated.h"

class AShooterEnemy;

/**
 *
 */
UENUM(BlueprintType)
enum class EEnemyOffsetState : uint8
{
	EOS_Aiming UMETA(DisplayName = "Aiming"),
	EOS_Hip UMETA(DisplayName = "Hip"),
	EOS_Reloading UMETA(DisplayName = "Reloading"),
	EOS_InAir UMETA(DisplayName = "InAir"),

	EOS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class SHOOTERGAME_API UShooterEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:
	UShooterEnemyAnimInstance();

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;

protected:
	// 止まっているときは、カメラを90度以上動かさないとプレイヤーが回転しないようにする
	void TurnInPlace();

	// 走っているときの傾きを計算する
	void Lean(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	AShooterEnemy* ShooterEnemy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	bool bIsAccelerating;

	// StartMotionで使用するためのOffset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"));
	float MovementOffsetYaw;

	// StopMotionを作動させるためのOffset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"));
	float LastMovementOffsetYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	bool bAiming;

	// 現在のフレームのキャラクターのYaw, 地面に立っているときのみ
	float CharacterYawWhenTurningInPlace;

	// １つ前のフレームのキャラクターのYaw, 地面に立っているときのみ
	float CharacterYawLastFrameWhenTurningInPlace;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"));
	float RootYawOffset;

	// 現在のフレームのRotation curveの値
	float RotationCurve;

	// １つ前のフレームのRotation curveの値
	float RotationCurveLastFrame;

	// Aimのアニメーションで仕様するpitch
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"));
	float Pitch;

	// reload中にtrue
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"));
	bool bReloading;

	// 現在のフレームのキャラクターのYaw
	FRotator CharacterRotation;

	// １つ前のフレームのキャラクターのYaw
	FRotator CharacterRotationLastFrame;

	// runnig blendspaceで使用するキャラクターのYaw delta
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Lean, meta = (AllowPrivateAccess = "true"));
	float YawDelta;

	// しゃがんでいるならtrue
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouching, meta = (AllowPrivateAccess = "true"));
	bool bCrouching;

	// 反動の大きさを変更する
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float RecoilWeight;

	// turn in place時にtrue
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	bool bTurningInPlace;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"));
	EEnemyOffsetState OffsetState;
};
