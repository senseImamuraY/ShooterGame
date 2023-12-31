// Fill out your copyright notice in the Description page of Project Settings.


#include "Environments/KillVolume.h"
#include "Components/BoxComponent.h"
#include "../Public/Player/ShooterCharacter.h"

// Sets default values
AKillVolume::AKillVolume() :
	BoxExtent(FVector(6000.f, 6000.f, 100.f))
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = DefaultSceneRoot;
	KillVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	KillVolume->SetupAttachment(RootComponent);

	KillVolume->OnComponentBeginOverlap.AddDynamic(this, &AKillVolume::OnBoxBeginOverlap);
}

void AKillVolume::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AShooterCharacter* Player = Cast<AShooterCharacter>(OtherActor))
	{
		// GameModeを取得して、InGameGameModeにCastする
		if (AShooterGameGameModeBase* GameMode = Cast<AShooterGameGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			// KillPlayerを呼び出してPlayerを破棄する
			GameMode->KillPlayer();
		}
	}
}

// Called when the game starts or when spawned
void AKillVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKillVolume::OnConstruction(const FTransform& Transform)
{
	// KillVolumeのBox Extentを変更する
	KillVolume->SetBoxExtent(BoxExtent);
}

