// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interfaces/PickupInterface.h"
#include "Item.generated.h"

class UBoxComponent;
class UWidgetComponent;
class USphereComponent;
class UCurveFloat;
class AShooterCharacter;
class USoundCue;

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Damaged UMETA(DisplayName = "Damaged"),
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_Uncommon UMETA(DisplayName = "Uncommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),

	EIR_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Pickup UMETA(DisplayName = "Pickup"),
	EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
	EIS_PickedUp UMETA(DisplayName = "PickedUp"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),
	EIS_InPool UMETA(DisplayName = "InPool"),

	EIS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Ammo UMETA(DisplayName = "Ammo"),
	EIT_Weapon UMETA(DisplayName = "Weapon"),
	EIT_GenericItem UMETA(DisplayName = "GenericItem"),

	EIT_MAX UMETA(DisplayName = "DefaultMAX")
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemReturnRequested, AItem*, Item);

UCLASS()
class SHOOTERGAME_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// AShooterCharacter::GetPickupItemで使用
	void PlayEquipSound(bool bForcePlaySound = false);

	// デリゲートの公開インスタンス
	FOnItemReturnRequested OnItemReturnRequested;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	// Rarityに基づいてActiveStarを設定
	void SetActiveStars();

	// Itemのプロパティ(State)を設定
	virtual void SetItemProperties(EItemState State);

	// 補間アニメーションが終わると呼ばれる
	void FinishInterping();

	// EquipIntreping stateのときに補間アニメーションを行う
	void ItemInterp(float DeltaTime);

	FVector GetInterpLocation();

	// 連続してWeaponをPickupしたときに、PickupSoundで鳴る音がずれてしまうので、対策としてboolを使用してタイミングをコントロールできるようにした
	void PlayPickupSound(bool bForcePlaySound = false);

protected:
	// Pickup Widgetに表示されるアイテムの名前
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FString ItemName;

	// Itemの数（ammoなど）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 ItemCount;

private:
	// Itemのスケルタルメッシュ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;

	// ItemのCollision
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionBox;

	// アイテムに表示されるPopup widget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* PickupWidget;

	// 接触しているアイテムのみをトレースできるようにする
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AreaSphere;

	// Itemのrarity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemRarity ItemRarity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TArray<bool> ActiveStars;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemState ItemState;

	// 補間に使うアセット
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ItemZCurve;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FVector ItemInterpStartLocation;

	// Item取得補間アニメーションの終点
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FVector CameraTargetLocation;
	
	// Item取得補間アニメーション中はture
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	bool bInterping;

	FTimerHandle ItemInterpTimer;

	// Item取得補間アニメーションの継続時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float ZCurveTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	AShooterCharacter* Character;

	// Item取得補間アニメーションに使用
	float ItemInterpX;
	float ItemInterpY;

	float InterpInitialYawOffset;

	// 補間中にitemをscaleする	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ItemScaleCurve;

	// アイテム取得時のサウンド
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USoundCue* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemType ItemType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 InterpLocIndex;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	UTexture2D* IconBackground;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	UTexture2D* ItemIcon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	int32 SlotIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	bool bCharacterInventoryFull;

public:
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	void SetItemState(EItemState State);
	FORCEINLINE int32 GetItemCount() const { return ItemCount; }
	virtual FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }

	FORCEINLINE USoundCue* GetPickupSound() const { return PickupSound; }
	FORCEINLINE void SetPickupSound(USoundCue* Sound) { PickupSound = Sound; }
	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }
	FORCEINLINE void SetEquipSound(USoundCue* Sound) { EquipSound = Sound; }

	FORCEINLINE void SetItemName(FString Name) { ItemName = Name; }
	FORCEINLINE void SetAmmoIcon(UTexture2D* Icon) { AmmoIcon = Icon; }
	FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }
	FORCEINLINE void SetSlotIndex(int32 Index) { SlotIndex = Index; }
	FORCEINLINE void SetCharacter(AShooterCharacter* Char) { Character = Char; }
	FORCEINLINE void SetCharacterInventoryFull(bool bFull) { bCharacterInventoryFull = bFull; }

	void StartItemCurve(AShooterCharacter* Char, bool bForcePlaySound = false);
};
