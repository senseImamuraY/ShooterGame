// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Public/Environments/Goal.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "../Public/Player/ShooterCharacter.h"
#include "../Public/Core/InGameHUD.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AGoal::AGoal()
{
    // StaticMeshComponentを追加し、RootComponentに設定する
    Goal = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    RootComponent = Goal;

    // NiagaraComponentを追加する
    UNiagaraComponent* NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraEffect"));
    NiagaraComp->SetupAttachment(RootComponent);

    // NiagaraSystemをLoadしてNiagaraComponentに設定する
    UNiagaraSystem* NiagaraSystemAsset = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/ShooterGame/Effects/Items/Goal"));
    NiagaraComp->SetAsset(NiagaraSystemAsset);
    NiagaraComp->Activate(true);

    // SphereCollisionを追加する
    Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    Sphere->SetupAttachment(RootComponent);

    // Radiusを設定する
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
    // 接触したActorがBallPlayerか判定する
    if (const AShooterCharacter* Player = Cast<AShooterCharacter>(OtherActor))
    {
        UE_LOG(LogTemp, Display, TEXT("Goal"));

        // PlayerControllerを取得する
        const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

        // InGameHUDクラスを取得する
        AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

        // ゲームオーバー画面を表示する
        HUD->DispGameClear();
    }
}

// Called every frame
void AGoal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

