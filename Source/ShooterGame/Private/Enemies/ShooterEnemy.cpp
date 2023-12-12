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

	// Enemy��AI Controller�����蓖�Ă�
	//C++������ShooterEnemy���C���X�^���X������ꍇ�A�����I�ɂ��̑�����s���K�v������
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
		// AreaSphere��CollisonBox�̃R���W��������
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

	// Barrel����g���[�X���s���BBarrel����̋O����D�悵�ē����蔻����s���B
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBeamLocation - MuzzleSocketLocation };
	// Location���s�b�^���̏ꍇ�A�ڐG���Ȃ��i�������ŏՓ˔��肪�s����ɂȂ�j�\�������邽�߁A1.25�{����
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

	// barrel��Endpoint�̊ԂɃI�u�W�F�N�g�����邩
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
		// �v���C���[�̃��b�V�����f�t�H���g�ŉ�]�����Ă��邽�߁A����ɍ��킹��
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
		// �v���C���[�̑����̈ʒu���v�Z
		FVector PlayerFeetLocation = Player->GetActorLocation();
		AimToCrouchingOrWallRuunningPlayer = PlayerFeetLocation - GunLocation;

		// 60%�̊m���Ő��x�𗎂Ƃ�
		if (FMath::RandRange(1, 100) <= 60)
		{
			// �����_���ȃY����ǉ�
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
