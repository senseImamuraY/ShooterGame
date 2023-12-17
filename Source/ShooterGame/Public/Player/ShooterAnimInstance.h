// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "../Weapon/WeaponType.h"
#include "ShooterAnimInstance.generated.h"

class AShooterCharacter;

/**
 * 
 */
UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Aiming UMETA(DisplayName = "Aiming"),
	EOS_Hip UMETA(DisplayName = "Hip"),
	EOS_Reloading UMETA(DisplayName = "Reloading"),
	EOS_InAir UMETA(DisplayName = "InAir"),

	EOS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class SHOOTERGAME_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UShooterAnimInstance();

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
	AShooterCharacter* ShooterCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	bool bIsAccelerating;

	// StartMotion�Ŏg�p���邽�߂�Offset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"));
	float MovementOffsetYaw;

	// StartMotion�Ŏg�p���邽�߂�Offset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"));
	float MovementOffsetRoll;

	// StopMotion���쓮�����邽�߂�Offset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"));
	float LastMovementOffsetYaw;
	
	// StopMotion���쓮�����邽�߂�Offset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"));
	float LastMovementOffsetRoll;

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
	

	// Aim�̃A�j���[�V�����Ŏd�l����roll
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"));
	float RootRollOffset;

	float CharacterRollWhenTurningInPlace;
	
	float CharacterRollLastFrameWhenTurningInPlace;


	// reload����true
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"));
	bool bReloading;

	// �ǂ�Aim Offset���g�p����̂����߂�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"));
	EOffsetState OffsetState;

	// ���݂̃t���[���̃L�����N�^�[��Yaw
	FRotator CharacterRotation;

	// �P�O�̃t���[���̃L�����N�^�[��Yaw
	FRotator CharacterRotationLastFrame;

	// runnig blendspace�Ŏg�p����L�����N�^�[��Yaw delta
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Lean, meta = (AllowPrivateAccess = "true"));
	float YawDelta;	
	
	// runnig blendspace�Ŏg�p����L�����N�^�[��Roll delta
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Lean, meta = (AllowPrivateAccess = "true"));
	float RollDelta;

	// ���Ⴊ��ł���Ȃ�true
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouching, meta = (AllowPrivateAccess = "true"));
	bool bCrouching;

	// �����̑傫����ύX����
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float RecoilWeight;

	// turn in place����true
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	bool bTurningInPlace;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	EWeaponType EquippedWeaponType;

	// reloading����false�ɂȂ�
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	bool bShouldUseFABRIK;
};
