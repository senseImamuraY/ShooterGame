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
    // StaticMeshComponent‚ğ’Ç‰Á‚µARootComponent‚Éİ’è‚·‚é
    Goal = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    RootComponent = Goal;

    // NiagaraComponent‚ğ’Ç‰Á‚·‚é
    UNiagaraComponent* NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraEffect"));
    NiagaraComp->SetupAttachment(RootComponent);

    // NiagaraSystem‚ğLoad‚µ‚ÄNiagaraComponent‚Éİ’è‚·‚é
    UNiagaraSystem* NiagaraSystemAsset = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/ShooterGame/Effects/Items/Goal"));
    NiagaraComp->SetAsset(NiagaraSystemAsset);
    NiagaraComp->Activate(true);

    // SphereCollision‚ğ’Ç‰Á‚·‚é
    Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    Sphere->SetupAttachment(RootComponent);

    // Radius‚ğİ’è‚·‚é
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
    // ÚG‚µ‚½Actor‚ªBallPlayer‚©”»’è‚·‚é
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

