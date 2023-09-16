// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Public/Environments/Goal.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "../Public/Player/ShooterCharacter.h"


// Sets default values
AGoal::AGoal() :
    bIsGoalAchieved(false)
{
    // StaticMeshComponent��ǉ����ARootComponent�ɐݒ肷��
    Goal = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    RootComponent = Goal;

    // NiagaraComponent��ǉ�����
    UNiagaraComponent* NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraEffect"));
    NiagaraComp->SetupAttachment(RootComponent);

    // NiagaraSystem��Load����NiagaraComponent�ɐݒ肷��
    UNiagaraSystem* NiagaraSystemAsset = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/ShooterGame/Effects/Items/Goal"));
    NiagaraComp->SetAsset(NiagaraSystemAsset);
    NiagaraComp->Activate(true);

    // SphereCollision��ǉ�����
    Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    Sphere->SetupAttachment(RootComponent);

    // Radius��ݒ肷��
    Sphere->SetSphereRadius(72.0f);

    Sphere->OnComponentBeginOverlap.AddDynamic(this, &AGoal::OnSphereBeginOverlap);

}

// Called when the game starts or when spawned
void AGoal::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGoal::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // �ڐG����Actor��BallPlayer�����肷��
    if (const AShooterCharacter* Player = Cast<AShooterCharacter>(OtherActor))
    {
        UE_LOG(LogTemp, Display, TEXT("Goal"));
        bIsGoalAchieved = true;
    }
}

// Called every frame
void AGoal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

