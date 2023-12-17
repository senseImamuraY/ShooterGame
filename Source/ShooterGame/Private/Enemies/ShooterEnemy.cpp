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

	EquippedWeapon->SetActorHiddenInGame(true);

	// EnemyにAI Controllerを割り当てる
	//C++側からShooterEnemyをインスタンス化する場合、明示的にこの操作を行う必要がある
	UClass* EnemyAIControllerClass = AShooterEnemyAIController::StaticClass();
	
	AAIController* NewAIController = GetWorld()->SpawnActor<AAIController>(EnemyAIControllerClass);

	if (NewAIController != nullptr)
	{
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

	AActor* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!Player) return;

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(Player);
	if (!ShooterCharacter) return;

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
	if (!BarrelSocket) return;

	const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());

	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
	}

	FHitResult BeamHitResult;
	bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamHitResult, Victim);

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
	if (Health <= 0) return;
	Shoot(Victim);
}

bool AShooterEnemy::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult, AActor* Victim)
{
	FVector OutBeamLocation;
	FHitResult EnemyGunHitResult;

	bool bCrosshairHit = TraceFromEnemyGuns(EnemyGunHitResult, OutBeamLocation, Victim);

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

	// barrelとEndpointの間にオブジェクトがあるか
	if (!OutHitResult.bBlockingHit) 
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
	FVector GunLocation;
	FRotator GunRotation;

	if (EquippedWeapon)
	{
		// プレイヤーのメッシュをデフォルトで回転させているため、それに合わせる
		GunLocation = EquippedWeapon->GetActorLocation();
		GunRotation = GetActorRotation();
	}
	else
	{
		return false; 
	}

	AShooterCharacter* Player = Cast<AShooterCharacter>(Victim);
	FVector AimToCrouchingOrWallRuunningPlayer = FVector(0.f, 0.f, 0.f);

	FVector Start{ GunLocation };
	float TraceDistance = 50000.f;
	FVector End{ Start + GunRotation.Vector() * TraceDistance };

	if (Player && (Player->GetCrouching() || Player->GetIsWallRunning()))
	{
		// プレイヤーの足元の位置を計算
		FVector PlayerFeetLocation = Player->GetActorLocation();
		AimToCrouchingOrWallRuunningPlayer = PlayerFeetLocation - GunLocation;

		// 60%の確率で精度を落とす
		if (FMath::RandRange(1, 100) <= 60)
		{
			// ランダムなズレを追加
			float RandomX = FMath::RandRange(-500.f, 500.f);
			float RandomY = FMath::RandRange(-500.f, 500.f);
			FVector RandomOffset(RandomX, RandomY, 0.f);
			End = AimToCrouchingOrWallRuunningPlayer + Start + RandomOffset;
		}
		else
		{
			End = AimToCrouchingOrWallRuunningPlayer + Start;
		}
	}

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(EquippedWeapon);

	GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility,
		Params);

	if (OutHitResult.bBlockingHit)
	{
		OutHitLocation = OutHitResult.Location;

		return true;
	}
	else
	{
		OutHitLocation = End;
	}

	return false;
}
