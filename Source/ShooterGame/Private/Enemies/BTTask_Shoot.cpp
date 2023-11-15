// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemies/BTTask_Shoot.h"
#include "AIController.h"
#include "./Enemies/ShooterEnemy.h"
#include "../Public/Player/ShooterCharacter.h"

UBTTask_Shoot::UBTTask_Shoot()
{
	NodeName = TEXT("Shoot");
}

EBTNodeResult::Type UBTTask_Shoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (!OwnerComp.GetAIOwner()) 
	{
		return EBTNodeResult::Failed;
	}

	AShooterEnemy* ShooterEnemy = Cast<AShooterEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (!ShooterEnemy)
	{
		return EBTNodeResult::Failed;
	}

	// プレイヤーのアクターを取得
	AActor* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!Player) return EBTNodeResult::Failed;

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(Player);
	if (!ShooterCharacter) return EBTNodeResult::Failed;

	ShooterEnemy->DoDamage(ShooterCharacter);

	return EBTNodeResult::Succeeded;
}
