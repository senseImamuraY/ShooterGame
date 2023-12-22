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
	// 基本のrate
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	// Aimingの真偽によってTurn rateを変える
	HipTurnRate(90.f),
	HipLookUpRate(90.f),
	AimingTurnRate(20.f),
	AimingLookUpRate(20.f),
	// Mouse look感度の調整
	MouseHipTurnRate(1.f),
	MouseHipLookUpRate(1.f),
	MouseAimingTurnRate(0.6f),
	MouseAimingLookUpRate(0.6f),
	//Aimingしたとき(照準を合わせたとき)にtrue
	bAiming(false),
	// Camera field of view の値
	CameraDefaultFOV(0.f), // BeginPlayで設定する
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
	// Item獲得時のアニメーションの補間で使う距離
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
	// プレイヤーのレベルシステム
	PlayerLevel(1),
	MaxPlayerLevel(5),
	PreExPoints(100),
	EarnExPoints(0),
	PlayerAttackPower(10),
	// プレイヤーの体力
	Health(100.f),
	MaxHealth(100.f),
	bIsDead(false),
	// IconAnimationで使用
	HighlightedSlot(-1)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f; // カメラとキャラクターの距離
	CameraBoom->bUsePawnControlRotation = true; // カメラはプレイヤーの入力に基づいて回転
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// 視線を動かしたときにキャラクターが回転しないようにする
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false; // キャラクターが移動する方向に自動的に向きを変えない
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // 引数がピッチ、ヨー、ロールの順番なのに注意
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
	// default weaponをスポーンさせてそれをメッシュにアタッチ
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
	// crosshairのtrace hitをチェック
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

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

	if (!OutHitResult.bBlockingHit) // barrelとEndpointの間にオブジェクトがあるか
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
	// エイムの真偽によって視野を調整
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

	// 照準線の位置を設定
	FVector2D CrosshairLocation(CenterPosition);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// 画面座標からワールド座標へ変換して、Position,Directionに代入
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
		FVector HitLocation; // 処理の都合上入れているだけ。
		TraceUnderCrosshairs(ItemTraceResult, HitLocation);

		if (ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<AItem>(ItemTraceResult.GetActor());
			const auto TraceHitWeapon = Cast<AWeapon>(TraceHitItem);

			if (TraceHitWeapon)
			{
				if (HighlightedSlot == -1)
				{
					// 使われていないスロットをハイライトに設定
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
				// ItemのPickup Widgetを出現させる
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

			// 範囲内にある1つのアイテムのみ、widgetを出現させる
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
		// トレースしているアイテムがないので、現在表示しているWidgetを削除する
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
		// AreaSphereとCollisonBoxのコリジョン無視
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
			// 壁ジャンプできないバグを修正するために一定時間WallRunを無効にする
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

	// しゃがみ動作時はマイナス、立ち動作時はプラス
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

	// Aimingしたときのズーム調整
	CameraInterpZoom(DeltaTime);

	// AimingしたときにLook感度調整
	SetLookRates();

	// OverlappedItemCountをチェックしてからitemをトレース
	TraceForItems();

	// 状態によってcapsuleのhalf heightを補間
	InterpCapsuleHalfHeight(DeltaTime);

	if (WallRunComponent)
	{
		WallRunComponent->WallRun();
	}

	// あとで削除
	if (GEngine)
	{
		int32 MessageIndex = 0;
		for (const auto& Pair : AmmoMap)
		{
			EAmmoType AmmoType = Pair.Key;
			int32 AmmoCount = Pair.Value;

			// AmmoType は enum なので、その名前を取得する
			const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EAmmoType"), true);
			FString AmmoTypeName = (EnumPtr != nullptr) ? EnumPtr->GetNameStringByIndex(static_cast<int32>(AmmoType)) : TEXT("Unknown");

			// ログメッセージを作成
			FString LogMessage = FString::Printf(TEXT("AmmoType: %s, Count: %d"), *AmmoTypeName, AmmoCount);

			// 画面にログを表示
			GEngine->AddOnScreenDebugMessage(MessageIndex, 5.f, FColor::White, LogMessage);

			// メッセージインデックスをインクリメント
			MessageIndex++;
		}
	}

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// 入力値が動いたときに関数を呼び出す
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

		// ダメージを受けた方向を計算する
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

	// ゲームの時間の流れを一時的に停止（実際には0.0001の値が入っている）
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0f);

	// 1.5秒後に時間の流れを元に戻す
	FTimerHandle TimerHandle;
	float StopTime = 0.00015f; // 実際には時間の流れは、0秒ではなく、小さい値(0.0001)が設定されているため、この時間は1.5秒に相当

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

	// PlayerControllerを取得する
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// InGameHUDクラスを取得する
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// ゲームオーバー画面を表示する
	HUD->DispGameOver();
}

void AShooterCharacter::ResetTimeDilation()
{
	// ゲームの時間の流れを元に戻す
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
		// カンスト処理
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
			// PldayerLevelを文字列に変換します。
			FString PlayerLevelString = FString::Printf(TEXT("%d"), PlayerLevel);

			// 文字列をFNameに変換して、FindRowに渡します。
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
	// インターフェースを通じて関数を呼び出す
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

