// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "./Item.h"
#include "../Interfaces/PickupInterface.h"
#include "Potion.generated.h"

class UNiagaraComponent;
class USphereComponent;

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API APotion : public AItem, public IPickupInterface
{
	GENERATED_BODY()
public:
	APotion();

	virtual void Tick(float DeltaTime) override;

	virtual void PickupItem(AShooterCharacter* ShooterCharacter) override;
protected:

	virtual void BeginPlay() override;

	virtual void SetItemProperties(EItemState State) override;

	UFUNCTION()
	void PotionSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Potion, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* PotionNiagara;

	// ê⁄êGÇµÇΩç€Ç…ÅAAmmoÇèEÇ§ÇΩÇﬂÇÃsphere
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Potion, meta = (AllowPrivateAccess = "true"))
	USphereComponent* PotionCollisionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Potion, meta = (AllowPrivateAccess = "true"))
	float RecoveryAmount;
};
