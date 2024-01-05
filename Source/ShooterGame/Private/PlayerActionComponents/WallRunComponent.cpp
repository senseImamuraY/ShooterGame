// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/PlayerActionComponents/WallRunComponent.h"
#include "../Public/Player/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Engine.h"
#include "../Public/Weapon/Weapon.h"
#include "Components/SphereComponent.h"
#include "../Public/Interfaces/BulletHitInterface.h"


// Sets default values for this component's properties
UWallRunComponent::UWallRunComponent() :
	bWallRunning(false),
	bCanWallRun(true),
	bFirstRayCast(true),
	PreviousYaw(0.f),
	MouseXValue(0.f),
	MouseYValue(0.f),
	InitialCameraLocation(0.f),
	InitialCameraRotation(0.f),
	HitWallNormal(0.f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UWallRunComponent::BeginPlay()
{
	Super::BeginPlay();
	
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwner());

	if (!ShooterCharacter) return;

	UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();

	// �v���C���[�ƃJ�����̊Ԃ̃x�N�g���̑傫����ۑ�
	InitialCameraDistance = FVector::Dist(Camera->GetComponentLocation(), ShooterCharacter->GetActorLocation());
}


void UWallRunComponent::WallRun()
{
	if (!bWallRunning)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwner());

		if (!ShooterCharacter) return;

		// �O���ɕǂ����邩�m�F���āA�\���ɋ߂���ΕǑ���̔���E����������
		FHitResult HitResult;
		const FVector LineLocationStart = ShooterCharacter->GetActorLocation();
		float RaycastDistance = 100.f;
		const FVector LineLocationEnd = LineLocationStart + ShooterCharacter->GetActorForwardVector() * RaycastDistance;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(ShooterCharacter);
		Params.AddIgnoredActor(ShooterCharacter->GetEquippedWeapon());
		
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			LineLocationStart,
			LineLocationEnd,
			ECollisionChannel::ECC_Visibility,
			Params);

		if (bHit)
		{
			if (ShooterCharacter->GetMovementComponent()->IsFalling() == false || bCanWallRun == false) return;
			//if (ShooterCharacter->GetMovementComponent()->IsFalling() == false || ShooterCharacter->GetVelocity().Size() <= 0 || bCanWallRun == false) return;
			IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(HitResult.GetActor());
			if (BulletHitInterface) return;

			float threshold = 0.1f;
			float dotProduct = FVector::DotProduct(HitResult.Normal, ShooterCharacter->GetActorUpVector());

			if (dotProduct < threshold)
			{
				UCharacterMovementComponent* CharMovement = Cast<UCharacterMovementComponent>(ShooterCharacter->GetMovementComponent());

				if (!CharMovement) return;

				HitWallNormal = HitResult.Normal;
				CharMovement->SetMovementMode(MOVE_Flying);
				CharMovement->bOrientRotationToMovement = false;
				CharMovement->BrakingDecelerationFlying = 10000.0f; // �Ǒ��蒆�Ɋ���Ȃ��悤�ɂ���
				bWallRunning = true; 

				// �Ǒ��蒆�̓v���C���[��ǂɐ����ɂȂ�悤�ɗ�������
				FRotator NewCharacterRotation = FRotationMatrix::MakeFromXZ(ShooterCharacter->GetActorUpVector(), HitResult.Normal).Rotator();

				// Roll��0,Pitch��90���ƃW���o�����b�N(�܂��A�����������邽�߂�UE����?�Ő��l����������Ĉ��肵�Ȃ�)�ɂȂ邽�߁A�l�𒲐�
				NewCharacterRotation.Roll = 0.1f;
				NewCharacterRotation.Pitch = 89.9f;

				ShooterCharacter->SetActorRotation(NewCharacterRotation);
			}
		}
	}
	else
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwner());
		if (!ShooterCharacter) return;

		UCharacterMovementComponent* CharMovement = Cast<UCharacterMovementComponent>(ShooterCharacter->GetMovementComponent());
		if (!CharMovement) return;

		UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();

		// �Ǒ��蒆�Ɍ��݂̃v���C���[�̑����ǂɂ��Ă��邩�m�F
		FHitResult HitResult;
		const FVector LineLocationStart = ShooterCharacter->GetActorLocation();
		const float RaycastDistance = 250.f;
		const FVector LineLocationEnd = LineLocationStart + ShooterCharacter->GetActorUpVector() * -RaycastDistance;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(ShooterCharacter);
		Params.AddIgnoredActor(ShooterCharacter->GetEquippedWeapon());

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			LineLocationStart,
			LineLocationEnd,
			ECollisionChannel::ECC_Visibility,
			Params);

		MouseXValue = ShooterCharacter->GetInputAxisValue("Turn");
		MouseYValue = ShooterCharacter->GetInputAxisValue("LookUp");

		ShooterCharacter->bUseControllerRotationYaw = false;

		// �L�����N�^�[�̉�]���X�V
		FQuat CharacterYawRotation(FVector::UpVector, FMath::DegreesToRadians(MouseXValue));
		ShooterCharacter->AddActorLocalRotation(CharacterYawRotation);


		if (Camera) {
			FQuat CameraYawRotation(ShooterCharacter->GetActorUpVector(), FMath::DegreesToRadians(MouseXValue));
			FQuat CameraPitchRotation(ShooterCharacter->GetActorRightVector(), FMath::DegreesToRadians(MouseYValue));
			FQuat CombinedRotation = CameraYawRotation * CameraPitchRotation;

			FVector CharacterToCamera = Camera->GetComponentLocation() - ShooterCharacter->GetActorLocation();
			FVector RotatedCharacterToCamera = CombinedRotation.RotateVector(CharacterToCamera);
			FVector NewCameraLocation = ShooterCharacter->GetActorLocation() + RotatedCharacterToCamera;
			FVector NormalizedRotatedCharacterToCamera = RotatedCharacterToCamera.GetSafeNormal();

			// �v���C���[����J�����ւ̃��C�L���X�g���s���A�ԂɃI�u�W�F�N�g���������m�F�B
			FHitResult CharacterToCameraHitResult = FHitResult();
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(ShooterCharacter);
			CollisionParams.AddIgnoredActor(ShooterCharacter->GetEquippedWeapon());

			Camera->AddWorldRotation(CombinedRotation);

			bool bHitBlock = GetWorld()->LineTraceSingleByChannel(
				CharacterToCameraHitResult,
				ShooterCharacter->GetActorLocation(),
				ShooterCharacter->GetActorLocation() + NormalizedRotatedCharacterToCamera * InitialCameraDistance,
				ECC_Visibility, CollisionParams);

			if (bHitBlock && !bFirstRayCast)
			{
				if (CharacterToCameraHitResult.GetActor() != ShooterCharacter)
				{
					// �q�b�g�����ʒu�ɃJ�������ړ�
					FVector AdjustedCameraLocation = CharacterToCameraHitResult.ImpactPoint;
					Camera->SetWorldLocation(AdjustedCameraLocation);
				}
			}
			else if (bFirstRayCast) 
			{
				Camera->SetWorldLocation(NewCameraLocation);
				bFirstRayCast = false;
			}
			else
			{
				Camera->SetWorldLocation(NewCameraLocation);
			}
		}

		if (!bHit)
		{
			FRotator NewRotation = FRotationMatrix::MakeFromXZ(ShooterCharacter->GetActorUpVector() * -1.f, FVector(0.f, 0.f, 1.f)).Rotator();

			NewRotation.Pitch = 0.1f;
			ShooterCharacter->SetActorRotation(NewRotation);
			bWallRunning = false;

			CharMovement->SetMovementMode(MOVE_Falling);
			CharMovement->bOrientRotationToMovement = true;

			ShooterCharacter->bUseControllerRotationYaw = true;
			ShooterCharacter->bUseControllerRotationRoll = false;

			HitWallNormal = FVector(0.f, 0.f, 0.f);

			if (Camera)
			{
				Camera->SetRelativeRotation(InitialCameraRotation);
				Camera->SetRelativeLocation(InitialCameraLocation);
			}

			// �Ǒ�����I�������ɁA�v���C���[�̈ړ����X���[�Y�ɂ��邽�߂ɑł��グ��
			FVector LaunchVelocity(50, 0, 600);
			ShooterCharacter->LaunchCharacter(LaunchVelocity, true, true);

			bFirstRayCast = true;
		}
	}
}

void UWallRunComponent::EnableWallRun()
{
	bCanWallRun = true;
}
