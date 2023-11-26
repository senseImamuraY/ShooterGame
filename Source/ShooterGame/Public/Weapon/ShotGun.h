// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "ShotGun.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShotGun : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(AShooterCharacter* ShooterCharacter) override;

private:
	virtual bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, TArray<FHitResult>& OutHitResults, AShooterCharacter* ShooterCharacter) override;

	virtual bool TraceUnderCrosshairs(TArray<FHitResult>& OutHitResults, FVector& OutHitLocation, AShooterCharacter* ShooterCharacter) override;

	void PerformTrace(const FVector& StartPosition, const FVector& Direction, float Distance, const FCollisionQueryParams& Params, TArray<FHitResult>& OutHitResults);

	virtual void BeginPlay() override;
};
