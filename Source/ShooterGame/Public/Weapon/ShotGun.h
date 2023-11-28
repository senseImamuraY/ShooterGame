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
	AShotGun();

	virtual void Fire(AShooterCharacter* ShooterCharacter) override;

private:
	virtual void BeginPlay() override;

	virtual bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, TArray<FHitResult>& OutHitResults, AShooterCharacter* ShooterCharacter) override;

	virtual bool TraceUnderCrosshairs(TArray<FHitResult>& OutHitResults, TArray<FVector>& OutHitLocations, AShooterCharacter* ShooterCharacter) override;

	void PerformTrace(const FVector& StartPosition, const FVector& Direction, float Distance, const FCollisionQueryParams& Params, FHitResult& OutHitResult, int index, FVector& OutHitLocation);

	FTransform SocketTransform;

	// Itemのスケルタルメッシュ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ShotGunMesh;

	void SetItemProperties(EItemState State) override;
};
