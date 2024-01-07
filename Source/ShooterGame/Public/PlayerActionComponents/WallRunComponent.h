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

	// 初回のレイキャストでは、クオータニオンで回転させたカメラのLocationを考慮したレイキャストができない可能性がある。なぜなら、
	// Camera->AddWorldRotation(CombinedRotation);
	// このコードが実行されても、即座にカメラのRotation・Locationの値が変更されない可能性があるから。（ログを出力して確認した。内部処理でそうなっている可能性がある）
	// そのため、この仕様?による悪影響を回避するためにboolで2回目以降のレイキャスト結果を使用して処理をするよう調整
	bool bFirstRayCast;

public:	
	FORCEINLINE bool GetIsWallRunning() const { return bWallRunning; }
	FORCEINLINE void SetCanWallRun(bool value) { bCanWallRun = value; }
	FORCEINLINE FTimerHandle& GetWallRunTimerHandle() { return WallRunTimerHandle; }
	FORCEINLINE FVector GetHitWallNormal() const { return HitWallNormal; }
};
