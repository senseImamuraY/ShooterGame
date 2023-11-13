// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/ShooterEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "../Public/Player/ShooterPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "../Public/Core/ScoreSystem/ScoreCounter.h"
#include "../Public/Player/ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

AShooterEnemy::AShooterEnemy() :
	ShooterEnemyExpPoint(1000.f),
	GhostEnemyAttackPower(10.f)
{
}

void AShooterEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AShooterEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShooterEnemy::Die()
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
		IExPointsInterface::Execute_CalculateExPoints(ShooterCharacter, ShooterEnemyExpPoint);
	}

	APlayerController* MyController = GetWorld()->GetFirstPlayerController();
	AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(MyController);

	UUserWidget* Widget = PlayerController->GetHUDOverlay();
	UWidget* ChildWidget = Widget->GetWidgetFromName(TEXT("BPW_Score"));
	UScoreCounter* ScoreWidget = Cast<UScoreCounter>(ChildWidget);
	ScoreWidget->UpdateScore(ShooterEnemyExpPoint);
}

void AShooterEnemy::DoDamage(AActor* Victim)
{
}

void AShooterEnemy::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}
