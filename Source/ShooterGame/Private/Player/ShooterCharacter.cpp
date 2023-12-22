// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/Player/ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "../Public/Items/Item.h"
#include "Components/WidgetComponent.h"
#include "../Public/Weapon/Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "../Public/Items/Ammo.h"
#include "../Public/Interfaces/BulletHitInterface.h"
#include "../Public/Enemies/Enemy.h"
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../Public/Core/LevelSystem/PlayerLevelSystem.h"
#include "../Public/Core/InGameHUD.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Camera/CameraShakeBase.h"

// Sets default values
AShooterCharacter::AShooterCharacter() : 
	// ��{��rate
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	// Aiming�̐^�U�ɂ����Turn rate��ς���
	HipTurnRate(90.f),
	HipLookUpRate(90.f),
	AimingTurnRate(20.f),
	AimingLookUpRate(20.f),
	// Mouse look���x�̒���
	MouseHipTurnRate(1.f),
	MouseHipLookUpRate(1.f),
	MouseAimingTurnRate(0.6f),
	MouseAimingLookUpRate(0.6f),
	//Aiming�����Ƃ�(�Ə������킹���Ƃ�)��true
	bAiming(false),
	// Camera field of view �̒l
	CameraDefaultFOV(0.f), // BeginPlay�Őݒ肷��
	CameraZoomedFOV(25.f),
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(20.f),
	// Automatic gun fire rate
	AutomaticFireRate(0.1f),
	bShouldFire(true),
	bFireButtonPressed(false),
	// Bullet fire timer
	ShootTimeDuration(0.05f),
	bFiringBullet(false),
	bShouldTraceForItems(false),
	OverlappedItemCount(0),
	// Item�l�����̃A�j���[�V�����̕�ԂŎg������
	CameraInterpDistance(250.f),
	CameraInterpElevation(65.f),
	// Starting ammo amounts
	Starting9mmAmmo(85),
	StartingShellsAmmo(120),
	CombatState(ECombatState::ECS_Unoccupied),
	bCrouching(false),
	BaseMovementSpeed(650.f),
	CrouchMovementSpeed(300.f),
	StandingCapsuleHalfHeight(88.f),
	CrouchingCapsuleHalfHeight(44.f),
	BaseGroundFriction(2.f),
	CrouchingGroundFriction(100.f),
	bAimingButtonPressed(false),
	bShouldPlayPickupSound(true),
	bShouldPlayEquipSound(true),
	PickupSoundResetTime(0.2f),
	EquipSoundResetTime(0.2f),
	// �v���C���[�̃��x���V�X�e��
	PlayerLevel(1),
	MaxPlayerLevel(5),
	PreExPoints(100),
	EarnExPoints(0),
	PlayerAttackPower(10),
	// �v���C���[�̗̑�
	Health(100.f),
	MaxHealth(100.f),
	bIsDead(false),
	// IconAnimation�Ŏg�p
	HighlightedSlot(-1)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f; // �J�����ƃL�����N�^�[�̋���
	CameraBoom->bUsePawnControlRotation = true; // �J�����̓v���C���[�̓��͂Ɋ�Â��ĉ�]
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// �����𓮂������Ƃ��ɃL�����N�^�[����]���Ȃ��悤�ɂ���
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false; // �L�����N�^�[���ړ���������Ɏ����I�Ɍ�����ς��Ȃ�
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // �������s�b�`�A���[�A���[���̏��ԂȂ̂ɒ���
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComp"));

	WeaponInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Interpolation Component"));
	WeaponInterpComp->SetupAttachment(GetFollowCamera());

	InterpComp1 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 1"));
	InterpComp1->SetupAttachment(GetFollowCamera());

	InterpComp2 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 2"));
	InterpComp2->SetupAttachment(GetFollowCamera());

	InterpComp3 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 3"));
	InterpComp3->SetupAttachment(GetFollowCamera());

	InterpComp4 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 4"));
	InterpComp4->SetupAttachment(GetFollowCamera());

	InterpComp5 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 5"));
	InterpComp5->SetupAttachment(GetFollowCamera());

	InterpComp6 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 6"));
	InterpComp6->SetupAttachment(GetFollowCamera());

	// Create and attach WallRunningComponent to the character
	WallRunComponent = CreateDefaultSubobject<UWallRunComponent>(TEXT("WallRunComponent"));

	LevelUpSound = LoadObject<USoundBase>(nullptr, TEXT("/Game/ShooterGame/Audio/SE/LevelUp/LevelUpSound.LevelUpSound"));

	static ConstructorHelpers::FClassFinder<UCameraShakeBase> CameraShake(TEXT("/Game/ShooterGame/Blueprints/Graphics/BP_CameraShakeForDeath.BP_CameraShakeForDeath_C"));
	if (CameraShake.Class != NULL)
	{
		CameraShakeClass = CameraShake.Class;
	}
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
	// default weapon���X�|�[�������Ă�������b�V���ɃA�^�b�`
	EquipWeapon(SpawnDefaultWeapon());
	WeaponInventory.Add(EquippedWeapon);
	EquippedWeapon->SetSlotIndex(0);
	EquippedWeapon->SetCharacter(this);

	InitializeAmmoMap();
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;

	InitializeInterpLocations();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->SetShowMouseCursor(false);
	PlayerController->SetInputMode(FInputModeGameOnly());

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
}

void AShooterCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && !WallRunComponent->GetIsWallRunning())
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
	else if ((Controller != nullptr) && (Value != 0.0f) && WallRunComponent->GetIsWallRunning())
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ GetActorForwardVector()};
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && !WallRunComponent->GetIsWallRunning())
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
	else if ((Controller != nullptr) && (Value != 0.0f) && WallRunComponent->GetIsWallRunning())
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ GetActorRightVector()};
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::Turn(float Value)
{
	float TurnScaleFactor{};
	if (bAiming)
	{
		TurnScaleFactor = MouseAimingTurnRate;
	}
	else
	{
		TurnScaleFactor = MouseHipTurnRate;
	}

	if (WallRunComponent->GetIsWallRunning())
	{
		TurnScaleFactor = 0.f;
	}

	AddControllerYawInput(Value * TurnScaleFactor);
}

void AShooterCharacter::LookUp(float Value)
{
	float LookUPScaleFactor{};
	if (bAiming)
	{
		LookUPScaleFactor = MouseAimingLookUpRate;
	}
	else
	{
		LookUPScaleFactor = MouseHipLookUpRate;
	}

	if (WallRunComponent->GetIsWallRunning())
	{
		LookUPScaleFactor = 0.f;
	}

	AddControllerPitchInput(Value * LookUPScaleFactor);
}

void AShooterCharacter::FireWeapon()
{
	if (EquippedWeapon == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (EquippedWeapon->GetbIsFiringCooldown() || WeaponHasAmmo())
	{
		PlayFireSound();
		EquippedWeapon->Fire(this);
		PlayGunfireMontage();
		EquippedWeapon->DecrementAmmo();

		StartCrosshairBulletFire();
		StartFireTimer(EquippedWeapon->GetCooldownTime());
	}
	else if (WeaponHasAmmo())
	{
		PlayFireSound();
		EquippedWeapon->Fire(this);
		PlayGunfireMontage();
		EquippedWeapon->DecrementAmmo();

		StartCrosshairBulletFire();
		float NoTime = 0;
		StartFireTimer(NoTime);
	}
}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult)
{
	FVector OutBeamLocation;
	// crosshair��trace hit���`�F�b�N
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

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

	if (!OutHitResult.bBlockingHit) // barrel��Endpoint�̊ԂɃI�u�W�F�N�g�����邩
	{
		OutHitResult.Location = OutBeamLocation;;
		return false;
	}

	return true;
}


void AShooterCharacter::AimingButtonPressed()
{
	bAimingButtonPressed = true;

	if (CombatState != ECombatState::ECS_Reloading)
	{
		Aim();
	}
}

void AShooterCharacter::AimingButtonReleased()
{
	bAimingButtonPressed = false;
	StopAiming();
}

void AShooterCharacter::CameraInterpZoom(float DeltaTime)
{
	// �G�C���̐^�U�ɂ���Ď���𒲐�
	if (bAiming)
	{
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV, 
			CameraZoomedFOV,
			DeltaTime,
			ZoomInterpSpeed);
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraDefaultFOV,
			DeltaTime,
			ZoomInterpSpeed);
	}

	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}

void AShooterCharacter::SetLookRates()
{
	if (bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

void AShooterCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;

	GetWorldTimerManager().SetTimer(
		CrosshairShootTimer,
		this,
		&AShooterCharacter::FinishCrosshairBulletFire,
		ShootTimeDuration);
}

void AShooterCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

void AShooterCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	FireWeapon();
}

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer(float Time)
{
	CombatState = ECombatState::ECS_FireTimerInProgress;

	GetWorldTimerManager().SetTimer(
		AutoFireTimer,
		this,
		&AShooterCharacter::AutoFireReset,
		AutomaticFireRate + Time);
}

void AShooterCharacter::AutoFireReset()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (WeaponHasAmmo())
	{
		if (bFireButtonPressed)
		{
			FireWeapon();
		}
	}
	else
	{
		ReloadWeapon();
	}
}

bool AShooterCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
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
		UGameplayStatics::GetPlayerController(this, 0),
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

			if (OutHitResult.bBlockingHit)
			{
				OutHitLocation = OutHitResult.Location;
				return true;
			}

			return true;
		}
	}

	return false;
}

void AShooterCharacter::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation; // �����̓s�������Ă��邾���B
		TraceUnderCrosshairs(ItemTraceResult, HitLocation);

		if (ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<AItem>(ItemTraceResult.GetActor());
			const auto TraceHitWeapon = Cast<AWeapon>(TraceHitItem);

			if (TraceHitWeapon)
			{
				if (HighlightedSlot == -1)
				{
					// �g���Ă��Ȃ��X���b�g���n�C���C�g�ɐݒ�
					HighlightInventorySlot();
				}
			}
			else
			{
				if (HighlightedSlot != -1)
				{
					UnHighlightInventorySlot();
				}
			}

			if (TraceHitItem && TraceHitItem->GetItemState() == EItemState::EIS_EquipInterping)
			{
				TraceHitItem = nullptr;
			}

			if (TraceHitItem && TraceHitItem->GetPickupWidget() && TraceHitItem->IsOverlappingActor(this))
			{
				// Item��Pickup Widget���o��������
				TraceHitItem->GetPickupWidget()->SetVisibility(true);

				if (WeaponInventory.Num() >= INVENTORY_CAPACITY)
				{
					TraceHitItem->SetCharacterInventoryFull(true);
				}
				else
				{
					TraceHitItem->SetCharacterInventoryFull(false);
				}
			}

			// �͈͓��ɂ���1�̃A�C�e���̂݁Awidget���o��������
			if (TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				}
			}

			TraceHitItemLastFrame = TraceHitItem;
		}
	}
	else if (TraceHitItemLastFrame)
	{
		// �g���[�X���Ă���A�C�e�����Ȃ��̂ŁA���ݕ\�����Ă���Widget���폜����
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
	}
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}

	return nullptr;
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip)
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

		if (EquippedWeapon == nullptr)
		{
			EquipItemDelegate.Broadcast(-1, WeaponToEquip->GetSlotIndex());
		}
		else
		{
			EquipItemDelegate.Broadcast(EquippedWeapon->GetSlotIndex(), WeaponToEquip->GetSlotIndex());
		}

		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

void AShooterCharacter::SelectButtonPressed()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (TraceHitItem)
	{
		TraceHitItem->StartItemCurve(this, true);
		TraceHitItem = nullptr;
	}
}

void AShooterCharacter::SelectButtonReleased()
{

}

void AShooterCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, Starting9mmAmmo);
	AmmoMap.Add(EAmmoType::EAT_Shells, StartingShellsAmmo);
}

bool AShooterCharacter::WeaponHasAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	return EquippedWeapon->GetAmmo() > 0;
}

void AShooterCharacter::PlayFireSound()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
}

void AShooterCharacter::PlayGunfireMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

void AShooterCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AShooterCharacter::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (EquippedWeapon == nullptr) return;

	if (CarryingAmmo() && !EquippedWeapon->ClipIsFull())
	{
		if (bAiming)
		{
			StopAiming();
		}

		CombatState = ECombatState::ECS_Reloading;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && ReloadMontage)
		{
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
		}
	}
}

bool AShooterCharacter::CarryingAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	auto AmmoType = EquippedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}

	return false;
}

void AShooterCharacter::GrabClip()
{
	if (EquippedWeapon == nullptr) return;
	if (HandSceneComponent == nullptr) return;

	int32 ClipBoneIndex{ EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetClipBoneName()) };
	ClipTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(ClipBoneIndex);

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("Hand_L")));
	HandSceneComponent->SetWorldTransform(ClipTransform);

	EquippedWeapon->SetMovingClip(true);
}

void AShooterCharacter::ReleaseClip()
{
	EquippedWeapon->SetMovingClip(false);
}

void AShooterCharacter::CrouchButtonPressed()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		bCrouching = !bCrouching;
	}
	if (bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
		GetCharacterMovement()->GroundFriction = CrouchingGroundFriction;
	}
	else
	{
		if (!bAiming)
		{
			GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
		}
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}
}

void AShooterCharacter::Jump()
{
	if (bCrouching)
	{
		bCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
	else
	{
		if (WallRunComponent->GetIsWallRunning())
		{
			// �ǃW�����v�ł��Ȃ��o�O���C�����邽�߂Ɉ�莞��WallRun�𖳌��ɂ���
			WallRunComponent->SetCanWallRun(false);
			FTimerHandle& TimerHandle = WallRunComponent->GetWallRunTimerHandle();
			float WallRunInterval = 0.5f;
			GetWorldTimerManager().SetTimer(TimerHandle, WallRunComponent, &UWallRunComponent::EnableWallRun, WallRunInterval, false);

			FRotator JumpDirection = WallRunComponent->GetHitWallNormal().Rotation();
			SetActorRotation(JumpDirection);

			const FVector JumpForwardBoost = FVector(25000.f, 0.f, 0.f);
			const FVector JumpUpwardForce = FVector(0.f, 0.f, 15000.f);

			FVector LaunchVelocity = GetActorForwardVector() * JumpForwardBoost + JumpUpwardForce;
			LaunchCharacter(LaunchVelocity, true, true);
		}
		else
		{
			ACharacter::Jump();
		}
	}
}

void AShooterCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight = 0;

	if (bCrouching)
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}
	else
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}
	const float InterpHalfHeight{ FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), TargetCapsuleHalfHeight, DeltaTime, 20.f)};

	// ���Ⴊ�ݓ��쎞�̓}�C�i�X�A�������쎞�̓v���X
	const float DeltaCapsuleHalfHeight{ InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };
	const FVector MeshOffset{ 0.f, 0.f, -DeltaCapsuleHalfHeight };
	GetMesh()->AddLocalOffset(MeshOffset);

	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
}

void AShooterCharacter::Aim()
{
	bAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
}

void AShooterCharacter::StopAiming()
{
	bAiming = false;

	if (!bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
}

void AShooterCharacter::InitializeInterpLocations()
{
	float InitialItemCount = 0;

	FInterpLocation WeaponLocation{ WeaponInterpComp, InitialItemCount };
	InterpLocations.Add(WeaponLocation);

	FInterpLocation InterpLoc1{ InterpComp1, InitialItemCount };
	InterpLocations.Add(InterpLoc1);

	FInterpLocation InterpLoc2{ InterpComp2, InitialItemCount };
	InterpLocations.Add(InterpLoc2);

	FInterpLocation InterpLoc3{ InterpComp3, InitialItemCount };
	InterpLocations.Add(InterpLoc3);

	FInterpLocation InterpLoc4{ InterpComp4, InitialItemCount };
	InterpLocations.Add(InterpLoc4);

	FInterpLocation InterpLoc5{ InterpComp5, InitialItemCount };
	InterpLocations.Add(InterpLoc5);

	FInterpLocation InterpLoc6{ InterpComp6, InitialItemCount };
	InterpLocations.Add(InterpLoc6);
}

int32 AShooterCharacter::GetInterpLocationIndex()
{
	int32 LowestIndex = 1;
	int32 LowestCount = INT_MAX;
	for (int32 i = 1; i < InterpLocations.Num(); i++)
	{
		if (InterpLocations[i].ItemCount < LowestCount)
		{
			LowestIndex = i;
			LowestCount = InterpLocations[i].ItemCount;
		}
	}

	return LowestIndex;
}

void AShooterCharacter::IncrementInterpLocItemCount(int32 Index, int32 Amount)
{
	if (Amount < -1 || Amount > 1) return;

	if (InterpLocations.Num() >= Index)
	{
		InterpLocations[Index].ItemCount += Amount;
	}
}

void AShooterCharacter::StartPickupSoundTimer()
{
	bShouldPlayPickupSound = false;
	GetWorldTimerManager().SetTimer(
		PickupSoundTimer,
		this,
		&AShooterCharacter::ResetPickupSoundTimer,
		PickupSoundResetTime);
}

void AShooterCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(
		EquipSoundTimer,
		this,
		&AShooterCharacter::ResetEquipSoundTimer,
		EquipSoundResetTime);
}

void AShooterCharacter::SetEquippedWeapon(AWeapon* NewWeapon)
{
	if (!NewWeapon) return;
	EquippedWeapon = NewWeapon;
}

void AShooterCharacter::SetTraceHitItem(AItem* NewTraceHitItem)
{
	TraceHitItem = NewTraceHitItem;
}

void AShooterCharacter::SetTraceHitItemLastFrame(AItem* NewTraceHitItemLastFrame)
{
	TraceHitItemLastFrame = NewTraceHitItemLastFrame;
}


void AShooterCharacter::SetPlayerHealth(float RecoveryAmount)
{
	Health = RecoveryAmount;
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Aiming�����Ƃ��̃Y�[������
	CameraInterpZoom(DeltaTime);

	// Aiming�����Ƃ���Look���x����
	SetLookRates();

	// OverlappedItemCount���`�F�b�N���Ă���item���g���[�X
	TraceForItems();

	// ��Ԃɂ����capsule��half height����
	InterpCapsuleHalfHeight(DeltaTime);

	if (WallRunComponent)
	{
		WallRunComponent->WallRun();
	}

	// ���Ƃō폜
	if (GEngine)
	{
		int32 MessageIndex = 0;
		for (const auto& Pair : AmmoMap)
		{
			EAmmoType AmmoType = Pair.Key;
			int32 AmmoCount = Pair.Value;

			// AmmoType �� enum �Ȃ̂ŁA���̖��O���擾����
			const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EAmmoType"), true);
			FString AmmoTypeName = (EnumPtr != nullptr) ? EnumPtr->GetNameStringByIndex(static_cast<int32>(AmmoType)) : TEXT("Unknown");

			// ���O���b�Z�[�W���쐬
			FString LogMessage = FString::Printf(TEXT("AmmoType: %s, Count: %d"), *AmmoTypeName, AmmoCount);

			// ��ʂɃ��O��\��
			GEngine->AddOnScreenDebugMessage(MessageIndex, 5.f, FColor::White, LogMessage);

			// ���b�Z�[�W�C���f�b�N�X���C���N�������g
			MessageIndex++;
		}
	}

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// ���͒l���������Ƃ��Ɋ֐����Ăяo��
	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);

	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &AShooterCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction("Select", IE_Released, this, &AShooterCharacter::SelectButtonReleased);

	PlayerInputComponent->BindAction("ReloadButton", IE_Pressed, this, &AShooterCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterCharacter::CrouchButtonPressed);

	PlayerInputComponent->BindAction("FKey", IE_Pressed, this, &AShooterCharacter::FKeyPressed);
	PlayerInputComponent->BindAction("1Key", IE_Pressed, this, &AShooterCharacter::OneKeyPressed);
	PlayerInputComponent->BindAction("2Key", IE_Pressed, this, &AShooterCharacter::TwoKeyPressed);
	PlayerInputComponent->BindAction("3Key", IE_Pressed, this, &AShooterCharacter::ThreeKeyPressed);
}



float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Die();
	}
	else
	{
		Health -= DamageAmount;

		// �_���[�W���󂯂��������v�Z����
		FVector DamageDirection;
		if (DamageCauser)
		{
			DamageDirection = GetActorLocation() - DamageCauser->GetActorLocation();
			DamageDirection.Normalize();
		}
		else
		{
			DamageDirection = FVector::BackwardVector;
		}
	}
	return DamageAmount;
}

void AShooterCharacter::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
	}

	bIsDead = true;

	// �Q�[���̎��Ԃ̗�����ꎞ�I�ɒ�~�i���ۂɂ�0.0001�̒l�������Ă���j
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0f);

	// 1.5�b��Ɏ��Ԃ̗�������ɖ߂�
	FTimerHandle TimerHandle;
	float StopTime = 0.00015f; // ���ۂɂ͎��Ԃ̗���́A0�b�ł͂Ȃ��A�������l(0.0001)���ݒ肳��Ă��邽�߁A���̎��Ԃ�1.5�b�ɑ���

	if (CameraShakeClass != NULL)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC)
		{
			PC->ClientStartCameraShake(CameraShakeClass);
		}
	}

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AShooterCharacter::ResetTimeDilation, StopTime, false);
}

void AShooterCharacter::FinishDeath()
{
	GetMesh()->bPauseAnims = true;
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		DisableInput(PC);
	}

	// PlayerController���擾����
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// InGameHUD�N���X���擾����
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// �Q�[���I�[�o�[��ʂ�\������
	HUD->DispGameOver();
}

void AShooterCharacter::ResetTimeDilation()
{
	// �Q�[���̎��Ԃ̗�������ɖ߂�
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
}

void AShooterCharacter::FinishReloading()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed)
	{
		Aim();
	}

	if (EquippedWeapon == nullptr) return;
	const auto AmmoType{ EquippedWeapon->GetAmmoType() };

	if (AmmoMap.Contains(AmmoType))
	{
		int32 CarriedAmmo = AmmoMap[AmmoType];

		const int32 MagEmptySpace = EquippedWeapon->GetMagazineCapacity() - 
			EquippedWeapon->GetAmmo();

		if (MagEmptySpace > CarriedAmmo)
		{
			EquippedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
		else
		{
			EquippedWeapon->ReloadAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
	}
}

void AShooterCharacter::FinishEquipping()
{
	CombatState = ECombatState::ECS_Unoccupied;
}

void AShooterCharacter::ResetPickupSoundTimer()
{
	bShouldPlayPickupSound = true;
}

void AShooterCharacter::ResetEquipSoundTimer()
{
	bShouldPlayEquipSound = true;
}

void AShooterCharacter::CalculateExPoints_Implementation(float AddedExPoints)
{
	EarnExPoints += AddedExPoints;

	while (EarnExPoints >= PreExPoints) {
		// �J���X�g����
		if (PlayerLevel == MaxPlayerLevel) {
			EarnExPoints = 0;
			return;
		}
		if (!LevelUpSound) return;

		PlayerLevel++;
		UGameplayStatics::PlaySoundAtLocation(this, LevelUpSound, GetActorLocation());
		EarnExPoints -= PreExPoints;

		FString DataTablePath(TEXT("/Game/ShooterGame/Blueprints/Core/LevelSystem/DT_PlayerLevelSystem.DT_PlayerLevelSystem"));
		UDataTable* ExPointsDataTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *DataTablePath));
		if (ExPointsDataTableObject) {
			FPlayerExP* ExPRow = nullptr;
			// PldayerLevel�𕶎���ɕϊ����܂��B
			FString PlayerLevelString = FString::Printf(TEXT("%d"), PlayerLevel);

			// �������FName�ɕϊ����āAFindRow�ɓn���܂��B
			ExPRow = ExPointsDataTableObject->FindRow<FPlayerExP>(*PlayerLevelString, TEXT(""));
			PreExPoints = ExPRow->PlayerExp;
			PlayerAttackPower = ExPRow->PlayerAttackPower;
		}
	}
}

void AShooterCharacter::FKeyPressed()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("FKeyPressed called"));
	}

	if (EquippedWeapon->GetSlotIndex() == 0) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 0);
}

void AShooterCharacter::OneKeyPressed()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("OneKeyPressed called"));
	}

	if (EquippedWeapon->GetSlotIndex() == 1) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 1);
}

void AShooterCharacter::TwoKeyPressed()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("TwoKeyPressed called"));
	}

	if (EquippedWeapon->GetSlotIndex() == 2) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 2);
}

void AShooterCharacter::ThreeKeyPressed()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("ThreeKeyPressed called"));
	}


	if (EquippedWeapon->GetSlotIndex() == 3) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 3);
}

void AShooterCharacter::ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex)
{
	if ((CurrentItemIndex == NewItemIndex) || (NewItemIndex >= WeaponInventory.Num()) || (CombatState != ECombatState::ECS_Unoccupied)) return;

	auto OldEquippedWeapon = EquippedWeapon;
	auto NewWeapon = Cast<AWeapon>(WeaponInventory[NewItemIndex]);
	EquipWeapon(NewWeapon);

	OldEquippedWeapon->SetItemState(EItemState::EIS_PickedUp);
	NewWeapon->SetItemState(EItemState::EIS_Equipped);

	CombatState = ECombatState::ECS_Equipping;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Equip"));
	}
	NewWeapon->PlayEquipSound(true);
}

int32 AShooterCharacter::GetEmptyInventorySlot()
{
	for (int32 i = 0; i < WeaponInventory.Num(); i++)
	{
		if (WeaponInventory[i] == nullptr)
		{
			return i;
		}
	}
	if (WeaponInventory.Num() < INVENTORY_CAPACITY)
	{
		return WeaponInventory.Num();
	}

	return -1;
}

void AShooterCharacter::HighlightInventorySlot()
{
	const int32 EmptySlot = GetEmptyInventorySlot();
	HighlightIconDelegate.Broadcast(EmptySlot, true);
	HighlightedSlot = EmptySlot;
}

void AShooterCharacter::UnHighlightInventorySlot()
{
	HighlightIconDelegate.Broadcast(HighlightedSlot, false);
	HighlightedSlot = -1;
}

void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

void AShooterCharacter::GetPickupItem(AItem* Item)
{
	// �C���^�[�t�F�[�X��ʂ��Ċ֐����Ăяo��
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(Item);
	if (PickupInterface)
	{
		PickupInterface->PickupItem(this);
	}
}

FInterpLocation AShooterCharacter::GetInterpLocation(int32 Index)
{
	if (Index <= InterpLocations.Num())
	{
		return InterpLocations[Index];
	}
	return FInterpLocation();
}

