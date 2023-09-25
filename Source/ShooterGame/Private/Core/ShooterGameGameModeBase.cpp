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
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("GetWorld() succeeded!"));
			EnemyPoolInstance->Initialize(World);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GetWorld() returned nullptr!"));
		}
	}

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


	// Set a timer to spawn an enemy every 5 seconds
	GetWorldTimerManager().SetTimer(SpawnEnemyTimerHandle, this, &AShooterGameGameModeBase::SpawnEnemy, 5.0f, true);
	GetWorldTimerManager().SetTimer(GoalTimerHandle, this, &AShooterGameGameModeBase::SpawnGoal, 60.0f, false);

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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("AEnemy::HandleEnemyDeath() is called."));
}


