// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/ShooterEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "../Public/Player/ShooterPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "../Public/Core/ScoreSystem/ScoreCounter.h"
#include "../Public/Player/ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "../Public/Weapon/Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Sound/SoundCue.h"
#include "./Enemies/ShooterEnemyAIController.h"
#include "AIController.h"

AShooterEnemy::AShooterEnemy() :
	ShooterEnemyExpPoint(1000.f),
	GhostEnemyAttackPower(10.f)
{
}

void AShooterEnemy::BeginPlay()
{
	Super::BeginPlay();

	EquipWeapon(SpawnDefaultWeapon());

	// AI Controllerクラスを指定
	UClass* EnemyAIControllerClass = AShooterEnemyAIController::StaticClass();
	
	// AI Controllerの生成
	AAIController* NewAIController = GetWorld()->SpawnActor<AAIController>(EnemyAIControllerClass);

	if (NewAIController != nullptr)
	{
		// EnemyにAI Controllerを割り当てる
		NewAIController->Possess(this);
	}
}

void AShooterEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

AWeapon* AShooterEnemy::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}

	return nullptr;
}

void AShooterEnemy::Die()
{
	Super::Die();

	// プレイヤーのアクターを取得
	AActor* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!Player) return;

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(Player);
	if (!ShooterCharacter) return;

	//// プレイヤーが獲得する経験値
	//float ExPoint = 100.f;
	if (ShooterCharacter->GetClass()->ImplementsInterface(UExPointsInterface::StaticClass()))
	{
		IExPointsInterface::Execute_CalculateExPoints(ShooterCharacter, ShooterEnemyExpPoint);
	}

	APlayerController* MyController = GetWorld()->GetFirstPlayerController();
	AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(MyController);

	EquippedWeapon->SetActorHiddenInGame(true);

	UUserWidget* Widget = PlayerController->GetHUDOverlay();
	UWidget* ChildWidget = Widget->GetWidgetFromName(TEXT("BPW_Score"));
	UScoreCounter* ScoreWidget = Cast<UScoreCounter>(ChildWidget);
	ScoreWidget->UpdateScore(ShooterEnemyExpPoint);
}

void AShooterEnemy::Shoot(AActor* Victim)
{
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		FHitResult BeamHitResult;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamHitResult, Victim);

		if (bBeamEnd)
		{
			if (BeamHitResult.GetActor())
			{
				AShooterCharacter* HitCharacter = Cast<AShooterCharacter>(BeamHitResult.GetActor());

				if (HitCharacter)
				{
					float WeaponDamage = EquippedWeapon->GetDamage();
					float TotalDamage = WeaponDamage;

					UGameplayStatics::ApplyDamage(
						BeamHitResult.GetActor(),
						TotalDamage,
						GetController(),
						this,
						UDamageType::StaticClass());

					if (HitPlayerImpactSound)
					{
						UGameplayStatics::PlaySoundAtLocation(this, HitPlayerImpactSound, GetActorLocation());
					}

					if (HitPlayerImpactParticles)
					{
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitPlayerImpactParticles, BeamHitResult.Location, FRotator(0.f), true);
					}
				}
				else
				{
					if (EnemyWeaponImpactParticles)
					{
						UGameplayStatics::SpawnEmitterAtLocation(
							GetWorld(),
							EnemyWeaponImpactParticles,
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

void AShooterEnemy::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		// AreaSphereとCollisonBoxのコリジョン無視
		WeaponToEquip->GetAreaSphere()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		WeaponToEquip->GetCollisionBox()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(
			FName("RightHandSocket"));

		if (HandSocket)
		{
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}

		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

void AShooterEnemy::DoDamage(AActor* Victim)
{
	Shoot(Victim);
}

bool AShooterEnemy::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult, AActor* Victim)
{
	FVector OutBeamLocation;
	// crosshairのtrace hitをチェック
	FHitResult CrosshairHitResult;

	bool bCrosshairHit = TraceFromEnemyGuns(CrosshairHitResult, OutBeamLocation, Victim);

	// Barrelからトレースを行う。Barrelからの軌道を優先して当たり判定を行う。
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBeamLocation - MuzzleSocketLocation };
	// Locationがピッタリの場合、接触しない（桁落ちで衝突判定が不安定になる）可能性があるため、1.25倍する
	const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25 };


	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(EquippedWeapon);


	GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility,
		Params);

	// レイトレースの可視化
	FColor LineColor = OutHitResult.bBlockingHit ? FColor::Red : FColor::Green;
	DrawDebugLine(
		GetWorld(),
		WeaponTraceStart,
		WeaponTraceEnd,
		LineColor,
		false, // 永続的なラインではない
		1.0f, // 表示時間（秒）
		0, // DepthPriority
		1.0f // Thickness
	);

	if (!OutHitResult.bBlockingHit) // barrelとEndpointの間にオブジェクトがあるか
	{
		OutHitResult.Location = OutBeamLocation;;
		return false;
	}

	return true;
}

void AShooterEnemy::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

bool AShooterEnemy::TraceFromEnemyGuns(FHitResult& OutHitResult, FVector& OutHitLocation, AActor* Victim)
{
	// 銃の位置と向きを取得
	FVector GunLocation;
	FRotator GunRotation;
	if (EquippedWeapon) // MyGunは銃の参照
	{
		GunLocation = EquippedWeapon->GetActorLocation();
		GunRotation = EquippedWeapon->GetActorRotation();
		// ログ出力を追加
		UE_LOG(LogTemp, Warning, TEXT("Gun Location: %s"), *GunLocation.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Gun Rotation: %s"), *GunRotation.ToString());

	}
	else
	{
		return false; // 銃がない場合は処理を中断
	}

	GunRotation = GetActorRotation();

	AShooterCharacter* Player = Cast<AShooterCharacter>(Victim);
	FVector AimToCrouchingPlayer = FVector(0.f, 0.f, 0.f);

	FVector Start{ GunLocation };
	FVector End{ Start + GunRotation.Vector() * 50000.f }; // ここでレイトレースの距離を設定

	if (Player && (Player->GetCrouching() || Player->GetIsWallRunning()))
	{
		// プレイヤーの足元の位置を計算
		FVector PlayerFeetLocation = Player->GetActorLocation();
		//FVector PlayerFeetLocation = Player->GetActorLocation() - FVector(0.f, 0.f, Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		AimToCrouchingPlayer = PlayerFeetLocation - GunLocation;

		// 60%の確率で精度を落とす
		if (FMath::RandRange(1, 100) <= 60)
		{
			// ランダムなズレを追加
			float RandomX = FMath::RandRange(-500.f, 500.f);
			float RandomY = FMath::RandRange(-500.f, 500.f);
			FVector RandomOffset(RandomX, RandomY, 0.f);
			End = AimToCrouchingPlayer + Start + RandomOffset;
		}
		else
		{
			End = AimToCrouchingPlayer + Start;
		}
	}


	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(EquippedWeapon);

	// レイトレースの実行
	GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility,
		Params);

	// レイトレースの可視化
	FColor LineColor = OutHitResult.bBlockingHit ? FColor::Red : FColor::Green;
	DrawDebugLine(
		GetWorld(),
		Start,
		End,
		LineColor,
		false, // 永続的なラインではない
		1.0f, // 表示時間（秒）
		0, // DepthPriority
		1.0f // Thickness
	);

	// ヒットした場合の処理
	if (OutHitResult.bBlockingHit)
	{
		OutHitLocation = OutHitResult.Location;

		AActor* HitActor = OutHitResult.GetActor();
		if (HitActor)
		{
			FString ActorName = HitActor->GetName();
			FString ActorClass = HitActor->GetClass()->GetName();

			// ログにヒットしたアクターの情報を出力
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s, Class: %s"), *ActorName, *ActorClass);
		}

		// ヒットしたコンポーネントの情報を取得
		UPrimitiveComponent* HitComponent = OutHitResult.GetComponent();
		if (HitComponent)
		{
			FString ComponentName = HitComponent->GetName();
			FString ComponentClass = HitComponent->GetClass()->GetName();

			// ログにヒットしたコンポーネントの情報を出力
			UE_LOG(LogTemp, Warning, TEXT("Hit Component: %s, Class: %s"), *ComponentName, *ComponentClass);
		}

		// ヒット位置と法線の情報をログに出力
		FVector HitLocation = OutHitResult.Location;
		FVector HitNormal = OutHitResult.Normal;
		UE_LOG(LogTemp, Warning, TEXT("Hit Location: %s, Normal: %s"), *HitLocation.ToString(), *HitNormal.ToString());


		return true;
	}

	return false;
}
