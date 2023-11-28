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


AShotGun::AShotGun()
{
	ShotGunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShotGunMesh"));
	ShotGunMesh->SetupAttachment(Super::GetItemMesh());
}

void AShotGun::Fire(AShooterCharacter* ShooterCharacter)
{
	const USkeletalMeshSocket* BarrelSocket = ShotGunMesh->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		SocketTransform = BarrelSocket->GetSocketTransform(ShotGunMesh);
		//const FTransform SocketTransform = BarrelSocket->GetSocketTransform(ShotGunMesh);

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
	
	//for (int i = 0; i < 5; i++) 
	//{
	//	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairBeamHitResults[i], OutBeamLocation[i], ShooterCharacter);
	//}
	//bool bCrosshairHit = TraceUnderCrosshairs(CrosshairBeamHitResults, OutBeamLocation[i], ShooterCharacter);

	const float RaycastDistance = 25'000.f;
	//const float RaycastDistance = 25'000.f;

	//if (CrosshairBeamHitResults.Num() == 0) return false;

	// 散弾のレイトレースを実行
	for (int i = 0; i < 5; i++)
	{
		//FVector AdjustedDirection = OutHitResults[i].ImpactPoint - MuzzleSocketLocation;
		//AdjustedDirection.Normalize();
		FVector BaseDirection = SocketTransform.GetRotation().GetForwardVector(); // 銃身の向きを基準にする
		FVector AdjustedDirection; // 銃身の向きを使用
		//FVector AdjustedDirection = BaseDirection; // 銃身の向きを使用

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
			//if (i % 2 == 0)
			//{
			//	RotationAxis = ShooterCharacter->GetActorUpVector();
			//}
			//else
			//{
			//	RotationAxis = ShooterCharacter->GetActorRightVector();
			//}
			//RotationAngle = FMath::DegreesToRadians(90.0f * i);
			////RotationAngle = FMath::DegreesToRadians(50.0f * (i / 2));
			//AdjustedDirection = StartToEnd.RotateAngleAxis(RotationAngle, RotationAxis);
			//AdjustedDirection.Normalize();
			RotationAxis = (i % 2 == 0) ? ShooterCharacter->GetActorRightVector() : ShooterCharacter->GetActorUpVector();

			// 2回ごとに角度の符号を切り替える
			RotationAngle = FMath::DegreesToRadians((i % 4 < 2) ? 90.0f : -90.0f);
			AdjustedDirection = StartToEnd.RotateAngleAxis(RotationAngle, RotationAxis);

			//AdjustedDirection = AdjustedDirection.RotateAngleAxis(RotationAngle, RotationAxis);
			AdjustedDirection.Normalize();

			//WeaponTraceEnd = MuzzleSocketLocation + AdjustedDirection * RaycastDistance;
		}

		//FVector WeaponTraceEnd{ MuzzleSocketLocation + AdjustedDirection * RaycastDistance };
		//const FVector Start{ MuzzleSocketLocation };
		//const FVector End{ OutBeamLocations[i] - MuzzleSocketLocation + AdjustedDirection * RaycastDistance};
		//const FVector End{ Start + AdjustedDirection * RaycastDistance };

		//FHitResult BeamHitResult;
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
			////FVector RotationAxis = ShooterCharacter->GetActorUpVector();
			//FVector RotationAxis = (i % 2 == 0) ? ShooterCharacter->GetActorUpVector() : ShooterCharacter->GetActorRightVector();
			//float RotationAngle = FMath::DegreesToRadians(90.0f * i);
			////float RotationAngle = FMath::DegreesToRadians(50.0f * (i / 2));
					// 2回ごとに軸を切り替える
			FVector RotationAxis = (i % 2 == 0) ? ShooterCharacter->GetActorRightVector() : ShooterCharacter->GetActorUpVector();

			// 2回ごとに角度の符号を切り替える
			float RotationAngle = FMath::DegreesToRadians((i % 4 < 2) ? 120.0f : -120.0f);

			FVector AdjustedDirection = CrosshairWorldDirection.RotateAngleAxis(RotationAngle, RotationAxis);

			AdjustedDirection = AdjustedDirection.RotateAngleAxis(RotationAngle, RotationAxis);
			AdjustedDirection.Normalize();


			PerformTrace(CrosshairWorldPosition, AdjustedDirection, RaycastDistance, Params, OutHitResults[i], i, OutHitLocations[i]);

			// デバッグラインの描画
			//FColor LineColor = FColor::Green;

			FVector EndPosition = Start + AdjustedDirection * RaycastDistance;

			DrawDebugLine(GetWorld(), Start, EndPosition, LineColor, false, 30.0f, 0, 1.0f);

		}

		//return OutHitResults.Num() > 0;
		return true;
	}

	return false;
}

void AShotGun::PerformTrace(const FVector& StartPosition, const FVector& Direction, float Distance, const FCollisionQueryParams& Params, FHitResult& OutHitResult, int index, FVector& OutHitLocation)
{
	FVector EndPosition = StartPosition + Direction * Distance;
	//FHitResult HitResult;

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

void AShotGun::SetItemProperties(EItemState State)
{
	Super::SetItemProperties(State);

	switch (State)
	{
	case EItemState::EIS_Pickup:
		// Mesh AreaSphere CollisionBoxのプロパティを設定
		ShotGunMesh->SetSimulatePhysics(false);
		ShotGunMesh->SetEnableGravity(false);
		ShotGunMesh->SetVisibility(true);
		ShotGunMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ShotGunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_Equipped:
		ShotGunMesh->SetSimulatePhysics(false);
		ShotGunMesh->SetEnableGravity(false);
		ShotGunMesh->SetVisibility(true);
		ShotGunMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ShotGunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_Falling:
		ShotGunMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ShotGunMesh->SetSimulatePhysics(true);
		ShotGunMesh->SetEnableGravity(true);
		ShotGunMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ShotGunMesh->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_WorldStatic,
			ECollisionResponse::ECR_Block);
		break;
	case EItemState::EIS_EquipInterping:
		ShotGunMesh->SetSimulatePhysics(false);
		ShotGunMesh->SetEnableGravity(false);
		ShotGunMesh->SetVisibility(true);
		ShotGunMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ShotGunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

