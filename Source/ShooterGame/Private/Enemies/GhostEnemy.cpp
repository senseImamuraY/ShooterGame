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

	// プレイヤーのアクターを取得
	AActor* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!Player) return;

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(Player);
	if (!ShooterCharacter) return;

	//// プレイヤーが獲得する経験値
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

	// 接触したActorがBallPlayerか判定する
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

	// 吹っ飛ばす力を設定する
	FVector LaunchVelocity = DamageDirection * 1000.f + FVector::UpVector * 500.f;
	ShooterCharacter->LaunchCharacter(LaunchVelocity, true, true);
}

void AGhostEnemy::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DoDamage(OtherActor);
}

void AGhostEnemy::ChasePlayer(float TimeDelta)
{
	// プレイヤーのアクターを取得
	AActor* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!Player) return;

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(Player);
	if (!ShooterCharacter) return;

	// プレイヤーの位置を取得
	FVector PlayerLocation = ShooterCharacter->GetActorLocation();

	// 敵とプレイヤーの間の方向ベクトルを計算
	FVector Direction = PlayerLocation - GetActorLocation();
	Direction.Normalize();

	// 敵を一定の速度でプレイヤーの方向に移動させる
	float Speed = 400.0f;
	SetActorLocation(GetActorLocation() + Direction * Speed * TimeDelta);

	// 敵がプレイヤーの方向を向くようにする
	FRotator NewRotation = Direction.Rotation();
	SetActorRotation(NewRotation);
}
