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
    Sphere->SetSphereRadius(25.0f);

    Sphere->OnComponentBeginOverlap.AddDynamic(this, &AGoal::OnSphereBeginOverlap);

    // ������ԂŔ�\���ƃR���W���������ɂ���
    NiagaraComp->SetVisibility(false);
    Sphere->SetVisibility(false);
    Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // �R���W�����𖳌��ɂ���

    // �X�P�[����(2.0, 2.0, 2.0)�ɐݒ肷��
    Goal->SetRelativeScale3D(FVector(10.0f, 10.0f, 10.0f));

    SetActorLocation(FVector(0.f, 0.f, 350.0f));
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

        // PlayerController���擾����
        const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

        // InGameHUD�N���X���擾����
        AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());

        // �Q�[���I�[�o�[��ʂ�\������
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

