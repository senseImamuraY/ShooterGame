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
	// �~�܂��Ă���Ƃ��́A�J������90�x�ȏ㓮�����Ȃ��ƃv���C���[����]���Ȃ��悤�ɂ���
	void TurnInPlace();

	// �����Ă���Ƃ��̌X�����v�Z����
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

	// StartMotion�Ŏg�p���邽�߂�Offset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"));
	float MovementOffsetYaw;

	// StopMotion���쓮�����邽�߂�Offset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"));
	float LastMovementOffsetYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	bool bAiming;

	// ���݂̃t���[���̃L�����N�^�[��Yaw, �n�ʂɗ����Ă���Ƃ��̂�
	float CharacterYawWhenTurningInPlace;

	// �P�O�̃t���[���̃L�����N�^�[��Yaw, �n�ʂɗ����Ă���Ƃ��̂�
	float CharacterYawLastFrameWhenTurningInPlace;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"));
	float RootYawOffset;

	// ���݂̃t���[����Rotation curve�̒l
	float RotationCurve;

	// �P�O�̃t���[����Rotation curve�̒l
	float RotationCurveLastFrame;

	// Aim�̃A�j���[�V�����Ŏd�l����pitch
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"));
	float Pitch;

	// reload����true
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"));
	bool bReloading;

	// ���݂̃t���[���̃L�����N�^�[��Yaw
	FRotator CharacterRotation;

	// �P�O�̃t���[���̃L�����N�^�[��Yaw
	FRotator CharacterRotationLastFrame;

	// runnig blendspace�Ŏg�p����L�����N�^�[��Yaw delta
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Lean, meta = (AllowPrivateAccess = "true"));
	float YawDelta;

	// ���Ⴊ��ł���Ȃ�true
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouching, meta = (AllowPrivateAccess = "true"));
	bool bCrouching;

	// �����̑傫����ύX����
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float RecoilWeight;

	// turn in place����true
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	bool bTurningInPlace;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"));
	EEnemyOffsetState OffsetState;
};