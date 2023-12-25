// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Items/Item.h"
#include "../Items/AmmoType.h"
#include "../Interfaces/PickupInterface.h"
#include "WeaponType.h"
#include "Engine/DataTable.h"
#include "Weapon.generated.h"

class USoundCue;
class UWidgetComponent;

USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MagazingCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* EquipSound;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* EnemyHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* EnemyHitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* AmmoIcon;
};

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AWeapon : public AItem, public IPickupInterface
{
	GENERATED_BODY()
	
public:
	AWeapon();

	virtual void Tick(float DeltaTime) override;

	virtual void PickupItem(AShooterCharacter* ShooterCharacter) override;

	virtual void Fire(AShooterCharacter* ShooterCharacter);

	virtual void ThrowWeapon();

	virtual void DropWeapon(AShooterCharacter* ShooterCharacter);

	void ResetFiringCooldown();

	void PlayFireSound();

protected:
	void StopFalling();

	virtual void OnConstruction(const FTransform& Transform) override;

	// ���C��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USoundCue* FireSound;	
	
	// �G�Ƀq�b�g�������̉�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USoundCue* EnemyHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* MuzzleFlash;

	// �G�Ƀq�b�g���Ȃ������Ƃ��̏e�e�̃q�b�g�G�t�F�N�g
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* DefaultHitParticles;

	// �G�Ƀq�b�g�����Ƃ��̏e�e�̃q�b�g�G�t�F�N�g
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* EnemyHitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	virtual bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult, AShooterCharacter* ShooterCharacter);
	virtual bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation, AShooterCharacter* ShooterCharacter);

	virtual bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, TArray<FHitResult>& OutHitResults, AShooterCharacter* ShooterCharacter);
	virtual bool TraceUnderCrosshairs(TArray<FHitResult>& OutHitResults, TArray<FVector>& OutHitLocations, AShooterCharacter* ShooterCharacter);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	EWeaponType WeaponType;

	// ����weapon��ammo�̎��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	EAmmoType AmmoType;

	bool bIsFiringCooldown;
	float CooldownTime;

private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;

	// Weapon�ɓ����Ă���c��e��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	int32 Ammo;

	// �ő及���\��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	int32 MagazineCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	FName ReloadMontageSection;

	// reload�A�j���[�V�����̃N���b�v�𓮂����Ă���Ƃ���true
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	bool bMovingClip;

	// clip bone�̖��O���i�[
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	FName ClipBoneName;

	// ����ɗ^����_���[�W
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	float Damage;

	void EquipWeapon(AShooterCharacter* ShooterCharacter);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"));
	UDataTable* WeaponDataTable;

public:
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagazineCapacity() const { return MagazineCapacity; }

	void DecrementAmmo();

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }
	FORCEINLINE FName GetClipBoneName() const { return ClipBoneName; }
	FORCEINLINE float GetDamage() const { return Damage; }

	void ReloadAmmo(int32 Amount);

	FORCEINLINE void SetMovingClip(bool Move) { bMovingClip = Move; }

	bool ClipIsFull();

	FORCEINLINE bool GetbIsFiringCooldown() const { return bIsFiringCooldown; }
	FORCEINLINE void SetbIsFiringCooldown(bool IsCooldown) { bIsFiringCooldown = IsCooldown; }
	FORCEINLINE float GetCooldownTime() { return CooldownTime; }
	FORCEINLINE void SetCooldownTime(float Time) { CooldownTime = Time; }
	FORCEINLINE USoundCue* GetEnemyHitSound() { return EnemyHitSound; }
	FORCEINLINE UParticleSystem* GetEnemyHitParticles() { return EnemyHitParticles; }
};
