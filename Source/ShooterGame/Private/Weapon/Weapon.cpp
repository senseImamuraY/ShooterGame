// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/Weapon/Weapon.h"
#include "../Public/Player/ShooterCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


AWeapon::AWeapon():
	ThrowWeaponTime(0.7f),
	bFalling(false),
	Ammo(30),
	MagazineCapacity(30),
	WeaponType(EWeaponType::EWT_SubmachineGun),
	AmmoType(EAmmoType::EAT_9mm),
	ReloadMontageSection(FName(TEXT("Reload SMG"))),
	ClipBoneName(TEXT("smg_clip")),
	Damage(20.f)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Keep the Weapon upright
	if (GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		const FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}

	ItemCount = Ammo;
}

void AWeapon::PickupItem(AShooterCharacter* ShooterCharacter)
{
	PlayEquipSound();

	TArray<AItem*>& WeaponInventory = ShooterCharacter->GetWeaponInventory();

	if (WeaponInventory.Num() < ShooterCharacter->GetINVENTORY_CAPACITY())
	{
		SetSlotIndex(WeaponInventory.Num());
		WeaponInventory.Add(this);
		SetItemState(EItemState::EIS_PickedUp);
	}
	else
	{
		if (WeaponInventory.Num() - 1 >= GetSlotIndex())
		{
			WeaponInventory[ShooterCharacter->GetEquippedWeapon()->GetSlotIndex()] = this;
			SetSlotIndex(ShooterCharacter->GetEquippedWeapon()->GetSlotIndex());
		}
		DropWeapon(ShooterCharacter);
		EquipWeapon(ShooterCharacter);
	}

	ShooterCharacter->SetTraceHitItem(nullptr);
	ShooterCharacter->SetTraceHitItemLastFrame(nullptr);
}

void AWeapon::DropWeapon(AShooterCharacter* ShooterCharacter)
{
	AWeapon* EquippedWeapon = ShooterCharacter->GetEquippedWeapon();

	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

void AWeapon::EquipWeapon(AShooterCharacter* ShooterCharacter)
{
	// AreaSphere‚ÆCollisonBox‚ÌƒRƒŠƒWƒ‡ƒ“–³Ž‹
	this->GetAreaSphere()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	this->GetCollisionBox()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	const USkeletalMeshSocket* HandSocket = ShooterCharacter->GetMesh()->GetSocketByName(
		FName("RightHandSocket"));

	if (HandSocket)
	{
		HandSocket->AttachActor(this, ShooterCharacter->GetMesh());
	}

	if (ShooterCharacter->GetEquippedWeapon() == nullptr)
	{
		ShooterCharacter->EquipItemDelegate.Broadcast(-1, this->GetSlotIndex());
	}
	else
	{
		ShooterCharacter->EquipItemDelegate.Broadcast(ShooterCharacter->GetEquippedWeapon()->GetSlotIndex(), this->GetSlotIndex());
	}

	ShooterCharacter->SetEquippedWeapon(this);
	this->SetItemState(EItemState::EIS_Equipped);
}

void AWeapon::ThrowWeapon()
{
	bFalling = true;
	GetWorldTimerManager().SetTimer(
		ThrowWeaponTimer,
		this,
		&AWeapon::StopFalling, 
		ThrowWeaponTime);
}

void AWeapon::DecrementAmmo()
{
	if (Ammo - 1 <= 0)
	{
		Ammo = 0;
	}
	else
	{
		--Ammo;
	}
}

void AWeapon::ReloadAmmo(int32 Amount)
{
	checkf(Ammo + Amount <= MagazineCapacity, TEXT("Attempt to reload with more than magazine capacity"));
	Ammo += Amount;
}

bool AWeapon::ClipIsFull()
{
	return Ammo >= MagazineCapacity;
}

void AWeapon::StopFalling()
{
	bFalling = false;
	SetItemState(EItemState::EIS_Pickup);
}

void AWeapon::OnConstruction(const FTransform& Transform)
{
	const FString WeaponTablePath = TEXT("/Script/Engine.DataTable'/Game/ShooterGame/Blueprints/Weapons/DT_Weapon.DT_Weapon'");
	UDataTable* WeaponTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *WeaponTablePath));

	if (!WeaponTableObject) return;

	FWeaponDataTable* WeaponDataRow = nullptr;

	switch (WeaponType)
	{
		case EWeaponType::EWT_SubmachineGun:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("SubmachineGun"), TEXT(""));
			break;
		case EWeaponType::EWT_ShotGun:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("ShotGun"), TEXT(""));
			break;
	}

	if (WeaponDataRow)
	{
		AmmoType = WeaponDataRow->AmmoType;
		Ammo = WeaponDataRow->WeaponAmmo;
		MagazineCapacity = WeaponDataRow->MagazingCapacity;
		SetPickupSound(WeaponDataRow->PickupSound);
		SetEquipSound(WeaponDataRow->EquipSound);
		GetItemMesh()->SetSkeletalMesh(WeaponDataRow->ItemMesh);
		SetItemName(WeaponDataRow->ItemName);
		SetAmmoIcon(WeaponDataRow->AmmoIcon);
		EnemyHitParticles = WeaponDataRow->EnemyHitParticles;
		EnemyHitSound = WeaponDataRow->EnemyHitSound;
	}
}

void AWeapon::Fire(AShooterCharacter* ShooterCharacter)
{
}

void AWeapon::PlayFireSound()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
}

bool AWeapon::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult, AShooterCharacter* ShooterCharacter)
{
	return false;
}

bool AWeapon::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation, AShooterCharacter* ShooterCharacter)
{
	return false;
}

bool AWeapon::GetBeamEndLocation(const FVector& MuzzleSocketLocation, TArray<FHitResult>& OutHitResults, AShooterCharacter* ShooterCharacter)
{
	return false;
}

bool AWeapon::TraceUnderCrosshairs(TArray<FHitResult>& OutHitResults, TArray<FVector>& OutHitLocations, AShooterCharacter* ShooterCharacter)
{
	return false;
}

void AWeapon::ResetFiringCooldown()
{
	bIsFiringCooldown = false;
}

