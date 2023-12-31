// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/KrakenEnemy.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "../Public/Player/ShooterCharacter.h"
#include "Perception/AISense.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "../Public/Player/ShooterPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "../Public/Core/ScoreSystem/ScoreCounter.h"
#include "Sound/SoundCue.h"
#include "./Enemies/KrakenEnemyAIController.h"


AKrakenEnemy::AKrakenEnemy() :
    RoamAreaRadius(3000.f),
    bIsRoaming(false),
    bIsAttacking(false),
    KrakenEnemyAttackPower(40.f),
    KrakenEnemyExpPoint(1500.f)
{
}

void AKrakenEnemy::BeginPlay()
{
	Super::BeginPlay();

    //C++������KrakenEnemy���C���X�^���X������ꍇ�A�����I�ɂ��̑�����s���K�v������
    UClass* KrakenAIControllerClass = AKrakenEnemyAIController::StaticClass();

    AAIController* NewAIController = GetWorld()->SpawnActor<AAIController>(KrakenAIControllerClass);

    if (NewAIController != nullptr)
    {
        NewAIController->Possess(this);
    }
}

void AKrakenEnemy::Tick(float DeltaTime)
{
    ChacePlayer();
}

void AKrakenEnemy::Die()
{
    Super::Die();

    // �v���C���[�̃A�N�^�[���擾
    AActor* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
    if (!Player) return;

    AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(Player);
    if (!ShooterCharacter) return;

    if (ShooterCharacter->GetClass()->ImplementsInterface(UExPointsInterface::StaticClass()))
    {
        IExPointsInterface::Execute_CalculateExPoints(ShooterCharacter, KrakenEnemyExpPoint);
    }

    APlayerController* MyController = GetWorld()->GetFirstPlayerController();
    AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(MyController);

    UUserWidget* Widget = PlayerController->GetHUDOverlay();
    UWidget* ChildWidget = Widget->GetWidgetFromName(TEXT("BPW_Score"));
    UScoreCounter* ScoreWidget = Cast<UScoreCounter>(ChildWidget);
    ScoreWidget->UpdateScore(KrakenEnemyExpPoint);
    ScoreWidget->UpdateComboCount();
}

void AKrakenEnemy::PlayDeathAnimation()
{
    Super::PlayDeathAnimation();

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && DeathMontage)
    {
        AnimInstance->Montage_Play(DeathMontage);
    }

    AAIController* AIController = Cast<AAIController>(this->GetController());
    if (AIController)
    {
        AIController->StopMovement();
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

    // �Q�[���̎��Ԃ̗�����ꎞ�I�ɒ�~�i���ۂɂ�0.0001�̒l�������Ă���j
    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0f);

    // 1.5�b��Ɏ��Ԃ̗�������ɖ߂�
    FTimerHandle TimerHandle;
    float StopTime = 0.00015f; // ���ۂɂ͎��Ԃ̗���́A0�b�ł͂Ȃ��A�������l(0.0001)���ݒ肳��Ă��邽�߁A���̎��Ԃ�1.5�b�ɑ���

    // �v���C���[�̃A�N�^�[���擾
    AActor* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
    if (!Player) return;

    AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(Player);
    if (!ShooterCharacter) return;

    if (ShooterCharacter->GetCameraShakeClass() != NULL)
    {
        if (PC)
        {
            PC->ClientStartCameraShake(ShooterCharacter->GetCameraShakeClass());
        }
    }
    GetWorldTimerManager().SetTimer(TimerHandle, this, &AKrakenEnemy::ResetTimeDilation, StopTime, false);
}

void AKrakenEnemy::ResetTimeDilation()
{
    if (DeathSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
    }

    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
}

void AKrakenEnemy::DoDamage(AActor* Victim)
{
    Super::DoDamage(Victim);

    AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(Victim);
    if (!ShooterCharacter) return;
    if (ShooterCharacter->GetbIsDead()) return;

    if (Health <= 0) return;

    BlowAwayPlayer();
}

void AKrakenEnemy::Roam()
{
    if (bIsRoaming || bIsAttacking) return;
    bIsRoaming = true;
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys) return;

    FNavLocation RandomPoint;
    bool bFound = NavSys->GetRandomPointInNavigableRadius(FVector(0,0,0), RoamAreaRadius, RandomPoint);
    if (bFound)
    {
        Destination = RandomPoint.Location;
        AAIController* AIController = Cast<AAIController>(this->GetController());
        if (AIController)
        {;
            float Arrange = 2.f; // ���������ړ�����悤�ɂ��邽�߂̒���
            AIController->MoveToLocation(Destination * Arrange, RoamAreaRadius);
        }

        // �^�C�}�[��ݒ�
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AKrakenEnemy::ResetbIsRoaming, 5.0f, false);
    }
}

void AKrakenEnemy::ResetbIsRoaming()
{
    bIsRoaming = false;
}

void AKrakenEnemy::ResetbIsAttacking()
{
    bIsAttacking = false;
}

void AKrakenEnemy::ChacePlayer()
{
    if (Target)
    {
        if (bIsMoving) return;

        if (!bIsAttacking && CombatRangeSphere->IsOverlappingActor(Target))
        {
            bIsAttacking = true;
            AAIController* AIController = Cast<AAIController>(this->GetController());
            AIController->StopMovement();
            float TargetRadius = 1200.f;
            AIController->MoveToLocation(Target->GetActorLocation(), TargetRadius);

            UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
            if (AnimInstance && AttackMontage)
            {
                AnimInstance->Montage_Play(AttackMontage);

                // �^�C�}�[��ݒ�
                FTimerHandle TimerHandle;
                GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AKrakenEnemy::ResetbIsAttacking, 5.0f, false);
            }
        }
        else
        {
            bIsMoving = true;
            // �^�C�}�[��ݒ�
            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AKrakenEnemy::MoveToPlayerLocation, 2.0f, false);
        }
    }
    else
    {
        Roam();
    }
}

void AKrakenEnemy::MoveToPlayerLocation()
{
    if (!Target) return;

    AAIController* AIController = Cast<AAIController>(this->GetController());
    float TargetRadius = 1000.f;
    AIController->MoveToLocation(Target->GetActorLocation(), TargetRadius);

    bIsMoving = false;
}

void AKrakenEnemy::BlowAwayPlayer()
{
    AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(Target);
    if (!ShooterCharacter->GetPlayerDamagedSound()) return;
    UGameplayStatics::PlaySoundAtLocation(this, ShooterCharacter->GetPlayerDamagedSound(), GetActorLocation());

    UGameplayStatics::ApplyDamage(
        ShooterCharacter,
        KrakenEnemyAttackPower,
        nullptr,
        this,
        UDamageType::StaticClass()
    );

    FVector DamageDirection = ShooterCharacter->GetActorLocation() - GetActorLocation();
    DamageDirection.Normalize();

    // ������΂��͂�ݒ肷��
    FVector LaunchVelocity = DamageDirection * 1000.f + FVector::UpVector * 500.f;
    ShooterCharacter->LaunchCharacter(LaunchVelocity, true, true);
}
