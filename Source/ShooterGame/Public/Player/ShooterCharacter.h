// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Items/AmmoType.h"
#include "../PlayerActionComponents/WallRunComponent.h"
#include "../Public/Interfaces/ExPointsInterface.h"
#include "ShooterCharacter.generated.h"

class UInputComponent;
class AWeapon;
class AAmmo;
class USpringArmComponent;
class UCameraComponent;
class USoundCue;
class UParticleSystem;
class UAnimMontage;
class AItem;

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_Equipping UMETA(DisplayName = "Equipping"),

	ECS_MAX UMETA(DisplayName = "Default Max")
};

USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()

	// interping�Ŏg�p
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent;

	// interping�Ŏg�p����A�C�e���̐�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlot, int32, NewSlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightIconDelegate, int32, SlotIndex, bool, bStartAnimation);

UCLASS()
class SHOOTERGAME_API AShooterCharacter : public ACharacter, public IExPointsInterface
{
	GENERATED_BODY()

public:
	AShooterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// �_���[�W���󂯂鏈��������֐�
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;

	void ReloadWeapon();

	// �Ō�̃t���[���Ńq�b�g�����A�C�e��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItemLastFrame;

	// �X���b�g�̏���InventoryBar�ɓn�����߂̃f���Q�[�g
	UPROPERTY(BlueprintAssignable, Category = Delegates, meta = (AllowPrivateAccess = "true"))
	FEquipItemDelegate EquipItemDelegate;

	// �X���b�g�̏���IconAnimation�ɓn�����߂̃f���Q�[�g
	UPROPERTY(BlueprintAssignable, Category = Delegates, meta = (AllowPrivateAccess = "true"))
	FHighlightIconDelegate HighlightIconDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	int32 HighlightedSlot;

	void UnHighlightInventorySlot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);	// �㉺�̓��͂��󂯎��

	void MoveRight(float Value);	// ���E�̓��͂��󂯎��

	// Rate�̒l��ύX����֐�
	// @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired rate
	void TurnAtRate(float Rate);

	// �C���v�b�g��ʂ��āAlook up/down rate�𒲐�
	// @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired rate
	void LookUpAtRate(float Rate);

	// �}�E�X��X�����̓����ɂ���ĉ�]
	// @param Value  The input value from mouse movement
	void Turn(float Value);

	// �}�E�X��Y�����̓����ɂ���ĉ�]
	// @param Value  The input value from mouse movement
	void LookUp(float Value);

	void FireWeapon(); // �e�̃{�^�����������Ƃ��ɌĂяo��
	 
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult);

	// bAiming�̐^�U��ݒ肷��
	void AimingButtonPressed();
	void AimingButtonReleased();

	void CameraInterpZoom(float DeltaTime);

	// Aiming�̒l�Ɋ�Â���BaseTurnRate��BaseLookUpRate��ݒ肷��
	void SetLookRates();

	void StartCrosshairBulletFire();

	UFUNCTION()
	void FinishCrosshairBulletFire();

	void FireButtonPressed();
	void FireButtonReleased();

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

	// �Ə��̐�ɂ���A�C�e����T��
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

	// OverlappedItemCount > 0�Ȃ�A�C�e�����g���[�X
	void TraceForItems();

	// default weapon���X�|�[�����Ă����ԋp
	AWeapon* SpawnDefaultWeapon();

	// weapon���擾���ă��b�V���ɃA�^�b�`
	void EquipWeapon(AWeapon* WeaponToEquip);

	void SelectButtonPressed();
	void SelectButtonReleased();

	// Ammo�̒l��������
	void InitializeAmmoMap();

	// Weapon��Ammo���c���Ă��邩�m�F
	bool WeaponHasAmmo();


	// FireWeapon�Ŏg�p����֐�
	void PlayFireSound();
	void PlayGunfireMontage();

	// ���͂��m�F
	void ReloadButtonPressed();

	// ���������Ă���weapon��ammoType�ɂ�����ammo�������Ă��邩�`�F�b�N����
	bool CarryingAmmo();

	// Animation Blueprint��Grab Clip notify�ŌĂяo�����
	UFUNCTION(BlueprintCallable)
	void GrabClip();

	// Animation Blueprint��Release Clip notify�ŌĂяo�����
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

	void CrouchButtonPressed();

	virtual void Jump() override;

	// ��Ԃɂ����half height��ύX
	void InterpCapsuleHalfHeight(float DeltaTime);

	void Aim();
	void StopAiming();

	void InitializeInterpLocations();

	virtual void CalculateExPoints_Implementation(float AddedExPoints) override; 

	void FKeyPressed();
	void OneKeyPressed();
	void TwoKeyPressed();
	void ThreeKeyPressed();

	void ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex);

	int32 GetEmptyInventorySlot();

	void HighlightInventorySlot();

private:
	// �L�����N�^�[�̌��ɃJ������u��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	// �L�����N�^�[�̃J�����œ���
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	// ���E�̉�]�̑������� deg/sec 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	// �㉺�̉�]�̑������� deg/sec 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	// Aiming(�Ə������킹�Ă���)���Ă��Ȃ��Ƃ���TurnRate
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipTurnRate;

	// Aiming���Ă��Ȃ��Ƃ���LookUpRate
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate;

	// Aiming���Ă���Ƃ���TurnRate
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingTurnRate;

	// Aiming���Ă���Ƃ���LookUpRate
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingLookUpRate;

	// �}�E�X��look���x�BAiming���Ă��Ȃ��Ƃ���TurnRate
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax= "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipTurnRate;

	// �}�E�X��look���x�BAiming���Ă��Ȃ��Ƃ���LookUpRate
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate;

	// �}�E�X��look���x�BAiming���Ă���Ƃ���TurnRate
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate;

	// �}�E�X��look���x�BAiming���Ă���Ƃ���LookUpRate
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookUpRate;

	// �e���������Ƃ��Ƀ����_���ɉ����𗬂�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* MuzzleFlash;

	// �e�������߂̃����^�[�W��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HipFireMontage;

	// �e�e�̃q�b�g�G�t�F�N�g
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	// Aiming����true�ɂȂ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	// �J�����̎���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float CameraDefaultFOV;

	// �Y�[�������Ƃ��̃J�����̎���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float CameraZoomedFOV;

	// ����t���[���ł̃J�����̎���
	float CameraCurrentFOV;

	// �Y�[���̑��x
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	float ShootTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;

	// �}�E�X�̍����N���b�N���ꂽ���ǂ���
	bool bFireButtonPressed;

	// �e���ł��邩�ǂ���
	bool bShouldFire;

	// �������C��rate
	float AutomaticFireRate;

	// �e���̃C���^�[�o��
	FTimerHandle AutoFireTimer;

	// �A�C�e�����g���[�X���邩�ǂ���
	bool bShouldTraceForItems;

	// �d�Ȃ��Ă���A�C�e���̐�
	int8 OverlappedItemCount;


	// ���ݑ������Ă���Weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	// ���݃g���[�X�Ƀq�b�g���Ă���A�C�e��(null�̉\������)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	// ��ԃA�j���[�V����������ۂɎg�p����x��(�O����)�̋���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpDistance;

	// ��ԃA�j���[�V����������ۂɎg�p����z��(�����)�̋���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpElevation;

	// ammo�̎�ނ��Ƃɒe���Ǘ������邽�߂�Map
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	// 9mmAmmo�̏����l
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;

	// ARAmmo�̏����l
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 StartingShellsAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	// reload�A�j���[�V������Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	// reload�A�j���[�V������Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* EquipMontage;

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	// �����[�h�A�j���[�V�������Ɏg�p
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FTransform ClipTransform;

	// �����[�h�A�j���[�V�������Ɏg�p�B�L�����N�^�[�̎��Bone�Ɏ��t����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;

	// ���Ⴊ��ł���Ƃ���true
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bCrouching;

	// �ʏ�̈ړ����x
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float BaseMovementSpeed;

	// ���Ⴊ�ݒ��̈ړ����x
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchMovementSpeed;

	// ���݂̃J�v�Z���̔����̍���
	float CurrentCapsuleHalfHeight;

	// �����Ă���Ƃ��̃J�v�Z���̔����̍���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float StandingCapsuleHalfHeight;

	// ���Ⴊ�ݒ��̃J�v�Z���̔����̍���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchingCapsuleHalfHeight;

	// �����Ă���Ƃ��̖��C
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float BaseGroundFriction;

	// ���Ⴊ�ݒ��̖��C
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchingGroundFriction;

	bool bAimingButtonPressed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* WeaponInterpComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp6;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FInterpLocation> InterpLocations;
	
	FTimerHandle PickupSoundTimer;
	FTimerHandle EquipSoundTimer;

	bool bShouldPlayPickupSound;
	bool bShouldPlayEquipSound;

	void ResetPickupSoundTimer();
	void ResetEquipSoundTimer();

	// ����Pickup Sound��炷�܂ł̎���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float PickupSoundResetTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float EquipSoundResetTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UWallRunComponent* WallRunComponent;

	// �o���l�ɂ���ăX�e�[�^�X��ς���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI, meta = (AllowPrivateAccess = "true"))
	int PlayerLevel;

	int MaxPlayerLevel;
	float PreExPoints; // ���x���A�b�v�ɕK�v�Ȍo���l
	float EarnExPoints; // ���̃��x���тŊl�������o���l�@

	UPROPERTY(meta = (AllowPrivateAccess = "true")) // TODO: �o�O����B
	float PlayerAttackPower; // ���̃��x���̍U����

	class USoundBase* LevelUpSound;

	// Player�̗̑�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float Health;

	// Player�̍ő�̗̑�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USoundBase* PlayerDamagedSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	TArray<AItem*> WeaponInventory;

	const int32 INVENTORY_CAPACITY{ 4 };

public:
	// �I�[�o�[�w�b�h�����炷���߂ɃC�����C����
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool GetAiming() const { return bAiming; }

	FORCEINLINE bool GetbFiringBullet() const { return bFiringBullet; }

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	// OverlappedItemCount�̑�����bShouldTraceForItems��Update���s��
	void IncrementOverlappedItemCount(int8 Amount);
		
	void GetPickupItem(AItem* Item);

	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE bool GetCrouching() const { return bCrouching; }
	FInterpLocation GetInterpLocation(int32 Index);

	// InterpLocations�̍ł�������ItemCount��index��Ԃ�
	int32 GetInterpLocationIndex();
	void IncrementInterpLocItemCount(int32 Index, int32 Amount);

	FORCEINLINE bool ShouldPlayPickupSound() const { return bShouldPlayPickupSound; }
	FORCEINLINE bool ShouldPlayEquipSound() const { return bShouldPlayEquipSound; }
	void StartPickupSoundTimer();
	void StartEquipSoundTimer();

	UFUNCTION(BlueprintCallable, Category = "WallRunning")
	FORCEINLINE bool GetIsWallRunning() const { return WallRunComponent->GetIsWallRunning(); }

	FORCEINLINE UWallRunComponent* GetWallRunComponent() const { return WallRunComponent; }

	FORCEINLINE USoundBase* GetPlayerDamagedSound() const { return PlayerDamagedSound; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	float GetPlayerAttackPower() const
	{
		return PlayerAttackPower;
	}

	FORCEINLINE TMap<EAmmoType, int32>& GetAmmoMap() { return AmmoMap; }
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }
	void SetEquippedWeapon(AWeapon* NewWeapon);

	void SetTraceHitItem(AItem* NewTraceHitItem);
	void SetTraceHitItemLastFrame(AItem* NewTraceHitItemLastFrame);

	FORCEINLINE float GetPlayerHealth() { return Health; }
	FORCEINLINE float GetPlayerMaxHealth() { return MaxHealth; }
	void SetPlayerHealth(float RecoveryAmount);

	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }

	FORCEINLINE TArray<AItem*>& GetWeaponInventory() { return WeaponInventory; }
	FORCEINLINE int32 GetINVENTORY_CAPACITY() { return INVENTORY_CAPACITY; }
};
