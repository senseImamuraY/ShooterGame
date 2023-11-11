// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyPool.h"
#include "./Enemies/Enemy.h"
#include "Engine/Engine.h"


// Sets default values
UEnemyPool::UEnemyPool()
{
	static ConstructorHelpers::FObjectFinder<UClass> EnemyBlueprint(TEXT("/Game/ShooterGame/Blueprints/Enemies/BP_Enemy.BP_Enemy_C"));

	if (EnemyBlueprint.Succeeded())
	{
		EnemyClass = EnemyBlueprint.Object;
	}
}

void UEnemyPool::Initialize(UWorld* World)
{
	WorldReference = World;

	for (int32 i = 0; i < MaxEnemies; ++i)
	{
		// �G�𐶐�
		AEnemy* NewEnemy = RandomSpawn();

		if (NewEnemy)
		{
			NewEnemy->SetActorHiddenInGame(true); // ������Ԃł͔�\���ɂ���
			NewEnemy->SetActorEnableCollision(false); // ������Ԃł̓R���W�����𖳌��ɂ���
			NewEnemy->SetActorTickEnabled(false);
			AvailableEnemies.Add(NewEnemy);
		}
	}
}

AEnemy* UEnemyPool::GetEnemy()
{
	if (AvailableEnemies.Num() > 0)
	{
		AEnemy* EnemyToReturn = AvailableEnemies.Pop();
		EnemyToReturn->SetActorHiddenInGame(false); // �G��\��
		EnemyToReturn->SetActorEnableCollision(true); // �R���W������L���ɂ���
		EnemyToReturn->SetActorTickEnabled(true);
		EnemyToReturn->InitEnemyHealth();

		return EnemyToReturn;
	}

	return nullptr; // ���p�\�ȓG���Ȃ��ꍇ
}


void UEnemyPool::ReturnEnemy(AEnemy* enemy)
{
	if (enemy)
	{
		// ��\���ɂ��鏈����x�点�邽�߂̃^�C�}�[��ݒ�
		FTimerHandle TimerHandle;
		float DelayTime = 1.0f; // 1�b��Ɏ��s����

		enemy->SetActorEnableCollision(false); // �R���W�����𖳌��ɂ���
		enemy->SetActorTickEnabled(false);
		enemy->SetActorHiddenInGame(true); // �G���\���ɂ���
		AvailableEnemies.Push(enemy);
	}
}

FVector UEnemyPool::GetRandomLocation()
{
	// �X�e�[�W�̒��S�ʒu
	FVector StageCenter = FVector(0.0f, 0.0f, 0.0f);

	// �X�e�[�W���~�Ɍ����Ă��Ƃ��́A���S����̋����i���a�j�B���傫�߂ɂƂ�B
	float Radius = 6000.0f;

	// �����_���Ȋp�x��0����360�x�̊ԂŐ���
	float RandomAngle = FMath::RandRange(0.0f, 360.0f);

	// �p�x���g�p����x�����y�̃I�t�Z�b�g���v�Z�B��]����ۂ�UE�̍��W�ɍ��킹��B
	float OffsetX = Radius * FMath::Sin(FMath::DegreesToRadians(RandomAngle));
	float OffsetY = Radius * FMath::Cos(FMath::DegreesToRadians(RandomAngle));
	float OffsetZ = 0.f;

	// �V�����X�|�[���ʒu���v�Z
	return StageCenter + FVector(OffsetX, OffsetY, OffsetZ);
}

AEnemy* UEnemyPool::RandomSpawn()
{
	if (EnemyClass)
	{
		// Define the spawn parameters
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		// �V�����X�|�[���ʒu���v�Z
		FVector SpawnLocation = GetRandomLocation();
		FRotator SpawnRotation = FRotator(0, 0, 0);

		// �G��V�����X�|�[���ʒu�ŃX�|�[��
		return WorldReference->SpawnActor<AEnemy>(EnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
	}
	else
	{
		return nullptr;
	}
}

