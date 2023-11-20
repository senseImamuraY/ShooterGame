// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/ShooterEnemyAIController.h"
#include "kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"


AShooterEnemyAIController::AShooterEnemyAIController()
{
	// BehaviorTreeアセットをロードして割り当てる
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> AIBehavior(TEXT("/Game/ShooterGame/Blueprints/Enemies/ShooterEnemyAI/BT_ShooterEnemyAI.BT_ShooterEnemyAI"));
	if (AIBehavior.Succeeded())
	{
		ShooterEnemyAIBehavior = AIBehavior.Object;
	}
}

void AShooterEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	if (ShooterEnemyAIBehavior)
	{
		RunBehaviorTree(ShooterEnemyAIBehavior);

		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

		if (PlayerPawn)
		{
			GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
		}

		UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
		if (BlackboardComp)
		{
			APawn* ControlledPawn = GetPawn();
			if (ControlledPawn)
			{
				BlackboardComp->SetValueAsVector(TEXT("StartLocation"), ControlledPawn->GetActorLocation());
			}
		}
	}
}



void AShooterEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
