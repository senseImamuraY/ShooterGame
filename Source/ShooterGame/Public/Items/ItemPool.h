// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemPool.generated.h"

class AItem;
class UClass;
class UWorld;
class AEnemy;


UCLASS()
class SHOOTERGAME_API UItemPool : public UObject
{
	GENERATED_BODY()

public:
	UItemPool();

	// Item�̎�ނ��ƂɃv�[�����Ǘ����邽�߂̃}�b�v
	//UPROPERTY()
	TMap<UClass*, TArray<AItem*>> ItemPools;

	// �v�[���̍ő�T�C�Y
	static const int32 MaxItems = 30;

	// �������֐�
	UFUNCTION(BlueprintCallable, Category = "ItemPool")
	void Initialize(UWorld* World);

	// ���p�\�ȓG���擾
	UFUNCTION(BlueprintCallable, Category = "ItemPool")
	AItem* GetItem(UClass* ItemClass);

	// �G���v�[���ɕԋp
	UFUNCTION(BlueprintCallable, Category = "ItemPool")
	void ReturnItem(AItem* item);

private:
	void InitializeItemPool(UClass* ItemClass, int32 ItemNum);

	// Ammo9mm��Blueprint�N���X
	TSubclassOf<AActor> Ammo9mmClass;

	// AmmoShells��Blueprint�N���X
	TSubclassOf<AActor> AmmoShellsClass;

	// Potion��Blueprint�N���X
	TSubclassOf<AActor> PotionClass;

	AItem* SpawnItem();

	TArray<UClass*> ItemClasses;

	UWorld* WorldReference;

	// 10���1��ShooterEnemy���X�|�[��������
	int32 SpawnCounter;

public:
	FORCEINLINE TSubclassOf<AActor> GetAmmo9mmClass() const { return Ammo9mmClass; }
	FORCEINLINE TSubclassOf<AActor> GetAmmoShellsClass() const { return AmmoShellsClass; }
	FORCEINLINE TSubclassOf<AActor> GetPotionClass() const { return PotionClass; }

	TSubclassOf<AActor> GetRandomItemClass();
};
