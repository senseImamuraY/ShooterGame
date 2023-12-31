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

	// 初回のレイトレースのみ接触判定されるバグを回避するために使用
	// 原因が分かっていないので可能ならば調査してリファクタリングを推奨
	bool bFirstRayCast;

public:	
	FORCEINLINE bool GetIsWallRunning() const { return bWallRunning; }
	FORCEINLINE void SetCanWallRun(bool value) { bCanWallRun = value; }
	FORCEINLINE FTimerHandle& GetWallRunTimerHandle() { return WallRunTimerHandle; }
	FORCEINLINE FVector GetHitWallNormal() const { return HitWallNormal; }
};
