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
    // StaticMeshComponent��ǉ����ARootComponent�ɐݒ肷��
    Goal = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    RootComponent = Goal;

    // NiagaraComponent��ǉ�����
    NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraEffect"));
    NiagaraComp->SetupAttachment(RootComponent);

    // NiagaraSystem��Load����NiagaraComponent�ɐݒ肷��
    UNiagaraSystem* NiagaraSystemAsset = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/ShooterGame/Effects/Items/Goal"));
    NiagaraComp->SetAsset(NiagaraSystemAsset);

    // SphereCollision��ǉ�����
    Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    Sphere->SetupAttachment(RootComponent);

    // Radius��ݒ肷��
    float SphereRadius = 25.f;
    Sphere->SetSphereRadius(SphereRadius);
    Sphere->OnComponentBeginOverlap.AddDynamic(this, &AGoal::OnSphereBeginOverlap);

    // ������ԂŔ�\���ƃR���W���������ɂ���
    NiagaraComp->SetVisibility(false);
    Sphere->SetVisibility(false);
    Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // �R���W�����𖳌��ɂ���

    // �X�P�[����ݒ肷��
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
    // �ڐG����Actor��BallPlayer�����肷��
    if (const AShooterCharacter* Player = Cast<AShooterCharacter>(OtherActor))
    {
        // PlayerController���擾����
        const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

        // InGameHUD�N���X���擾����
        AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

        if (ClearSound)
        {
            UAudioComponent* AudioComp = UGameplayStatics::SpawnSound2D(GetWorld(), ClearSound);
            if (AudioComp)
            {
                AudioComp->bIsUISound = true; // �ꎞ��~���ł��Đ������悤�ɐݒ�
            }
            UGameplayStatics::PlaySoundAtLocation(this, ClearSound, GetActorLocation());
        }

        // �Q�[���N���A��ʂ�\������
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
    // �R���|�[�l���g��\�����A�R���W������L���ɂ���
    NiagaraComp->SetVisibility(true);
    NiagaraComp->Activate(true);

    Sphere->SetVisibility(true);
    Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // �R���W������L���ɂ���
}

