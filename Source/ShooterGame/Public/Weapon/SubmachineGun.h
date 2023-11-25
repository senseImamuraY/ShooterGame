// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "SubmachineGun.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API ASubmachineGun : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(AShooterCharacter* ShooterCharacter) override;

private:
	virtual bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult, AShooterCharacter* ShooterCharacter) override;

	virtual bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation, AShooterCharacter* ShooterCharacter) override;

};
