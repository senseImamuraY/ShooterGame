// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ShotGun.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "../Public/Interfaces/BulletHitInterface.h"
#include "../Public/Enemies/Enemy.h"
#include "Particles/ParticleSystemComponent.h"


void AShotGun::BeginPlay()
{
	Super::BeginPlay();

}


AShotGun::AShotGun()
{
	BeamLineLocations.Init(FVector(0.f, 0.f, 0.f), 5);
	CooldownTime = 0.7;
}

void AShotGun::Fire(AShooterCharacter* ShooterCharacter)
{
	SetbIsFiringCooldown(true);

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AWeapon::ResetFiringCooldown, 0.2f, false);

	const USkeletalMeshSocket* BarrelSocket = GetItemMesh()->GetSocketByName("BarrelSocket");
	if (!BarrelSocket) return;

	SocketTransform = BarrelSocket->GetSocketTransform(GetItemMesh());

	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
	}

	TArray<FHitResult> BeamHitResults;
	BeamHitResults.Init(FHitResult(), 5); // �T�C�Y 5 �ŏ��������A�e�v�f�� FHitResult() �ŏ���������

	bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamHitResults, ShooterCharacter);
	if (!bBeamEnd) return;

	for (const FHitResult& BeamHitResult : BeamHitResults) 
	{
		if (!BeamHitResult.GetActor()) continue;
		IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamHitResult.GetActor());

		if (BulletHitInterface)
		{
			BulletHitInterface->BulletHit_Implementation(BeamHitResult, ShooterCharacter, ShooterCharacter->GetController());

			AEnemy* HitEnemy = Cast<AEnemy>(BeamHitResult.GetActor());
			if (!HitEnemy) continue;

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
			if (!ImpactParticles) return;

			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ImpactParticles,
				BeamHitResult.Location);
		}
	}
}


bool AShotGun::GetBeamEndLocation(const FVector& MuzzleSocketLocation, TArray<FHitResult>& OutHitResults, AShooterCharacter* ShooterCharacter)
{
	TArray<FVector> OutBeamLocations;
	OutBeamLocations.Init(FVector(0.f, 0.f, 0.f), 5);
	BeamLineLocations.Init(FVector(0.f, 0.f, 0.f), 5);

	bool bCrosshairHit = TraceUnderCrosshairs(OutHitResults, OutBeamLocations, ShooterCharacter);
	const float RaycastDistance = 25'000.f;

	// �U�e�̃��C�g���[�X�����s
	for (int i = 0; i < 5; i++)
	{
		const FVector WeaponTraceStart{ MuzzleSocketLocation };
		const FVector StartToEnd{ OutBeamLocations[i] - MuzzleSocketLocation };
		FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd.GetSafeNormal() * RaycastDistance};

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(ShooterCharacter);
		Params.AddIgnoredActor(this);

		GetWorld()->LineTraceSingleByChannel(
			//BeamHitResult
			OutHitResults[i],
			WeaponTraceStart,
			WeaponTraceEnd,
			ECollisionChannel::ECC_Visibility,
			Params);

		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			BeamParticles,
			SocketTransform);

		if (Beam)
		{
			Beam->SetVectorParameter(FName("Target"), BeamLineLocations[i]);
		}
	}

	return OutHitResults.Num() > 0;
}

bool AShotGun::TraceUnderCrosshairs(TArray<FHitResult>& OutHitResults, TArray<FVector>& OutHitLocations, AShooterCharacter* ShooterCharacter)
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

	if (bScreenToWorld)
	{
		const float RaycastDistance = 25'000.f;

		const FVector Start{ CrosshairWorldPosition };
		FVector End{ Start + CrosshairWorldDirection * RaycastDistance };

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(ShooterCharacter);
		Params.AddIgnoredActor(this);

		// ���S�̃��C�g���[�X
		PerformTrace(CrosshairWorldPosition, CrosshairWorldDirection, RaycastDistance, Params, OutHitResults[0], 0, OutHitLocations[0]);

		// �X�΂��������ǉ��̃��C�g���[�X
		for (int i = 1; i < 5; i++)
		{
			// 2�񂲂ƂɎ���؂�ւ���
			FVector RotationAxis = (i % 2 == 0) ? ShooterCharacter->GetActorRightVector() : ShooterCharacter->GetActorUpVector();

			// 2�񂲂ƂɊp�x�̕�����؂�ւ���
			float RotationAngle = FMath::DegreesToRadians((i % 4 < 2) ? 120.0f : -120.0f);

			FVector AdjustedDirection = CrosshairWorldDirection.RotateAngleAxis(RotationAngle, RotationAxis);
			AdjustedDirection = AdjustedDirection.RotateAngleAxis(RotationAngle, RotationAxis);
			AdjustedDirection.Normalize();

			PerformTrace(CrosshairWorldPosition, AdjustedDirection, RaycastDistance, Params, OutHitResults[i], i, OutHitLocations[i]);
		}

		return true;
	}

	return false;
}

void AShotGun::PerformTrace(const FVector& StartPosition, const FVector& Direction, float Distance, const FCollisionQueryParams& Params, FHitResult& OutHitResult, int index, FVector& OutHitLocation)
{
	FVector EndPosition = StartPosition + Direction * Distance;

	GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		StartPosition,
		EndPosition,
		ECollisionChannel::ECC_Visibility,
		Params);

	if (OutHitResult.bBlockingHit)
	{
		OutHitLocation = OutHitResult.Location;
		BeamLineLocations[index] = OutHitResult.Location;
	}
	else
	{
		BeamLineLocations[index] = EndPosition;
	}
}


