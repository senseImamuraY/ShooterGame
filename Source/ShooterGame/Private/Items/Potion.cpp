// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Potion.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "../Public/Player/ShooterCharacter.h"


APotion::APotion() :
	RecoveryAmount(50.f)
{
    // AmmoはNiagaraComponentとして利用したいのでコード側で定義
	PotionNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PotionNiagara"));
    SetRootComponent(PotionNiagara);

    GetCollisionBox()->SetupAttachment(GetRootComponent());
    GetPickupWidget()->SetupAttachment(GetRootComponent());
    GetAreaSphere()->SetupAttachment(GetRootComponent());

    PotionCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PotionCollisionSphere"));
    PotionCollisionSphere->SetupAttachment(GetRootComponent());
    PotionCollisionSphere->SetSphereRadius(50.f);
}

void APotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APotion::PickupItem(AShooterCharacter* ShooterCharacter)
{
	float CurrentHealth = ShooterCharacter->GetPlayerHealth();
	float MaxHealth = ShooterCharacter->GetPlayerMaxHealth();
	if (CurrentHealth + RecoveryAmount >= MaxHealth)
	{
		ShooterCharacter->SetPlayerHealth(MaxHealth);
	}
	else
	{
		ShooterCharacter->SetPlayerHealth(CurrentHealth + RecoveryAmount);
	}

	PlayEquipSound();
    
    this->Destroy();
}

void APotion::BeginPlay()
{
    Super::BeginPlay();

    PotionCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APotion::PotionSphereOverlap);
}

void APotion::SetItemProperties(EItemState State)
{
	Super::SetItemProperties(State);

	switch (State)
	{
	case EItemState::EIS_Pickup:
		// Mesh AreaSphere CollisionBoxのプロパティを設定
		PotionNiagara->SetSimulatePhysics(true);
		PotionNiagara->SetEnableGravity(true);
		//PotionNiagara->SetSimulatePhysics(false);
		//PotionNiagara->SetEnableGravity(false);
		PotionNiagara->SetVisibility(true);
		//PotionNiagara->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		//PotionNiagara->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		PotionNiagara->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		PotionNiagara->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_WorldStatic,
			ECollisionResponse::ECR_Block);
		break;
	case EItemState::EIS_Equipped:
		PotionNiagara->SetSimulatePhysics(false);
		PotionNiagara->SetEnableGravity(false);
		PotionNiagara->SetVisibility(true);
		PotionNiagara->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		PotionNiagara->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_Falling:
		PotionNiagara->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		PotionNiagara->SetSimulatePhysics(true);
		PotionNiagara->SetEnableGravity(true);
		PotionNiagara->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		PotionNiagara->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_WorldStatic,
			ECollisionResponse::ECR_Block);
		break;
	case EItemState::EIS_EquipInterping:
		PotionNiagara->SetSimulatePhysics(false);
		PotionNiagara->SetEnableGravity(false);
		PotionNiagara->SetVisibility(true);
		PotionNiagara->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		PotionNiagara->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

void APotion::PotionSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor)
    {
        auto OverlappedCharacter = Cast<AShooterCharacter>(OtherActor);

        if (OverlappedCharacter)
        {
            StartItemCurve(OverlappedCharacter);
            PotionCollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    }
}
