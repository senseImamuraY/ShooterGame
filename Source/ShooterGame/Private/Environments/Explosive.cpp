// Fill out your copyright notice in the Description page of Project Settings.


#include "Environments/Explosive.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "../Public/Enemies/Enemy.h"
#include "Components/SphereComponent.h"


// Sets default values
AExplosive::AExplosive() :
	Damage(100.f),
    ExplosionLaunchMagnitude(1.f),
    ExplosionDelay(0.1f),
    bAlreadyExploded(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ExplosiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplosiveMesh"));
	SetRootComponent(ExplosiveMesh);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(GetRootComponent());
}


// Called when the game starts or when spawned
void AExplosive::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExplosive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosive::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController)
{
    // ディレイを掛けると、短時間で複数レイキャストがヒットする場合があるため、boolを使用
    if (bAlreadyExploded)
    {
        return;
    }

    bAlreadyExploded = true;

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	if (ExplodeParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeParticles, HitResult.Location, FRotator(0.f), true);
	}

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Shooter, ShooterController]()
        {
            TArray<AActor*> OverlappingActors;
            GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

            for (auto Actor : OverlappingActors)
            {
                UE_LOG(LogTemp, Warning, TEXT("Actor damaged by explosive: %s"), *Actor->GetName());

                // 吹き飛ばし効果を適用
                ACharacter* Character = Cast<ACharacter>(Actor);
                if (Character)
                {
                    FVector LaunchDirection = (Character->GetActorLocation() - GetActorLocation()).GetSafeNormal() + FVector(0, 0, 1);
                    FVector Direction = (Character->GetActorLocation() - GetActorLocation());
                    FVector NormalizedDirection = Direction.GetSafeNormal();

                    LaunchDirection = LaunchDirection * ExplosionLaunchMagnitude;
                    Character->LaunchCharacter(LaunchDirection, true, true);
                }

                UGameplayStatics::ApplyDamage(
                    Actor,
                    Damage,
                    ShooterController,
                    Shooter,
                    UDamageType::StaticClass()
                );
            }

            Destroy();
        }, ExplosionDelay, false);
}

