// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyPool.generated.h"

UCLASS()
class SHOOTERGAME_API UEnemyPool : public UObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UEnemyPool();

	// ���p�\�ȓG�̃��X�g
	UPROPERTY()
	TArray<class AEnemy*> AvailableEnemies;

	// �v�[���̍ő�T�C�Y
	static const int32 MaxEnemies = 15;

	// �������֐�
	UFUNCTION(BlueprintCallable, Category = "EnemyPool")
	void Initialize(UWorld* World);

	// ���p�\�ȓG���擾
	UFUNCTION(BlueprintCallable, Category = "EnemyPool")
	AEnemy* GetEnemy();	

	// �G���v�[���ɕԋp
	UFUNCTION(BlueprintCallable, Category = "EnemyPool")
	void ReturnEnemy(AEnemy* enemy);

	// �����_���ȓG�̃X�|�[���ʒu��Ԃ�
	UFUNCTION(BlueprintCallable, Category = "EnemyPool")
	FVector GetRandomLocation();

private:
	AEnemy* RandomSpawn();

	class UClass* EnemyClass;

	class UWorld* WorldReference;
};
