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
	
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetComponentByClass(UCharacterMovementComponent::StaticClass()));

	if (MovementComponent)
	{
		MovementComponent->MovementMode = MOVE_Flying;
	}
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
	// �v���C���[�̃A�N�^�[���擾
	AActor* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!Player) return;

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(Player);
	if (!ShooterCharacter) return;

	// �v���C���[���l������o���l
	float ExPoint = 100.f;
	if (ShooterCharacter->GetClass()->ImplementsInterface(UExPointsInterface::StaticClass()))
	{
		IExPointsInterface::Execute_CalculateExPoints(ShooterCharacter, ExPoint);
	}


	APlayerController* MyController = GetWorld()->GetFirstPlayerController();
	AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(MyController);

	UUserWidget* Widget = PlayerController->GetHUDOverlay();
	UWidget* ChildWidget = Widget->GetWidgetFromName(TEXT("BPW_Score"));
	UScoreCounter* ScoreWidget = Cast<UScoreCounter>(ChildWidget);
	ScoreWidget->UpdateScore(100);

	OnEnemyDead.Broadcast(this);
	HideHealthBar();
}


void AEnemy::DoDamage(AActor* Victim)
{
	if (Victim == nullptr) return;
	AShooterCharacter* Player = Cast<AShooterCharacter>(Victim);

	// �ڐG����Actor��BallPlayer�����肷��
	if (!Player) return;

	UGameplayStatics::ApplyDamage(
		Player,
		BaseEnemyAttackPower,
		nullptr,
		this,
		UDamageType::StaticClass()
	);

	if (!Player->GetPlayerDamagedSound()) return;
	UGameplayStatics::PlaySoundAtLocation(this, Player->GetPlayerDamagedSound(), GetActorLocation());
}

void AEnemy::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DoDamage(OtherActor);
}

void AEnemy::ChasePlayer(float DeltaTime)
{
	// �v���C���[�̃A�N�^�[���擾
	AActor* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!Player) return;

	// �v���C���[�̈ʒu���擾
	FVector PlayerLocation = Player->GetActorLocation();

	// �G�ƃv���C���[�̊Ԃ̕����x�N�g�����v�Z
	FVector Direction = PlayerLocation - GetActorLocation();
	Direction.Normalize();

	// �G�����̑��x�Ńv���C���[�̕����Ɉړ�������
	float Speed = 400.0f;
	SetActorLocation(GetActorLocation() + Direction * Speed * DeltaTime);

	// �G���v���C���[�̕����������悤�ɂ���
	FRotator NewRotation = Direction.Rotation();
	SetActorRotation(NewRotation);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ChasePlayer(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::BulletHit_Implementation(FHitResult HitResult)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitResult.Location, FRotator(0.f), true);
	}

	ShowHealthBar();
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
	}

	return DamageAmount;
}

