// Copyright Epic Games, Inc. All Rights Reserved.


#include "../Public/Core/ShooterGameGameModeBase.h"
#include "Core/InGameHUD.h"
#include "./Player/ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "../Public/Enemies/Enemy.h"
#include "../Public/Environments/Goal.h"
#include "../Public/Enemies/EnemyPool.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"


AShooterGameGameModeBase::AShooterGameGameModeBase() 
{
	HUDClass = AInGameHUD::StaticClass();
	PlayerControllerClass = AShooterPlayerController::StaticClass();
}

void AShooterGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// EnemyPool�̃C���X�^���X�𐶐�
	EnemyPoolInstance = NewObject<UEnemyPool>();

	// EnemyPool��������
	if (EnemyPoolInstance)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("GetWorld() succeeded!"));
			EnemyPoolInstance->Initialize(World);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GetWorld() returned nullptr!"));
		}
	}

	// Set a timer to spawn an enemy every 5 seconds
	GetWorldTimerManager().SetTimer(SpawnEnemyTimerHandle, this, &AShooterGameGameModeBase::SpawnEnemy, 5.0f, true);
	GetWorldTimerManager().SetTimer(GoalTimerHandle, this, &AShooterGameGameModeBase::SpawnGoal, 10.0f, false);

	// AGoal�N���X�̃C���X�^���X�𐶐�
	Goal = GetWorld()->SpawnActor<AGoal>();

	// ��: AShooterGameGameModeBase��BeginPlay�֐���
	for (TActorIterator<AEnemy> It(GetWorld()); It; ++It)
	{
		AEnemy* Enemy = *It;
		if (Enemy)
		{
			// Enemy�̖��O���擾
			FString EnemyName = Enemy->GetName();

			// Enemy�̈ʒu���擾
			FVector EnemyLocation = Enemy->GetActorLocation();
			FString LocationString = FString::Printf(TEXT("X: %f, Y: %f, Z: %f"), EnemyLocation.X, EnemyLocation.Y, EnemyLocation.Z);

			// �f�o�b�O���b�Z�[�W��\��
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Enemy Name: %s, Location: %s"), *EnemyName, *LocationString));
			}

			Enemy->OnEnemyDead.AddDynamic(this, &AShooterGameGameModeBase::HandleEnemyDeath);
		}
	}

	if (GetWorldTimerManager().IsTimerActive(SpawnEnemyTimerHandle))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("SpawnEnemyTimer is active!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SpawnEnemyTimer is not active!"));
	}

}

void AShooterGameGameModeBase::KillPlayer()
{
	//	// ���݂�LevelName���擾����
	//const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());

	//// ���݂�Level���J���Ȃ���
	//UGameplayStatics::OpenLevel(GetWorld(), FName(*CurrentLevelName));
	RestartGame();
}

void AShooterGameGameModeBase::RestartGame()
{
	// PlayerController���擾����
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// InGameHUD�N���X���擾����
	AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

	// �Q�[���I�[�o�[��ʂ�\������
	HUD->DispGameOver();
}


void AShooterGameGameModeBase::SpawnEnemy()
{
	if (EnemyPoolInstance)
	{
		AEnemy* SpawnedEnemy = EnemyPoolInstance->GetEnemy();
		if (SpawnedEnemy)
		{
			// ... �X�|�[���ʒu���]�̌v�Z ...
			FVector SpawnLocation = EnemyPoolInstance->GetRandomLocation();
			// �G�̈ʒu�Ɖ�]��ݒ�
			SpawnedEnemy->SetActorLocationAndRotation(SpawnLocation, FRotator(0, 0, 0));

		}
	}
}

void AShooterGameGameModeBase::SpawnGoal()
{
	Goal->Spawn();
}

void AShooterGameGameModeBase::HandleEnemyDeath(AEnemy* DeadEnemy)
{
	// ������EnemyPool��ReturnEnemy�֐����Ăяo���āADeadEnemy��ԋp����Ȃǂ̏������s��
	EnemyPoolInstance->ReturnEnemy(DeadEnemy);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("AEnemy::HandleEnemyDeath() is called."));
}


