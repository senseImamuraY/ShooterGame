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
	// crosshairのtrace hitをチェック
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation, ShooterCharacter);

	// Barrelからトレースを行う。Barrelからの軌道を優先して当たり判定を行う。
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBeamLocation - MuzzleSocketLocation };
	// Locationがピッタリの場合、接触しない（桁落ちで衝突判定が不安定になる）可能性があるため、1.25倍する
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

	if (!OutHitResult.bBlockingHit) // barrelとEndpointの間にオブジェクトがあるか
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

	// 照準線の位置を設定
	FVector2D CrosshairLocation(CenterPosition);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// 画面座標からワールド座標へ変換して、Position,Directionに代入
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
