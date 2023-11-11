// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Public/Environments/Goal.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "../Public/Player/ShooterCharacter.h"
#include "../Public/Core/InGameHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

// Sets default values
AGoal::AGoal()
{
    // StaticMeshComponentを追加し、RootComponentに設定する
    Goal = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    RootComponent = Goal;

    // NiagaraComponentを追加する
    NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraEffect"));
    NiagaraComp->SetupAttachment(RootComponent);

    // NiagaraSystemをLoadしてNiagaraComponentに設定する
    UNiagaraSystem* NiagaraSystemAsset = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/ShooterGame/Effects/Items/Goal"));
    NiagaraComp->SetAsset(NiagaraSystemAsset);

    // SphereCollisionを追加する
    Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    Sphere->SetupAttachment(RootComponent);

    // Radiusを設定する
    float SphereRadius = 25.f;
    Sphere->SetSphereRadius(SphereRadius);
    Sphere->OnComponentBeginOverlap.AddDynamic(this, &AGoal::OnSphereBeginOverlap);

    // 初期状態で非表示とコリジョン無効にする
    NiagaraComp->SetVisibility(false);
    Sphere->SetVisibility(false);
    Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // コリジョンを無効にする

    // スケールを設定する
    FVector GoalScale = FVector(10.0f, 10.0f, 10.0f);
    Goal->SetRelativeScale3D(GoalScale);
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
        // PlayerControllerを取得する
        const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

        // InGameHUDクラスを取得する
        AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

        if (ClearSound)
        {
            UAudioComponent* AudioComp = UGameplayStatics::SpawnSound2D(GetWorld(), ClearSound);
            if (AudioComp)
            {
                AudioComp->bIsUISound = true; // 一時停止中でも再生されるように設定
            }
            UGameplayStatics::PlaySoundAtLocation(this, ClearSound, GetActorLocation());
        }

        // ゲームクリア画面を表示する
        HUD->DispGameClear();
    }
}

// Called every frame
void AGoal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGoal::Spawn()
{
    // コンポーネントを表示し、コリジョンを有効にする
    NiagaraComp->SetVisibility(true);
    NiagaraComp->Activate(true);

    Sphere->SetVisibility(true);
    Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // コリジョンを有効にする
}

