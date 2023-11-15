// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/Enemy.h"
#include "ShooterEnemy.generated.h"

class UInputComponent;
class AWeapon;
class AAmmo;
class USpringArmComponent;
class UCameraComponent;
class USoundCue;
class UParticleSystem;
class UAnimMontage;
class AItem;

/**
 * 
 */


UCLASS()
class SHOOTERGAME_API AShooterEnemy : public AEnemy
{
	GENERATED_BODY()
public:
	AShooterEnemy();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	virtual void DoDamage(AActor* Victim) override;
	
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult, AActor* Victim);

protected:
	virtual void Die() override;

	void Shoot(AActor* Victim);

	void EquipWeapon(AWeapon* WeaponToEquip);

	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	// 現在装備しているWeapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	// 銃を撃ったときにランダムに音声を流す
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* MuzzleFlash;

	// 銃弾のヒットエフェクトs
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* EnemyWeaponImpactParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	bool TraceFromEnemyGuns(FHitResult& OutHitResult, FVector& OutHitLocation, AActor* Victim);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

private:
	float GhostEnemyAttackPower;

	AWeapon* SpawnDefaultWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ShooterEnemyExpPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* HitPlayerImpactParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USoundCue* HitPlayerImpactSound;

};
