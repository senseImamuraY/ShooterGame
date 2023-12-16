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

	// Itemの種類ごとにプールを管理するためのマップ
	//UPROPERTY()
	TMap<UClass*, TArray<AItem*>> ItemPools;

	// プールの最大サイズ
	static const int32 MaxItems = 30;

	// 初期化関数
	UFUNCTION(BlueprintCallable, Category = "ItemPool")
	void Initialize(UWorld* World);

	// 利用可能な敵を取得
	UFUNCTION(BlueprintCallable, Category = "ItemPool")
	AItem* GetItem(UClass* ItemClass);

	// 敵をプールに返却
	UFUNCTION(BlueprintCallable, Category = "ItemPool")
	void ReturnItem(AItem* item);

private:
	void InitializeItemPool(UClass* ItemClass, int32 ItemNum);

	// Ammo9mmのBlueprintクラス
	TSubclassOf<AActor> Ammo9mmClass;

	// AmmoShellsのBlueprintクラス
	TSubclassOf<AActor> AmmoShellsClass;

	// PotionのBlueprintクラス
	TSubclassOf<AActor> PotionClass;

	AItem* SpawnItem();

	TArray<UClass*> ItemClasses;

	UWorld* WorldReference;

	// 10回に1回ShooterEnemyをスポーンさせる
	int32 SpawnCounter;

public:
	FORCEINLINE TSubclassOf<AActor> GetAmmo9mmClass() const { return Ammo9mmClass; }
	FORCEINLINE TSubclassOf<AActor> GetAmmoShellsClass() const { return AmmoShellsClass; }
	FORCEINLINE TSubclassOf<AActor> GetPotionClass() const { return PotionClass; }

	TSubclassOf<AActor> GetRandomItemClass();
};
