// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/GhostEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "../Public/Player/ShooterPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "../Public/Core/ScoreSystem/ScoreCounter.h"
#include "../Public/Player/ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

AGhostEnemy::AGhostEnemy() :
	GhostExpPoint(100.f),
	GhostEnemyAttackPower(40.f)
{
}

void AGhostEnemy::BeginPlay()
{
	Super::BeginPlay();

	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetComponentByClass(UCharacterMovementComponent::StaticClass()));

	if (MovementComponent)
	{
		MovementComponent->MovementMode = MOVE_Flying;
	}
}

void AGhostEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ChasePlayer(DeltaTime);
}

void AGhostEnemy::Die()
{
	Super::Die();

	// �v���C���[�̃A�N�^�[���擾
	AActor* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!Player) return;

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(Player);
	if (!ShooterCharacter) return;

	//// �v���C���[���l������o���l
	//float ExPoint = 100.f;
	if (ShooterCharacter->GetClass()->ImplementsInterface(UExPointsInterface::StaticClass()))
	{
		IExPointsInterface::Execute_CalculateExPoints(ShooterCharacter, GhostExpPoint);
	}

	APlayerController* MyController = GetWorld()->GetFirstPlayerController();
	AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(MyController);

	UUserWidget* Widget = PlayerController->GetHUDOverlay();
	UWidget* ChildWidget = Widget->GetWidgetFromName(TEXT("BPW_Score"));
	UScoreCounter* ScoreWidget = Cast<UScoreCounter>(ChildWidget);
	ScoreWidget->UpdateScore(GhostExpPoint);
}

void AGhostEnemy::DoDamage(AActor* Victim)
{
	if (Victim == nullptr) return;
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(Victim);

	// �ڐG����Actor��BallPlayer�����肷��
	if (!ShooterCharacter) return;

	UGameplayStatics::ApplyDamage(
		ShooterCharacter,
		GhostEnemyAttackPower,
		nullptr,
		this,
		UDamageType::StaticClass()
	);

	if (!ShooterCharacter->GetPlayerDamagedSound()) return;
	UGameplayStatics::PlaySoundAtLocation(this, ShooterCharacter->GetPlayerDamagedSound(), GetActorLocation());

	FVector DamageDirection = ShooterCharacter->GetActorLocation() - GetActorLocation();
	DamageDirection.Normalize();

	// ������΂��͂�ݒ肷��
	FVector LaunchVelocity = DamageDirection * 1000.f + FVector::UpVector * 500.f;
	ShooterCharacter->LaunchCharacter(LaunchVelocity, true, true);
}

void AGhostEnemy::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DoDamage(OtherActor);
}

void AGhostEnemy::ChasePlayer(float TimeDelta)
{
	// �v���C���[�̃A�N�^�[���擾
	AActor* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!Player) return;

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(Player);
	if (!ShooterCharacter) return;

	// �v���C���[�̈ʒu���擾
	FVector PlayerLocation = ShooterCharacter->GetActorLocation();

	// �G�ƃv���C���[�̊Ԃ̕����x�N�g�����v�Z
	FVector Direction = PlayerLocation - GetActorLocation();
	Direction.Normalize();

	// �G�����̑��x�Ńv���C���[�̕����Ɉړ�������
	float Speed = 400.0f;
	SetActorLocation(GetActorLocation() + Direction * Speed * TimeDelta);

	// �G���v���C���[�̕����������悤�ɂ���
	FRotator NewRotation = Direction.Rotation();
	SetActorRotation(NewRotation);
}
