// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Items/Item.h"
#include "../Items/AmmoType.h"
#include "../Interfaces/PickupInterface.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SubmachineGun UMETA(DisplayName = "SubmachineGun"),
	EWT_AssaultRifle UMETA(DisplayName = "AssaultRifle"),

	EWT_MAX UMETA(DisplayName = "DefaultMax")
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

protected:
	void StopFalling();
private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;

	// Weaponに入っている残り弾数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	int32 Ammo;

	// 最大所持可能量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	int32 MagazineCapacity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	EWeaponType WeaponType;

	// このweaponのammoの種類
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	FName ReloadMontageSection;

	// reloadアニメーションのクリップを動かしているときにtrue
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	bool bMovingClip;

	// clip boneの名前を格納
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	FName ClipBoneName;

	// 相手に与えるダメージ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"));
	float Damage;

	void DropWeapon(AShooterCharacter* ShooterCharacter);

	void EquipWeapon(AShooterCharacter* ShooterCharacter);

public:
	void ThrowWeapon();

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
};
