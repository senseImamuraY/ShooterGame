// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/PlayerActionComponents/WallRunComponent.h"
#include "../Public/Player/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Engine.h"

// Sets default values for this component's properties
UWallRunComponent::UWallRunComponent() :
	bWallRunning(false),
	bCanWallRun(true),
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

void UWallRunComponent::WallRun()
{
	if (!bWallRunning)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwner());

		if (!ShooterCharacter) return;

		FHitResult HitResult;
		const FVector LineLocationStart = ShooterCharacter->GetActorLocation();
		const FVector LineLocationEnd = LineLocationStart + ShooterCharacter->GetActorForwardVector() * 100.f;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(ShooterCharacter);

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			LineLocationStart,
			LineLocationEnd,
			ECollisionChannel::ECC_Visibility,
			Params);


		UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();

		if (bHit)
		{
			if (ShooterCharacter->GetMovementComponent()->IsFalling() == false || ShooterCharacter->GetVelocity().Size() <= 0 || bCanWallRun == false) return;

			float threshold = 0.1f;
			float dotProduct = FVector::DotProduct(HitResult.Normal, ShooterCharacter->GetActorUpVector());

			if (dotProduct < threshold)
			{
				UCharacterMovementComponent* CharMovement = Cast<UCharacterMovementComponent>(ShooterCharacter->GetMovementComponent());

				if (!CharMovement) return;

				HitWallNormal = HitResult.Normal;
				CharMovement->SetMovementMode(MOVE_Flying);
				CharMovement->bOrientRotationToMovement = false;
				CharMovement->BrakingDecelerationFlying = 10000.0f;
	
				bWallRunning = true; 

				FRotator NewRotation = FRotationMatrix::MakeFromXZ(ShooterCharacter->GetActorUpVector(), HitResult.Normal).Rotator();

				// Yawを-90度回転させる。
				NewRotation.Roll = 0.1f;
				NewRotation.Pitch = 89.9f;		

				ShooterCharacter->SetActorRotation(NewRotation);	
			}
		}
	}
	else
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwner());
		if (!ShooterCharacter) return;

		UCharacterMovementComponent* CharMovement = Cast<UCharacterMovementComponent>(ShooterCharacter->GetMovementComponent());
		if (!CharMovement) return;

		FHitResult HitResult;
		const FVector LineLocationStart = ShooterCharacter->GetActorLocation();
		const float RaycastDistance = 250.f;
		const FVector LineLocationEnd = LineLocationStart + ShooterCharacter->GetActorUpVector() * -RaycastDistance;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(ShooterCharacter);

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			LineLocationStart,
			LineLocationEnd,
			ECollisionChannel::ECC_Visibility,
			Params);

		MouseXValue = ShooterCharacter->GetInputAxisValue("Turn");
		MouseYValue = ShooterCharacter->GetInputAxisValue("LookUp");

		ShooterCharacter->bUseControllerRotationYaw = false;

		// キャラクターの回転を更新
		FQuat CharacterYawRotation(FVector::UpVector, FMath::DegreesToRadians(MouseXValue));
		ShooterCharacter->AddActorLocalRotation(CharacterYawRotation);

		UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();
		
		if (Camera) {
			FRotator CurrentCharacterRotation = ShooterCharacter->GetActorRotation();
			FVector CameraToCharacter = Camera->GetComponentLocation() - ShooterCharacter->GetActorLocation();
			FQuat CameraYawRotation(ShooterCharacter->GetActorUpVector(), FMath::DegreesToRadians(MouseXValue));
			FQuat CameraPitchRotation(ShooterCharacter->GetActorRightVector(), FMath::DegreesToRadians(MouseYValue));
			FQuat CombinedRotation = CameraYawRotation * CameraPitchRotation;

			// ピッチの値を-89から89の範囲に制限
			FRotator CombinedRotator = CombinedRotation.Rotator();
			CombinedRotation = CombinedRotator.Quaternion();

			FVector RotatedVector = CombinedRotation.RotateVector(CameraToCharacter);
			FVector NewCameraLocation = ShooterCharacter->GetActorLocation() + RotatedVector;
			Camera->SetWorldLocation(NewCameraLocation);
			Camera->AddWorldRotation(CombinedRotation);
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

			// 上方向に力を与えるベクトルを定義
			FVector LaunchVelocity(50, 0, 600);

			// キャラクターを打ち上げる
			ShooterCharacter->LaunchCharacter(LaunchVelocity, true, true);
		}
	}
}

void UWallRunComponent::EnableWallRun()
{
	bCanWallRun = true;
}

