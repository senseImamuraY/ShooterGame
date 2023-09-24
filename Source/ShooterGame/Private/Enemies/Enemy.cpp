// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemCOmponent.h"
#include "Components/SphereComponent.h"
#include "../Public/Player/ShooterCharacter.h"
#include "../Public/Core/InGameHUD.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AEnemy::AEnemy() :
	Health(100.f),
	MaxHealth(100.f),
	HealthBarDisplayTime(4.f)
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
	OnEnemyDead.Broadcast(this);
	HideHealthBar();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AEnemy::Die() is called."));
	}
}

void AEnemy::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
		HUD->DispGameOver();
	}
}

void AEnemy::ChasePlayer(float DeltaTime)
{
	// プレイヤーのアクターを取得
	AActor* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!Player) return;

	// プレイヤーの位置を取得
	FVector PlayerLocation = Player->GetActorLocation();

	// 敵とプレイヤーの間の方向ベクトルを計算
	FVector Direction = PlayerLocation - GetActorLocation();
	Direction.Normalize();

	// 敵を一定の速度でプレイヤーの方向に移動させる
	float Speed = 400.0f;  // これは適切な速度に調整してください
	SetActorLocation(GetActorLocation() + Direction * Speed * DeltaTime);

	// 敵がプレイヤーの方向を向くようにする
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

