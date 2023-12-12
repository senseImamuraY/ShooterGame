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


void AShotGun::Fire(AShooterCharacter* ShooterCharacter)
{
	const USkeletalMeshSocket* BarrelSocket = GetItemMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		SocketTransform = BarrelSocket->GetSocketTransform(GetItemMesh());

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		TArray<FHitResult> BeamHitResults;
		BeamHitResults.Init(FHitResult(), 5); // サイズ 5 で初期化し、各要素を FHitResult() で初期化する

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
	TArray<FVector> OutBeamLocations;
	OutBeamLocations.Init(FVector(0.f, 0.f, 0.f), 5);
	// crosshairのtrace hitをチェック
	TArray<FHitResult> CrosshairBeamHitResults;
	CrosshairBeamHitResults.Init(FHitResult(), 5);

	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairBeamHitResults, OutBeamLocations, ShooterCharacter);
	
	const float RaycastDistance = 25'000.f;

	// 散弾のレイトレースを実行
	for (int i = 0; i < 5; i++)
	{
		FVector BaseDirection = SocketTransform.GetRotation().GetForwardVector(); // 銃身の向きを基準にする
		FVector AdjustedDirection; // 銃身の向きを使用

		// 傾斜の追加
		FVector RotationAxis;
		float RotationAngle;

		const FVector WeaponTraceStart{ MuzzleSocketLocation };
		const FVector StartToEnd{ OutBeamLocations[i] - MuzzleSocketLocation };

		FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd };

		if (i == 0) // 中央
		{
			// 傾斜なし
		}
		else
		{
			RotationAxis = (i % 2 == 0) ? ShooterCharacter->GetActorRightVector() : ShooterCharacter->GetActorUpVector();

			// 2回ごとに角度の符号を切り替える
			RotationAngle = FMath::DegreesToRadians((i % 4 < 2) ? 90.0f : -90.0f);
			AdjustedDirection = StartToEnd.RotateAngleAxis(RotationAngle, RotationAxis);

			AdjustedDirection.Normalize();
		}

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

		// デバッグラインの描画
		FColor LineColor = FColor::Red;

		DrawDebugLine(GetWorld(), WeaponTraceStart, WeaponTraceEnd, LineColor, false, 30.0f, 0, 1.0f);


		if (OutHitResults[i].bBlockingHit)
		{
			OutHitResults[i].Location = OutBeamLocations[i];
			//OutHitResults.Add(BeamHitResult);
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
		const float RaycastDistance = 25'000.f;
		//const float RaycastDistance = 50'000.f;

		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * RaycastDistance };
		//OutHitLocations[i] = End;

		for (int i = 0; i < 5; i++) OutHitLocations[i] = End;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(ShooterCharacter);
		Params.AddIgnoredActor(this);

		// 中心のレイトレース
		PerformTrace(CrosshairWorldPosition, CrosshairWorldDirection, RaycastDistance, Params, OutHitResults[0], 0, OutHitLocations[0]);
		FColor LineColor = FColor::Green;
		DrawDebugLine(GetWorld(), Start, End, LineColor, false, 30.0f, 0, 1.0f);

		// 傾斜を加えた追加のレイトレース
		for (int i = 1; i < 5; i++)
		{
			// 2回ごとに軸を切り替える
			FVector RotationAxis = (i % 2 == 0) ? ShooterCharacter->GetActorRightVector() : ShooterCharacter->GetActorUpVector();

			// 2回ごとに角度の符号を切り替える
			float RotationAngle = FMath::DegreesToRadians((i % 4 < 2) ? 120.0f : -120.0f);

			FVector AdjustedDirection = CrosshairWorldDirection.RotateAngleAxis(RotationAngle, RotationAxis);

			AdjustedDirection = AdjustedDirection.RotateAngleAxis(RotationAngle, RotationAxis);
			AdjustedDirection.Normalize();


			PerformTrace(CrosshairWorldPosition, AdjustedDirection, RaycastDistance, Params, OutHitResults[i], i, OutHitLocations[i]);

			FVector EndPosition = Start + AdjustedDirection * RaycastDistance;

			DrawDebugLine(GetWorld(), Start, EndPosition, LineColor, false, 30.0f, 0, 1.0f);

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
		//OutHitResults.Add(HitResult);
		OutHitLocation = OutHitResult.Location;
	}
}


