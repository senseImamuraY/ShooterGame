// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/Enemy.h"
#include "GhostEnemy.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AGhostEnemy : public AEnemy
{
	GENERATED_BODY()
public:
	AGhostEnemy();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void Die() override;

	virtual void PlayDeathAnimation() override;

	virtual void DoDamage(AActor* Victim) override;

	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	float GhostEnemyAttackPower;

	void ChasePlayer(float TimeDelta);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float GhostEnemyExpPoint;
};
