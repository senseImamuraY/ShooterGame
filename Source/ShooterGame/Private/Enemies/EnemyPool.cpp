// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyPool.h"
#include "./Enemies/Enemy.h"
#include "Engine/Engine.h"
#include "./Enemies/ShooterEnemy.h"
#include "../Public/Weapon/Weapon.h"

// Sets default values
UEnemyPool::UEnemyPool() :
	SpawnCounter(1)
{
	static ConstructorHelpers::FObjectFinder<UClass> GhostEnemyBlueprint(TEXT("/Game/ShooterGame/Blueprints/Enemies/BP_GhostEnemy.BP_GhostEnemy_C"));
	static ConstructorHelpers::FObjectFinder<UClass> ShooterEnemyBlueprint(TEXT("/Game/ShooterGame/Blueprints/Enemies/ShooterEnemyAI/BP_ShooterEnemy.BP_ShooterEnemy_C"));
	
	if (GhostEnemyBlueprint.Succeeded())
	{
		EnemyClasses.Add(GhostEnemyBlueprint.Object);
		GhostEnemyClass = GhostEnemyBlueprint.Object;
	}

	if (ShooterEnemyBlueprint.Succeeded())
	{
		EnemyClasses.Add(ShooterEnemyBlueprint.Object);
		ShooterEnemyClass = ShooterEnemyBlueprint.Object;
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

		AShooterEnemy* ShooterEnemy = Cast<AShooterEnemy>(EnemyToReturn);

		if (ShooterEnemy) 
		{
			ShooterEnemy->GetEquippedWeapon()->SetActorHiddenInGame(false);
		}

		return EnemyToReturn;
	}

	return nullptr; // ���p�\�ȓG���Ȃ��ꍇ
}


void UEnemyPool::ReturnEnemy(AEnemy* enemy)
{
	if (enemy)
	{
		enemy->SetActorEnableCollision(false); // �R���W�����𖳌��ɂ���
		enemy->SetActorTickEnabled(false);
		enemy->SetActorHiddenInGame(true);
		AvailableEnemies.Push(enemy);
	}
}

FVector UEnemyPool::GetRandomLocation()
{
	// �X�e�[�W�̒��S�ʒu
	FVector StageCenter = FVector(0.0f, 0.0f, 88.0f);

	// �X�e�[�W���~�Ɍ����Ă��Ƃ��́A���S����̋����i���a�j�B���傫�߂ɂƂ�B
	float Radius = 4500.0f;

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
	// 7���1��ShooterEnemy��I��
	UClass* SelectedClass = nullptr;

	if (SpawnCounter % 7 == 0 && ShooterEnemyClass != nullptr)
	{
		SelectedClass = ShooterEnemyClass;
	}
	else if (GhostEnemyClass != nullptr)
	{
		// ����ȊO�̏ꍇ��GhostEnemy��I��
		SelectedClass = GhostEnemyClass;
	}

	SpawnCounter++;

	if (SelectedClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		// �V�����X�|�[���ʒu���v�Z
		FVector SpawnLocation = GetRandomLocation();
		FRotator SpawnRotation = FRotator(0, 0, 0);

		// �G��V�����X�|�[���ʒu�ŃX�|�[��
		return WorldReference->SpawnActor<AEnemy>(SelectedClass, SpawnLocation, SpawnRotation, SpawnParams);
	}
	else
	{
		return nullptr;
	}
}

