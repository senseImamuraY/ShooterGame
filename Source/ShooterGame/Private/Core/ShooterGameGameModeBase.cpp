// Copyright Epic Games, Inc. All Rights Reserved.


#include "../Public/Core/ShooterGameGameModeBase.h"
#include "Core/InGameHUD.h"
#include "./Player/ShooterPlayerController.h"
#include "./Player/ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "../Public/Enemies/Enemy.h"
#include "../Public/Environments/Goal.h"
#include "../Public/Enemies/EnemyPool.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"


AShooterGameGameModeBase::AShooterGameGameModeBase() :
	StartDelaty(3.f)
{
	HUDClass = AInGameHUD::StaticClass();
	PlayerControllerClass = AShooterPlayerController::StaticClass();
	PrimaryActorTick.bCanEverTick = true;
}

void AShooterGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	HandleGameStart();

	// EnemyPool�̃C���X�^���X�𐶐�
	EnemyPoolInstance = NewObject<UEnemyPool>();

	// EnemyPool��������
	if (EnemyPoolInstance)
	{
		UWorld* World = GetWorld();

		if (World)
		{
			EnemyPoolInstance->Initialize(World);
		}
	}

	for (TActorIterator<AGoal> It(GetWorld()); It; ++It)
	{
		Goal = *It;
		if (Goal)
		{
			break;
		}
	}

	for (TActorIterator<AEnemy> It(GetWorld()); It; ++It)
	{
		AEnemy* Enemy = *It;

		if (Enemy)
		{
			Enemy->OnEnemyDead.AddDynamic(this, &AShooterGameGameModeBase::HandleEnemyDeath);
		}
	}

	float EnemySpawnInterval = 5.f;
	float TimeUntilGoalAppears = 60.f;

	// Set a timer to spawn an enemy every 5 seconds
	GetWorldTimerManager().SetTimer(SpawnEnemyTimerHandle, this, &AShooterGameGameModeBase::SpawnEnemy, EnemySpawnInterval, true);
	GetWorldTimerManager().SetTimer(GoalTimerHandle, this, &AShooterGameGameModeBase::SpawnGoal, TimeUntilGoalAppears, false);
}

void AShooterGameGameModeBase::KillPlayer()
{
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
			FRotator SpawnRotation = FRotator(0, 0, 0);
			// �G�̈ʒu�Ɖ�]��ݒ�
			SpawnedEnemy->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
		}
	}
}

void AShooterGameGameModeBase::SpawnGoal()
{
	Goal->Spawn();
}

void AShooterGameGameModeBase::HandleGameStart()
{
	AShooterCharacter* Player = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	StartGame();

	if (PlayerController)
	{
		PlayerController->SetPlayerEnabledState(false);
		
		// 3�b���EnablePlayer�֐����Ăяo��
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AShooterGameGameModeBase::EnablePlayer, StartDelaty, false);
	}
}

void AShooterGameGameModeBase::EnablePlayer()
{
	AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PlayerController)
	{
		PlayerController->SetPlayerEnabledState(true);
	}
}

void AShooterGameGameModeBase::HandleEnemyDeath(AEnemy* DeadEnemy)
{
	// ������EnemyPool��ReturnEnemy�֐����Ăяo���āADeadEnemy��ԋp����Ȃǂ̏������s��
	EnemyPoolInstance->ReturnEnemy(DeadEnemy);
}


