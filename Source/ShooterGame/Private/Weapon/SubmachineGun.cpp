// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/SubmachineGun.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "../Public/Interfaces/BulletHitInterface.h"
#include "../Public/Enemies/Enemy.h"
#include "Particles/ParticleSystemComponent.h"


void ASubmachineGun::BeginPlay()
{
	Super::BeginPlay();

	CooldownTime = 0.0f;
}


void ASubmachineGun::Fire(AShooterCharacter* ShooterCharacter)
{
	const USkeletalMeshSocket* BarrelSocket = this->GetItemMesh()->GetSocketByName("BarrelSocket");
	if (!BarrelSocket) return;

	const FTransform SocketTransform = BarrelSocket->GetSocketTransform(this->GetItemMesh());

	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
	}

	FHitResult BeamHitResult;
	bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamHitResult, ShooterCharacter);

	UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		BeamParticles,
		SocketTransform);

	if (Beam)
	{
		Beam->SetVectorParameter(FName("Target"), BeamHitResult.Location);
	}

	if (!bBeamEnd) return;

	if (BeamHitResult.GetActor())
	{
		IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamHitResult.GetActor());

		if (BulletHitInterface)
		{
			BulletHitInterface->BulletHit_Implementation(BeamHitResult, ShooterCharacter, ShooterCharacter->GetController());

			AEnemy* HitEnemy = Cast<AEnemy>(BeamHitResult.GetActor());
			if (!HitEnemy) return;

			float WeaponDamage = this->GetDamage();
			float TotalDamage = WeaponDamage + ShooterCharacter->GetPlayerAttackPower();

			UGameplayStatics::ApplyDamage(
				BeamHitResult.GetActor(),
				TotalDamage,
				ShooterCharacter->GetController(),
				ShooterCharacter,
				UDamageType::StaticClass());
		}
		else
		{
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					BeamHitResult.Location);
			}
		}
	}
}


bool ASubmachineGun::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult, AShooterCharacter* ShooterCharacter)
{
	FVector OutBeamLocation;
	// crosshair��trace hit���`�F�b�N
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation, ShooterCharacter);

	// Barrel����g���[�X���s���BBarrel����̋O����D�悵�ē����蔻����s���B
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBeamLocation - MuzzleSocketLocation };
	// Location���s�b�^���̏ꍇ�A�ڐG���Ȃ��i�������ŏՓ˔��肪�s����ɂȂ�j�\�������邽�߁A1.25�{����
	const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25 };

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(ShooterCharacter);
	Params.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility,
		Params);

	if (!OutHitResult.bBlockingHit) // barrel��Endpoint�̊ԂɃI�u�W�F�N�g�����邩
	{
		OutHitResult.Location = OutBeamLocation;;
		return false;
	}

	return true;
}

bool ASubmachineGun::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation, AShooterCharacter* ShooterCharacter)
{
	FVector2D ViewportSize;

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CenterPosition = FVector2D(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	// �Ə����̈ʒu��ݒ�
	FVector2D CrosshairLocation(CenterPosition);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// ��ʍ��W���烏�[���h���W�֕ϊ����āAPosition,Direction�ɑ��
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(ShooterCharacter, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (!bScreenToWorld) return false;

	const float RaycastDistance = 50'000.f;
	const FVector Start{ CrosshairWorldPosition };
	const FVector End{ Start + CrosshairWorldDirection * RaycastDistance };
	OutHitLocation = End;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(ShooterCharacter);
	Params.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility,
		Params);

	if (!OutHitResult.bBlockingHit) return false;

	OutHitLocation = OutHitResult.Location;

	return true;
}
