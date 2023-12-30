// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/Enemy.h"
#include "KrakenEnemy.generated.h"

class APawn;


/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AKrakenEnemy : public AEnemy
{
	GENERATED_BODY()

public:
	AKrakenEnemy();

	UFUNCTION(BlueprintCallable)
	virtual void DoDamage(AActor* Victim) override;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Die() override;

	virtual void PlayDeathAnimation() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USoundCue* DeathSound;

	void ResetTimeDilation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float KrakenEnemyExpPoint;

	float RoamAreaRadius;

	bool bIsRoaming;
	void Roam();
	void ResetbIsRoaming();
	FVector Destination;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Chace, meta = (AllowPrivateAccess = "true"))
	APawn* Target;

	bool bIsAttacking;
	void ResetbIsAttacking();

	UFUNCTION(BlueprintCallable)
	void ChacePlayer();

	bool bIsMoving;

	void MoveToPlayerLocation();
	FVector LastSeenPlayerPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float KrakenEnemyAttackPower;

	void BlowAwayPlayer();
};
