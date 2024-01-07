// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WallRunComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTERGAME_API UWallRunComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWallRunComponent();

	void WallRun();

	void EnableWallRun();

	float MouseXValue;
	float MouseYValue;

private:
	bool bWallRunning;
	bool bCanWallRun;
	FTimerHandle WallRunTimerHandle;

	float PreviousYaw;

	FVector InitialCameraLocation;
	FRotator InitialCameraRotation;

	FVector HitWallNormal;

	void BeginPlay() override;

	float InitialCameraDistance;

	// ����̃��C�L���X�g�ł́A�N�I�[�^�j�I���ŉ�]�������J������Location���l���������C�L���X�g���ł��Ȃ��\��������B�Ȃ��Ȃ�A
	// Camera->AddWorldRotation(CombinedRotation);
	// ���̃R�[�h�����s����Ă��A�����ɃJ������Rotation�ELocation�̒l���ύX����Ȃ��\�������邩��B�i���O���o�͂��Ċm�F�����B���������ł����Ȃ��Ă���\��������j
	// ���̂��߁A���̎d�l?�ɂ�鈫�e����������邽�߂�bool��2��ڈȍ~�̃��C�L���X�g���ʂ��g�p���ď���������悤����
	bool bFirstRayCast;

public:	
	FORCEINLINE bool GetIsWallRunning() const { return bWallRunning; }
	FORCEINLINE void SetCanWallRun(bool value) { bCanWallRun = value; }
	FORCEINLINE FTimerHandle& GetWallRunTimerHandle() { return WallRunTimerHandle; }
	FORCEINLINE FVector GetHitWallNormal() const { return HitWallNormal; }
};
