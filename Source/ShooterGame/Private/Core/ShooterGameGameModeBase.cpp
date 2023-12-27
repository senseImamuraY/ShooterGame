// Copyright Epic Games, Inc. All Rights Reserved.


#include "../Public/Core/ShooterGameGameModeBase.h"
#include "Core/InGameHUD.h"
#include "./Player/ShooterPlayerController.h"
#include "./Player/ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "../Public/Enemies/Enemy.h"
#include "../Public/Enemies/EnemyPool.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "../Public/Items/ItemPool.h"
#include "../Public/Items/Item.h"
#include "../Public/Weapon/Weapon.h"


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

	// Pool�̃C���X�^���X�𐶐�
	EnemyPoolInstance = NewObject<UEnemyPool>();
	ItemPoolInstance = NewObject<UItemPool>();

	// Pool��������
	if (EnemyPoolInstance)
	{
		UWorld* World = GetWorld();

		if (World)
		{
			EnemyPoolInstance->Initialize(World);
		}
	}

	if (ItemPoolInstance)
	{
		UWorld* World = GetWorld();

		if (World)
		{
			ItemPoolInstance->Initialize(World);
		}
	}

	// �}�b�v��S�T�����āA���ꂼ���Handle��K�؂�Actor�Ƀo�C���h
	for (TActorIterator<AEnemy> It(GetWorld()); It; ++It)
	{
		AEnemy* Enemy = *It;

		if (Enemy)
		{
			Enemy->OnEnemyDead.AddDynamic(this, &AShooterGameGameModeBase::HandleEnemyDeath);
		}
	}

	for (TActorIterator<AItem> It(GetWorld()); It; ++It)
	{
		AItem* Item = *It;
		AWeapon* Weapon = Cast<AWeapon>(Item);
		if (Weapon) continue;

		if (Item)
		{
			Item->OnItemReturnRequested.AddDynamic(this, &AShooterGameGameModeBase::HandleItemReturn);
		}
	}

	float EnemySpawnInterval = 5.f;
	float TimeUntilGoalAppears = 60.f;

	GetWorldTimerManager().SetTimer(SpawnEnemyTimerHandle, this, &AShooterGameGameModeBase::SpawnEnemy, EnemySpawnInterval, true);
}

void AShooterGameGameModeBase::KillPlayer()
{
	RestartGame();
}

void AShooterGameGameModeBase::RestartGame()
{
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
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
			// �X�|�[���ʒu���]�̌v�Z
			FVector SpawnLocation = EnemyPoolInstance->GetRandomLocation();
			FRotator SpawnRotation = FRotator(0, 0, 0);
			SpawnedEnemy->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
		}
	}
}

void AShooterGameGameModeBase::HandleGameStart()
{
	AShooterCharacter* Player = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	//StartGame();

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

void AShooterGameGameModeBase::HandleItemReturn(AItem* Item)
{
	ItemPoolInstance->ReturnItem(Item);
}

void AShooterGameGameModeBase::HandleEnemyDeath(AEnemy* DeadEnemy)
{
	// ������EnemyPool��ReturnEnemy�֐����Ăяo���āADeadEnemy��ԋp����Ȃǂ̏������s��
	// �܂��A������DeadEnemy�������ꏊ�ɃA�C�e�����X�|�[������
	EnemyPoolInstance->ReturnEnemy(DeadEnemy);

	AItem* SpawnedItem = ItemPoolInstance->GetItem(ItemPoolInstance->GetRandomItemClass());
	if (SpawnedItem)
	{
		FVector EnemyLocation = DeadEnemy->GetActorLocation();
		SpawnedItem->SetActorLocation(EnemyLocation);
	}
}


