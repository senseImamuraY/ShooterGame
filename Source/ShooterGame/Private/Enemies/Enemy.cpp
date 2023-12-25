// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "../Public/Player/ShooterCharacter.h"
#include "../Public/Core/InGameHUD.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Public/Player/ShooterPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "../Public/Core/ScoreSystem/ScoreCounter.h"
#include "Components/CapsuleComponent.h"
#include "Sound/SoundCue.h"
#include "../Public/Weapon/Weapon.h"

// Sets default values
AEnemy::AEnemy() :
	Health(100.f),
	MaxHealth(100.f),
	HealthBarDisplayTime(4.f),
	BaseEnemyAttackPower(20.f),
	DamageInterval(2.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRange"));
	CombatRangeSphere->SetupAttachment(GetRootComponent());

	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnSphereBeginOverlap);
}

void AEnemy::InitEnemyHealth()
{
	Health = MaxHealth;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
}

void AEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(
		HealthBarTimer,
		this,
		&AEnemy::HideHealthBar,
		HealthBarDisplayTime);
}

void AEnemy::Die()
{	
	if (DeadParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeadParticles, GetActorLocation(), FRotator(0.f), true);
	}

	OnEnemyDead.Broadcast(this);
	HideHealthBar();
}


void AEnemy::DoDamage(AActor* Victim)
{

}

void AEnemy::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}


// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(Shooter);
	if (!ShooterCharacter) return;

	USoundCue* ImpactSound = ShooterCharacter->GetEquippedWeapon()->GetEnemyHitSound();
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	UParticleSystem* EnemyHitParticle = ShooterCharacter->GetEquippedWeapon()->GetEnemyHitParticles();
	if (EnemyHitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemyHitParticle, HitResult.Location, FRotator(0.f), true);
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Die();
	}
	else
	{
		Health -= DamageAmount;
		ShowHealthBar();
	}

	return DamageAmount;
}

