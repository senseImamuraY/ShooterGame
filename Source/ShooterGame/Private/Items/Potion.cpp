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
	PotionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PotionMesh"));
    SetRootComponent(PotionMesh);

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
    
    //this->Destroy();
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
		//PotionMesh->SetSimulatePhysics(true);
		//PotionMesh->SetEnableGravity(true);
		//PotionMesh->SetSimulatePhysics(false);
		//PotionMesh->SetEnableGravity(false);
		PotionMesh->SetVisibility(true);
		//PotionMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		//PotionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		PotionMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		PotionMesh->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_WorldStatic,
			ECollisionResponse::ECR_Block);
		PotionCollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EItemState::EIS_Equipped:
		PotionMesh->SetSimulatePhysics(false);
		PotionMesh->SetEnableGravity(false);
		PotionMesh->SetVisibility(true);
		PotionMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		PotionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_Falling:
		PotionMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		PotionMesh->SetSimulatePhysics(true);
		PotionMesh->SetEnableGravity(true);
		PotionMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		PotionMesh->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_WorldStatic,
			ECollisionResponse::ECR_Block);
		break;
	case EItemState::EIS_EquipInterping:
		PotionMesh->SetSimulatePhysics(false);
		PotionMesh->SetEnableGravity(false);
		PotionMesh->SetVisibility(true);
		PotionMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		PotionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_InPool:
		//StateString = TEXT("InPool");
		PotionMesh->SetSimulatePhysics(false);
		PotionMesh->SetEnableGravity(false);
		PotionMesh->SetVisibility(false);
		PotionMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		PotionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		PotionCollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
