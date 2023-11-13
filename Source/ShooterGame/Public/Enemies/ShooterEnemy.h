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

protected:
	virtual void Die() override;

	virtual void DoDamage(AActor* Victim) override;

	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	float GhostEnemyAttackPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ShooterEnemyExpPoint;

};
