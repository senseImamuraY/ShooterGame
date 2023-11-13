// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/ShooterEnemyAIController.h"
#include "kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

void AShooterEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	if (ShooterEnemyAIBehavior)
	{
		RunBehaviorTree(ShooterEnemyAIBehavior);

		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

		FTimerHandle UnusedHandle;
		GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &AShooterEnemyAIController::InitializeBlackboard, 1.0f, false);
		GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
	}
}

void AShooterEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (LineOfSightTo(PlayerPawn))
	{
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
		GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());
	}
	else
	{
		GetBlackboardComponent()->ClearValue(TEXT("PlayerLocation"));
	}
}


void AShooterEnemyAIController::InitializeBlackboard()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
	}
}
