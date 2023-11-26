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
	
	WeaponType = EWeaponType::EWT_ShotGun;
}


void AShotGun::Fire(AShooterCharacter* ShooterCharacter)
{
	const USkeletalMeshSocket* BarrelSocket = this->GetItemMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(this->GetItemMesh());

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		TArray<FHitResult> BeamHitResults;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamHitResults, ShooterCharacter);

		if (bBeamEnd)
		{
			for (const FHitResult& BeamHitResult : BeamHitResults) 
			{
				if (BeamHitResult.GetActor())
				{
					IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamHitResult.GetActor());

					if (BulletHitInterface)
					{
						BulletHitInterface->BulletHit_Implementation(BeamHitResult, ShooterCharacter, ShooterCharacter->GetController());

						AEnemy* HitEnemy = Cast<AEnemy>(BeamHitResult.GetActor());

						if (HitEnemy)
						{
							float WeaponDamage = this->GetDamage();
							float TotalDamage = WeaponDamage + ShooterCharacter->GetPlayerAttackPower();

							UGameplayStatics::ApplyDamage(
								BeamHitResult.GetActor(),
								TotalDamage,
								ShooterCharacter->GetController(),
								ShooterCharacter,
								UDamageType::StaticClass());
						}
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

				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					BeamParticles,
					SocketTransform);

				if (Beam)
				{
					Beam->SetVectorParameter(FName("Target"), BeamHitResult.Location);
				}
			}
		} 
	}
}

bool AShotGun::GetBeamEndLocation(const FVector& MuzzleSocketLocation, TArray<FHitResult>& OutHitResults, AShooterCharacter* ShooterCharacter)
{
	FVector OutBeamLocation;
	// crosshairのtrace hitをチェック
	TArray<FHitResult> CrosshairBeamHitResults;
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairBeamHitResults, OutBeamLocation, ShooterCharacter);

	const float RaycastDistance = 25'000.f;

	if (CrosshairBeamHitResults.Num() == 0) return false;

	// 散弾のレイトレースを実行
	for (int i = 0; i < 5; i++)
	{
		FVector AdjustedDirection = CrosshairBeamHitResults[i].ImpactPoint - MuzzleSocketLocation;
		AdjustedDirection.Normalize();

		// 傾斜の追加
		FVector RotationAxis;
		float RotationAngle;

		if (i == 0) // 中央
		{
			// 傾斜なし
		}
		else
		{
			if (i % 2 == 0)
			{
				RotationAxis = ShooterCharacter->GetActorUpVector();
			}
			else
			{
				RotationAxis = ShooterCharacter->GetActorRightVector();
			}
			RotationAngle = FMath::DegreesToRadians(10.0f * (i / 2));
			AdjustedDirection = AdjustedDirection.RotateAngleAxis(RotationAngle, RotationAxis);
		}

		const FVector Start{ MuzzleSocketLocation };
		const FVector End{ Start + AdjustedDirection * RaycastDistance };

		FHitResult BeamHitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(ShooterCharacter);
		Params.AddIgnoredActor(this);

		GetWorld()->LineTraceSingleByChannel(
			BeamHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility,
			Params);

		// デバッグラインの描画
		FColor LineColor;
		switch (i)
		{
		case 0: LineColor = FColor::Red; break;
		case 1: LineColor = FColor::Green; break;
		case 2: LineColor = FColor::Blue; break;
		case 3: LineColor = FColor::Yellow; break;
		case 4: LineColor = FColor::Cyan; break;
		default: LineColor = FColor::White; break;
		}
		DrawDebugLine(GetWorld(), Start, End, LineColor, false, 1.0f, 0, 1.0f);


		if (BeamHitResult.bBlockingHit)
		{
			OutHitResults.Add(BeamHitResult);
		}
	}

	return OutHitResults.Num() > 0;
}

bool AShotGun::TraceUnderCrosshairs(TArray<FHitResult>& OutHitResults, FVector& OutHitLocation, AShooterCharacter* ShooterCharacter)
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

	if (bScreenToWorld)
	{
		const float RaycastDistance = 50'000.f;

		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * RaycastDistance };
		OutHitLocation = End;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(ShooterCharacter);
		Params.AddIgnoredActor(this);


		// 中心のレイトレース
		PerformTrace(CrosshairWorldPosition, CrosshairWorldDirection, RaycastDistance, Params, OutHitResults);

		// 傾斜を加えた追加のレイトレース
		for (int i = 0; i < 4; i++)
		{
			FVector RotationAxis = (i % 2 == 0) ? ShooterCharacter->GetActorUpVector() : ShooterCharacter->GetActorRightVector();
			float RotationAngle = FMath::DegreesToRadians(10.0f * (i / 2));
			FVector AdjustedDirection = CrosshairWorldDirection.RotateAngleAxis(RotationAngle, RotationAxis);

			PerformTrace(CrosshairWorldPosition, AdjustedDirection, RaycastDistance, Params, OutHitResults);
		}

		return OutHitResults.Num() > 0;
	}

	return false;
}

void AShotGun::PerformTrace(const FVector& StartPosition, const FVector& Direction, float Distance, const FCollisionQueryParams& Params, TArray<FHitResult>& OutHitResults)
{
	FVector EndPosition = StartPosition + Direction * Distance;
	FHitResult HitResult;

	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartPosition,
		EndPosition,
		ECollisionChannel::ECC_Visibility,
		Params);

	if (HitResult.bBlockingHit)
	{
		OutHitResults.Add(HitResult);
	}
}

