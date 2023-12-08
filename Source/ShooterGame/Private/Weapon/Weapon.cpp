// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/Weapon/Weapon.h"
#include "../Public/Player/ShooterCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"


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
	DropWeapon(ShooterCharacter);
	EquipWeapon(ShooterCharacter);
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
		ShooterCharacter->SetEquippedWeapon(this);
		this->SetItemState(EItemState::EIS_Equipped);
	}
}

void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
	GetItemMesh()->SetWorldRotation(
		MeshRotation,
		false,
		nullptr,
		ETeleportType::TeleportPhysics);

	const FVector MeshForward{ GetItemMesh()->GetForwardVector() };
	const FVector MeshRight{ GetItemMesh()->GetRightVector() };

	// Weapon‚ð“Š‚°‚é•ûŒü
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward);

	float RandomRotation{ FMath::FRandRange(-40.f, 40.f)};
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation,FVector(0.f, 0.f, 1.f));
	ImpulseDirection *= 20'000.f;
	GetItemMesh()->AddImpulse(ImpulseDirection);

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

void AWeapon::Fire(AShooterCharacter* ShooterCharacter)
{
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

