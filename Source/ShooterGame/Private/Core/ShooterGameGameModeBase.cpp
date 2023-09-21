// Copyright Epic Games, Inc. All Rights Reserved.


#include "../Public/Core/ShooterGameGameModeBase.h"
#include "Core/InGameHUD.h"
#include "./Player/ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "../Public/Enemies/Enemy.h"
#include "../Public/Environments/Goal.h"

AShooterGameGameModeBase::AShooterGameGameModeBase() 
{
	HUDClass = AInGameHUD::StaticClass();
	PlayerControllerClass = AShooterPlayerController::StaticClass();

	static ConstructorHelpers::FObjectFinder<UClass> EnemyBlueprint(TEXT("/Game/ShooterGame/Blueprints/Enemies/BP_Enemy.BP_Enemy_C"));
	if (EnemyBlueprint.Succeeded())
	{
		EnemyClass = EnemyBlueprint.Object;
	}
}

void AShooterGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// Set a timer to spawn an enemy every 5 seconds
	GetWorldTimerManager().SetTimer(SpawnEnemyTimerHandle, this, &AShooterGameGameModeBase::SpawnEnemy, 5.0f, true);
	GetWorldTimerManager().SetTimer(GoalTimerHandle, this, &AShooterGameGameModeBase::SpawnGoal, 10.0f, false);

	// AGoal�N���X�̃C���X�^���X�𐶐�
	Goal = GetWorld()->SpawnActor<AGoal>();
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
	//// Define the spawn parameters
	//FActorSpawnParameters SpawnParams;
	//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	//// Spawn the enemy at a specific location and rotation
	//GetWorld()->SpawnActor<AEnemy>(AEnemy::StaticClass(), FVector(0, 0, 100), FRotator(0, 0, 0), SpawnParams);
	// Blueprint�̃A�Z�b�g�p�X
	
    if (EnemyClass)
    {
        // Define the spawn parameters
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		// �X�e�[�W�̒��S�ʒu
		FVector StageCenter(0.0f, 0.0f, 0.0f);

		// �X�e�[�W���~�Ɍ����Ă��Ƃ��́A���S����̋����i���a�j�B���傫�߂ɂƂ�B
		float Radius = 6000.0f;

		// �����_���Ȋp�x��0����360�x�̊ԂŐ���
		float RandomAngle = FMath::RandRange(0.0f, 360.0f);

		// �p�x���g�p����x�����y�̃I�t�Z�b�g���v�Z�B��]����ۂ�UE�̍��W�ɍ��킹��B
		float OffsetX = Radius * FMath::Sin(FMath::DegreesToRadians(RandomAngle));  
		float OffsetY = Radius * FMath::Cos(FMath::DegreesToRadians(RandomAngle));

		// �V�����X�|�[���ʒu���v�Z
		FVector SpawnLocation = StageCenter + FVector(OffsetX, OffsetY, 0.0f);

		// �G��V�����X�|�[���ʒu�ŃX�|�[��
		GetWorld()->SpawnActor<AEnemy>(EnemyClass, SpawnLocation, FRotator(0, 0, 0), SpawnParams);

        // Spawn the enemy using the Blueprint class
        //GetWorld()->SpawnActor<AEnemy>(EnemyClass, FVector(5200.f, 5200.f, 200.f), FRotator(0.f, 0.f, 0.f), SpawnParams);
    }
}

void AShooterGameGameModeBase::SpawnGoal()
{
	Goal->Spawn();
}




