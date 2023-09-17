// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/PlayerActionComponents/WallRunComponent.h"
#include "../Public/Player/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
UWallRunComponent::UWallRunComponent() :
	bWallRun(false),
	PreviousYaw(0.f),
	DefaultYaw(0.f),
	MouseXValue(0.f),
	MouseYValue(0.f),
	InitialCameraLocation(0.f),
	InitialCameraRotation(0.f),
	HitWallNormal(0.f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UWallRunComponent::WallRun()
{
	//GEngine->AddOnScreenDebugMessage(8, 50.f, FColor::Purple, FString::Printf(TEXT("bWallRun = %s"), bWallRun ? TEXT("true") : TEXT("false")));
	if (!bWallRun)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwner());
		if (!ShooterCharacter) return;

		FHitResult HitResult;
		const FVector LineLocationStart = ShooterCharacter->GetActorLocation();
		const FVector LineLocationEnd = LineLocationStart + ShooterCharacter->GetActorForwardVector() * 100.f;
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			LineLocationStart,
			LineLocationEnd,
			ECollisionChannel::ECC_Visibility);

		UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();
		//Camera->SetWorldRotation(FRotator(0.f, 0.f, 0.f));
		//GEngine->AddOnScreenDebugMessage(14, 50.f, FColor::Cyan, FString::Printf(TEXT("Camera Name: %s"), *Camera->GetName()));
		//GEngine->AddOnScreenDebugMessage(13, 50.f, FColor::Green, FString::Printf(TEXT("Camera Rotation: Pitch=%f, Yaw=%f, Roll=%f"), Camera->GetComponentRotation().Pitch, Camera->GetComponentRotation().Yaw, Camera->GetComponentRotation().Roll));
		
		if (bHit)
		{
			DrawDebugLine(GetWorld(), LineLocationStart, LineLocationEnd, FColor::Green, false, 1, 0, 1);

			if (ShooterCharacter->GetMovementComponent()->IsFalling() == false || ShooterCharacter->GetVelocity().Size() <= 0) return;

			float threshold = 0.1f;
			//float dotProduct = FVector::DotProduct(HitResult.Normal, FVector(0.f, 0.f, 1.f));
			float dotProduct = FVector::DotProduct(HitResult.Normal, ShooterCharacter->GetActorUpVector());
			if (dotProduct < threshold)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Dot Product: %f"), dotProduct);
				//GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Yellow, FString::Printf(TEXT("dotProduct: %f"), dotProduct));

				// 1. CharacterMovement�R���|�[�l���g���擾
				UCharacterMovementComponent* CharMovement = Cast<UCharacterMovementComponent>(ShooterCharacter->GetMovementComponent());
				if (!CharMovement) return;

				HitWallNormal = HitResult.Normal;
				// 3. MovementMode��Flying�ɐݒ�
				CharMovement->SetMovementMode(MOVE_Flying);
				CharMovement->bOrientRotationToMovement = false;
				// ��: BrakingDecelerationFlying��600.0f�ɐݒ肷��ꍇ
				CharMovement->BrakingDecelerationFlying = 10000.0f;

				bWallRun = true;
				DefaultYaw = ShooterCharacter->GetActorRotation().Yaw;
				// 4. MakeRotFromXZ���g�p���ĉ�]�𓱏o
				FRotator NewRotation = FRotationMatrix::MakeFromXZ(ShooterCharacter->GetActorUpVector(), HitResult.Normal).Rotator();
				//FRotator NewRotation = FRotationMatrix::MakeFromXZ(ShooterCharacter->GetActorUpVector(), HitResult.Normal).Rotator() * 0.99f;

				//FRotator CharacterRotationPrev = ShooterCharacter->GetActorRotation();
				//FVector CharacterLocation = ShooterCharacter->GetActorLocation();
				FRotator NewCam = NewRotation;
				//NewCam.Roll = 0.f;
				//NewCam.Yaw = 0.f;
				// Yaw��-90�x��]������
				NewRotation.Roll = 0.f;
				NewRotation.Pitch = 89.9f;		
				//NewRotation.Yaw = -90;
				//NewRotation.Pitch = 0.1f;
				// ��ʂ�NewRotation�̒l��\��
				CharMovement->RotationRate = FRotator(100.f, 100.f, 0.f); // �������s�b�`�A���[�A���[���̏��ԂȂ̂ɒ���
				
				//GEngine->AddOnScreenDebugMessage(3, 50.f, FColor::Blue, FString::Printf(TEXT("NewRotation: Pitch=%f, Yaw=%f, Roll=%f"), NewRotation.Pitch, NewRotation.Yaw, NewRotation.Roll));

				//NewCam *= -1;
				NewCam = FRotator(-90.f, 0.f, 0.f);

				ShooterCharacter->SetActorRotation(NewRotation);

				//UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();
				if (Camera)
				{
					//InitialCameraLocation = FVector(-230.f, 35.f, 100.f);
					InitialCameraLocation = FVector(-270.f, -30.f, -160.f);
					//InitialCameraLocation = Camera->GetComponentLocation() - ShooterCharacter->GetActorLocation() + FVector(-150.f, 0.f, 50.f);
					//GEngine->AddOnScreenDebugMessage(17, 50.f, FColor::Blue, FString::Printf(TEXT("InitialCameraLocation: X=%f, Y=%f, Z=%f"), InitialCameraLocation.X, InitialCameraLocation.Y, InitialCameraLocation.Z));

					InitialCameraRotation = Camera->GetComponentRotation();
					//Camera->SetRelativeRotation(NewCam);
					//Camera->SetRelativeLocation(FVector(0.f, 0.f, 0.f));

					// �L�����N�^�[�̉�]�ɍ��킹��FollowCamera�̈ʒu���X�V
					FVector NewCameraOffset = NewRotation.RotateVector(InitialCameraLocation);
					Camera->SetWorldLocation(NewCameraOffset + ShooterCharacter->GetActorLocation());

					// FindLookAtRotation���g�p���āA�J�������v���C���[���������߂̉�]���v�Z
					FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Camera->GetComponentLocation(), ShooterCharacter->GetActorLocation());
					LookAtRotation.Roll = LookAtRotation.Roll - 180.f;
					// �v�Z���ꂽ��]���J�����ɓK�p
					Camera->SetWorldRotation(LookAtRotation);

					//Camera->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f)); // �K�v�ɉ����ĉ�]�����Z�b�g�܂��͒���
					//Camera->SetRelativeRotation(NewCam); // �K�v�ɉ����ĉ�]�����Z�b�g�܂��͒���
				}				//ShooterCharacter->SetActorRotation(NewCharacterQuat.Rotator());
				// 5. SetActorRotation���g�p���ĐV������]��K�p

			}
		}
		else
		{
			DrawDebugLine(GetWorld(), LineLocationStart, LineLocationEnd, FColor::Red, false, 1, 0, 1);
		}

	}
	else
	{
		//AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwner());
		//if (!ShooterCharacter) return;
		//// 1. CharacterMovement�R���|�[�l���g���擾
		//UCharacterMovementComponent* CharMovement = Cast<UCharacterMovementComponent>(ShooterCharacter->GetMovementComponent());
		//if (!CharMovement) return;

		//FHitResult HitResult;
		//const FVector LineLocationStart = ShooterCharacter->GetActorLocation();
		//const FVector LineLocationEnd = LineLocationStart + ShooterCharacter->GetActorUpVector() * -100.f;
		//bool bHit = GetWorld()->LineTraceSingleByChannel(
		//	HitResult,
		//	LineLocationStart,
		//	LineLocationEnd,
		//	ECollisionChannel::ECC_Visibility);

		//ShooterCharacter->bUseControllerRotationYaw = false;
		////ShooterCharacter->bUseControllerRotationRoll = true;
		////ShooterCharacter->bUseControllerRotationPitch = true;

		////CharMovement->RotationRate = FRotator(0.f, 0.f, 540.f); // �������s�b�`�A���[�A���[���̏��ԂȂ̂ɒ���
		//FRotator CurrentRotation = ShooterCharacter->GetActorRotation();
		//GEngine->AddOnScreenDebugMessage(4, 50.f, FColor::Orange, FString::Printf(TEXT("CurrentRotation: Pitch=%f, Yaw=%f, Roll=%f"), CurrentRotation.Pitch, CurrentRotation.Yaw, CurrentRotation.Roll));

		//MouseXValue = ShooterCharacter->GetInputAxisValue("Turn");
		//MouseYValue = ShooterCharacter->GetInputAxisValue("LookUp");


		//// ���݂̃��[��]���擾
		////float CurrentYaw = ShooterCharacter->GetActorRotation().Yaw;
		////GEngine->AddOnScreenDebugMessage(6, 50.f, FColor::Green, FString::Printf(TEXT("CurrentYaw=%f"), CurrentYaw));
		////GEngine->AddOnScreenDebugMessage(6, 50.f, FColor::Green, FString::Printf(TEXT("CurrentRoll=%f"), ShooterCharacter->GetActorRotation().Roll));

		////// ���[��]�̕ω��ʂ��v�Z
		////float DeltaYaw = CurrentYaw - PreviousYaw;
		//CurrentRotation.Roll += MouseXValue;
		//CurrentRotation.Pitch = MouseYValue;
		//ShooterCharacter->SetActorRotation(CurrentRotation);
		////// ���[��]�̕ω��ʂ����[����]�ɓK�p
		////float NewRoll = ShooterCharacter->GetActorRotation().Roll + DeltaYaw * 10.f;
		////FRotator NewRollRotation = FRotator(ShooterCharacter->GetActorRotation().Pitch, DefaultYaw, NewRoll);
		////ShooterCharacter->SetActorRotation(NewRollRotation);

		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwner());
		if (!ShooterCharacter) return;
		// 1. CharacterMovement�R���|�[�l���g���擾
		UCharacterMovementComponent* CharMovement = Cast<UCharacterMovementComponent>(ShooterCharacter->GetMovementComponent());
		if (!CharMovement) return;

		FHitResult HitResult;
		const FVector LineLocationStart = ShooterCharacter->GetActorLocation();
		const FVector LineLocationEnd = LineLocationStart + ShooterCharacter->GetActorUpVector() * -250.f;
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			LineLocationStart,
			LineLocationEnd,
			ECollisionChannel::ECC_Visibility);

		MouseXValue = ShooterCharacter->GetInputAxisValue("Turn");
		MouseYValue = ShooterCharacter->GetInputAxisValue("LookUp");

		ShooterCharacter->bUseControllerRotationYaw = false;
		////ShooterCharacter->bUseControllerRotationRoll = true;
		////ShooterCharacter->bUseControllerRotationPitch = true;

		//////CharMovement->RotationRate = FRotator(0.f, 0.f, 540.f); // �������s�b�`�A���[�A���[���̏��ԂȂ̂ɒ���
		//FRotator CurrentRotation2 = ShooterCharacter->GetActorRotation();
		//GEngine->AddOnScreenDebugMessage(4, 50.f, FColor::Orange, FString::Printf(TEXT("CurrentRotation: Pitch=%f, Yaw=%f, Roll=%f"), CurrentRotation2.Pitch, CurrentRotation2.Yaw, CurrentRotation2.Roll));

		// �L�����N�^�[�̉�]���X�V
		FRotator CharacterDeltaRotation(0.0f, MouseXValue, 0.0f);
		ShooterCharacter->AddActorLocalRotation(CharacterDeltaRotation);

		// �J�����̉�]���X�V
		UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();
		if (Camera) {
			FRotator CameraDeltaRotation(-MouseYValue, MouseXValue, 0.0f);
			Camera->AddLocalRotation(CameraDeltaRotation);
		}


		//// 1. �L�����N�^�[�̌��݂̃N�I�[�^�j�I�����擾
		//FQuat CurrentCharacterQuat = ShooterCharacter->GetActorQuat();

		//// 2. �L�����N�^�[�p�̐V�����N�I�[�^�j�I����]���쐬
		//FQuat CharacterDeltaQuat = FQuat(FVector(0.0f, 0.0f, 1.0f), FMath::DegreesToRadians(MouseXValue));

		//// 3. �L�����N�^�[�̃N�I�[�^�j�I���ƐV�����N�I�[�^�j�I����g�ݍ��킹��
		//FQuat NewCharacterQuat = CurrentCharacterQuat * CharacterDeltaQuat;

		//// 4. ���̑g�ݍ��킹���N�I�[�^�j�I�����L�����N�^�[�̉�]�ɓK�p
		//ShooterCharacter->SetActorRotation(NewCharacterQuat.Rotator());

		//// 5. �J�����p�̐V�����N�I�[�^�j�I����]���쐬
		//// ���̗�ł́AZ���i���[�j�𒆐S��MouseYValue������]������N�I�[�^�j�I�����쐬���܂��B
		//FQuat CameraDeltaQuat = FQuat(FVector(0.0f, 0.0f, 1.0f), FMath::DegreesToRadians(MouseXValue));


		//// 2. ���͒lValue���g�p���āA�s�b�`��]�̃N�I�[�^�j�I�����v�Z
		//FQuat PitchQuat = FQuat(FVector(0.0f, 1.0f, 0.0f), FMath::DegreesToRadians(MouseYValue));

		//// 6. �J�����̌��݂̃N�I�[�^�j�I�����擾
		//UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();

		//if (Camera) {
		//	FQuat CurrentCameraQuat = Camera->GetComponentQuat();

		//	//// 7. �J�����̃N�I�[�^�j�I���ƐV�����N�I�[�^�j�I����g�ݍ��킹��
		//	//FQuat NewCameraQuat = CurrentCameraQuat * CameraDeltaQuat;

		//	//// 3. ���݂̃N�I�[�^�j�I���ƐV�����N�I�[�^�j�I����g�ݍ��킹��
		//	//FQuat NewQuat = CurrentCameraQuat * PitchQuat;
		//	 // 7. �J�����̃N�I�[�^�j�I���ƐV�����N�I�[�^�j�I����g�ݍ��킹��
		//	FQuat NewCameraQuat = CurrentCameraQuat * CameraDeltaQuat * PitchQuat;
		//	//FQuat NewCameraQuat = CurrentCameraQuat * CameraDeltaQuat * PitchQuat;


		//	// 8. ���̑g�ݍ��킹���N�I�[�^�j�I�����J�����̉�]�ɓK�p
		//	Camera->SetWorldRotation(NewCameraQuat.Rotator());

		//	// 4. �V������]���J�����ɓK�p
		//	//Camera->SetWorldRotation(NewQuat.Rotator());

		//	FVector NextCameraLocation = Camera->GetComponentLocation() - ShooterCharacter->GetActorLocation();
		//	//FRotator NextCameraRotation = Camera->GetComponentRotation();
		//	//Camera->SetRelativeRotation(NewCam);
		//	//Camera->SetRelativeLocation(FVector(0.f, 0.f, 0.f));

		//	// �L�����N�^�[�̉�]�ɍ��킹��FollowCamera�̈ʒu���X�V
		//	FVector NewNextCameraOffset = NewCameraQuat.RotateVector(NextCameraLocation);


		//	GEngine->AddOnScreenDebugMessage(15, 50.f, FColor::Red, FString::Printf(TEXT("NewNextCameraOffset: %s"), *NewNextCameraOffset.ToString()));
		//	GEngine->AddOnScreenDebugMessage(16, 50.f, FColor::Red, FString::Printf(TEXT("NextCameraLocation: %s"), *NextCameraLocation.ToString()));


		//	Camera->SetWorldLocation(NewNextCameraOffset + ShooterCharacter->GetActorLocation());


		//}
		if (!bHit)
		{

			FRotator NewRotation = FRotationMatrix::MakeFromXZ(ShooterCharacter->GetActorUpVector() * -1.f, FVector(0.f, 0.f, 1.f)).Rotator();
			//FRotator NewRotation = FRotationMatrix::MakeFromXZ(ShooterCharacter->GetActorUpVector() * -1.f, FVector(0.f, 0.f, 1.f)).Rotator() * 0.99f;

			// Yaw��-90�x��]������
			//NewRotation.Roll = 0.01f;
			// ��ʂ�NewRotation�̒l��\��
			NewRotation.Pitch = 0.1f;
			// 5. SetActorRotation���g�p���ĐV������]��K�p
			ShooterCharacter->SetActorRotation(NewRotation);
			//if (ShooterCharacter->GetMovementComponent()->IsFalling())
			{
				bWallRun = false;
				//GEngine->AddOnScreenDebugMessage(4, 50.f, FColor::Orange, FString::Printf(TEXT("DDD")));

				CharMovement->SetMovementMode(MOVE_Falling);
				CharMovement->bOrientRotationToMovement = true;

				ShooterCharacter->bUseControllerRotationYaw = true;
				ShooterCharacter->bUseControllerRotationRoll = false;
				//ShooterCharacter->bUseControllerRotationPitch = false;

				CharMovement->RotationRate = FRotator(0.f, 540.f, 0.f); // �������s�b�`�A���[�A���[���̏��ԂȂ̂ɒ���
				//UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();
				HitWallNormal = FVector(0.f, 0.f, 0.f);

				if (Camera)
				{
					//GEngine->AddOnScreenDebugMessage(12, 50.f, FColor::Red, TEXT("Setting Camera Rotation..."));
					//Camera->SetWorldRotation(InitialCameraRotation);
					Camera->SetRelativeRotation(InitialCameraRotation);
					Camera->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
					Camera->SetRelativeLocation(FVector(0.f, 0.f, 0.f));

				}
				// ������ɗ͂�^����x�N�g�����`
				FVector LaunchVelocity(30, 0, 500); // Z��������500�̗͂�^����

				// �L�����N�^�[��ł��グ��
				ShooterCharacter->LaunchCharacter(LaunchVelocity, true, true);
			}
		}
	}
}


// Called when the game starts
void UWallRunComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWallRunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

