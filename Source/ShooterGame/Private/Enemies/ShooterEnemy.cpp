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

	// AI Controller�N���X���w��
	UClass* EnemyAIControllerClass = AShooterEnemyAIController::StaticClass();
	
	// AI Controller�̐���
	AAIController* NewAIController = GetWorld()->SpawnActor<AAIController>(EnemyAIControllerClass);

	if (NewAIController != nullptr)
	{
		// Enemy��AI Controller�����蓖�Ă�
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

	// �v���C���[�̃A�N�^�[���擾
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
	// crosshair��trace hit���`�F�b�N
	FHitResult CrosshairHitResult;

	bool bCrosshairHit = TraceFromEnemyGuns(CrosshairHitResult, OutBeamLocation, Victim);

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

	// ���C�g���[�X�̉���
	FColor LineColor = OutHitResult.bBlockingHit ? FColor::Red : FColor::Green;
	DrawDebugLine(
		GetWorld(),
		WeaponTraceStart,
		WeaponTraceEnd,
		LineColor,
		false, // �i���I�ȃ��C���ł͂Ȃ�
		1.0f, // �\�����ԁi�b�j
		0, // DepthPriority
		1.0f // Thickness
	);

	if (!OutHitResult.bBlockingHit) // barrel��Endpoint�̊ԂɃI�u�W�F�N�g�����邩
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
	// �e�̈ʒu�ƌ������擾
	FVector GunLocation;
	FRotator GunRotation;
	if (EquippedWeapon) // MyGun�͏e�̎Q��
	{
		GunLocation = EquippedWeapon->GetActorLocation();
		GunRotation = EquippedWeapon->GetActorRotation();
		// ���O�o�͂�ǉ�
		UE_LOG(LogTemp, Warning, TEXT("Gun Location: %s"), *GunLocation.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Gun Rotation: %s"), *GunRotation.ToString());

	}
	else
	{
		return false; // �e���Ȃ��ꍇ�͏����𒆒f
	}

	GunRotation = GetActorRotation();

	AShooterCharacter* Player = Cast<AShooterCharacter>(Victim);
	FVector AimToCrouchingPlayer = FVector(0.f, 0.f, 0.f);

	FVector Start{ GunLocation };
	FVector End{ Start + GunRotation.Vector() * 50000.f }; // �����Ń��C�g���[�X�̋�����ݒ�

	if (Player && (Player->GetCrouching() || Player->GetIsWallRunning()))
	{
		// �v���C���[�̑����̈ʒu���v�Z
		FVector PlayerFeetLocation = Player->GetActorLocation();
		//FVector PlayerFeetLocation = Player->GetActorLocation() - FVector(0.f, 0.f, Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		AimToCrouchingPlayer = PlayerFeetLocation - GunLocation;

		// 60%�̊m���Ő��x�𗎂Ƃ�
		if (FMath::RandRange(1, 100) <= 60)
		{
			// �����_���ȃY����ǉ�
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

	// ���C�g���[�X�̎��s
	GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility,
		Params);

	// ���C�g���[�X�̉���
	FColor LineColor = OutHitResult.bBlockingHit ? FColor::Red : FColor::Green;
	DrawDebugLine(
		GetWorld(),
		Start,
		End,
		LineColor,
		false, // �i���I�ȃ��C���ł͂Ȃ�
		1.0f, // �\�����ԁi�b�j
		0, // DepthPriority
		1.0f // Thickness
	);

	// �q�b�g�����ꍇ�̏���
	if (OutHitResult.bBlockingHit)
	{
		OutHitLocation = OutHitResult.Location;

		AActor* HitActor = OutHitResult.GetActor();
		if (HitActor)
		{
			FString ActorName = HitActor->GetName();
			FString ActorClass = HitActor->GetClass()->GetName();

			// ���O�Ƀq�b�g�����A�N�^�[�̏����o��
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s, Class: %s"), *ActorName, *ActorClass);
		}

		// �q�b�g�����R���|�[�l���g�̏����擾
		UPrimitiveComponent* HitComponent = OutHitResult.GetComponent();
		if (HitComponent)
		{
			FString ComponentName = HitComponent->GetName();
			FString ComponentClass = HitComponent->GetClass()->GetName();

			// ���O�Ƀq�b�g�����R���|�[�l���g�̏����o��
			UE_LOG(LogTemp, Warning, TEXT("Hit Component: %s, Class: %s"), *ComponentName, *ComponentClass);
		}

		// �q�b�g�ʒu�Ɩ@���̏������O�ɏo��
		FVector HitLocation = OutHitResult.Location;
		FVector HitNormal = OutHitResult.Normal;
		UE_LOG(LogTemp, Warning, TEXT("Hit Location: %s, Normal: %s"), *HitLocation.ToString(), *HitNormal.ToString());


		return true;
	}

	return false;
}
