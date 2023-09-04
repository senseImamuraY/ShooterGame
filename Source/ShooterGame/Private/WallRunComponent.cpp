// Fill out your copyright notice in the Description page of Project Settings.


#include "WallRunComponent.h"
#include "../ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values for this component's properties
UWallRunComponent::UWallRunComponent() :
	bWallRun(false),
	PreviousYaw(0.f),
	DefaultYaw(0.f),
	MouseXValue(0.f),
	MouseYValue(0.f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UWallRunComponent::WallRun()
{
	GEngine->AddOnScreenDebugMessage(8, 50.f, FColor::Purple, FString::Printf(TEXT("bWallRun = %s"), bWallRun ? TEXT("true") : TEXT("false")));
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
				GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Yellow, FString::Printf(TEXT("dotProduct: %f"), dotProduct));

				// 1. CharacterMovement�R���|�[�l���g���擾
				UCharacterMovementComponent* CharMovement = Cast<UCharacterMovementComponent>(ShooterCharacter->GetMovementComponent());
				if (!CharMovement) return;

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

				// Yaw��-90�x��]������
				NewRotation.Roll = 0.f;
				NewRotation.Pitch = 89.9f;		
				//NewRotation.Yaw = -90;
				//NewRotation.Pitch = 0.1f;
				// ��ʂ�NewRotation�̒l��\��
				CharMovement->RotationRate = FRotator(100.f, 100.f, 0.f); // �������s�b�`�A���[�A���[���̏��ԂȂ̂ɒ���

				GEngine->AddOnScreenDebugMessage(3, 50.f, FColor::Blue, FString::Printf(TEXT("NewRotation: Pitch=%f, Yaw=%f, Roll=%f"), NewRotation.Pitch, NewRotation.Yaw, NewRotation.Roll));

				FQuat NewCharacterQuat = FQuat::FindBetweenVectors(ShooterCharacter->GetActorUpVector(), HitResult.Normal);
				//ShooterCharacter->SetActorRotation(NewCharacterQuat.Rotator());
				// 5. SetActorRotation���g�p���ĐV������]��K�p
				ShooterCharacter->SetActorRotation(NewRotation);

				// 1. �L�����N�^�[�̈ʒu���擾
				//FVector CharacterLocation = ShooterCharacter->GetActorLocation();
				FRotator CharacterRotation = ShooterCharacter->GetActorRotation();

				// 2. �J�����̌��݂̈ʒu���擾
				//UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();
				//if (!Camera) return;
				//FVector CameraLocation = Camera->GetComponentLocation();
				//FRotator CameraRotation = Camera->GetComponentRotation();

				// 3. �L�����N�^�[�𒆐S�Ƃ������[�J�����W�n�ɃJ�����̈ʒu��ϊ�
				//FVector LocalPosition = CameraLocation - CharacterLocation;


				//FQuat QuatCharacterRotation = CharacterRotation.Quaternion();
				//FQuat QuatCharacterRotationPrev = CharacterRotationPrev.Quaternion();
				//FQuat QuatNewCameraRotation = CameraRotation.Quaternion();

				//FQuat CharacterDelta = QuatCharacterRotation * QuatCharacterRotationPrev.Inverse();
				////FQuat CharacterDelta = CharacterRotation * CharacterRotationPrev.GetInverse();

				//FVector NewCameraPosition = CharacterLocation + CharacterDelta * LocalPosition;
				//FQuat NewCameraRotation = CharacterDelta * QuatNewCameraRotation;

				//Camera->SetWorldLocation(NewCameraPosition);

				//Camera->SetWorldRotation(NewCameraRotation.Rotator());

				// 4. �N�I�[�^�j�I�����g�p���ă��[�J�����W�n�̈ʒu����]
				FQuat RotationQuat = FQuat(NewCharacterQuat);
				//FVector RotatedLocalPosition = RotationQuat.RotateVector(LocalPosition);

				// 5. ��]��̈ʒu�����[���h���W�n�ɕϊ�
				//FVector NewCameraLocation = CharacterLocation + RotatedLocalPosition;

				// 6. �J�����̈ʒu���X�V
				//Camera->SetWorldLocation(NewCameraLocation);

				//// �J�����̌������L�����N�^�[�̕����Ɍ�����
				//FVector LookAtTarget = ShooterCharacter->GetActorLocation();
				//FQuat CameraLookAtQuat = FQuat::FindBetweenVectors(Camera->GetForwardVector(), LookAtTarget - Camera->GetComponentLocation());
				//Camera->SetWorldRotation(CameraLookAtQuat.Rotator());

		//		FQuat NewCharacterQuat = FQuat::FindBetweenVectors(ShooterCharacter->GetActorUpVector(), -HitResult.Normal);
		//		//ShooterCharacter->SetActorRotation(NewCharacterQuat.Rotator());

		//		// 2. �L�����N�^�[�̉�]����ɁA�J�����̐V�����ʒu���v�Z
		//		FVector CameraOffset = ShooterCharacter->GetCameraBoom()->SocketOffset;
		//		FVector NewCameraPosition = ShooterCharacter->GetActorLocation() + NewCharacterQuat.RotateVector(CameraOffset + FVector(0.f, 50.f, 70.f));

				UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();
				if (Camera) {
					//NewRotation *= -1;
					//Camera->SetWorldRotation(NewRotation);
					//FVector CameraOffset = ShooterCharacter->GetActorForwardVector() * -1 * 180.f; // CameraDistance�̓J�������L�����N�^�[����ǂꂾ������Ă��邩�̋����ł��B
					////Camera->SocketOffset = FVector(0.f, 50.f, 70.f);
					//Camera->SetWorldLocation(ShooterCharacter->GetActorLocation() + CameraOffset + FVector(0.f, 50.f, 70.f));
		// CameraBoom�̌������L�����N�^�[�̌����ɍ��킹��
					FVector LookAtTarget = ShooterCharacter->GetActorLocation();
					//FQuat CameraLookAtQuat = FQuat::FindBetweenVectors(Camera->GetForwardVector(), LookAtTarget - NewCameraPosition);

					//Camera->SetWorldLocation(NewCameraPosition);
					//Camera->SetWorldRotation(CameraLookAtQuat.Rotator());
			
				}
				
					 // �L�����N�^�[�̉�]��ݒ�
				//FQuat NewCharacterQuat = FQuat::FindBetweenVectors(ShooterCharacter->GetActorUpVector(), -HitResult.Normal);
				////ShooterCharacter->SetActorRotation(NewCharacterQuat.Rotator());

				//// �J�����u�[���̐ݒ�𒲐�
				//USpringArmComponent* CameraBoom = ShooterCharacter->GetCameraBoom();
				//if (CameraBoom)
				//{
				//	CameraBoom->TargetArmLength = 230.f; // �J�����ƃL�����N�^�[�̋�����ݒ�
				//	CameraBoom->SocketOffset = FVector(0.f, 35.f, 80.f); // �I�t�Z�b�g��ݒ�
				//	CameraBoom->bUsePawnControlRotation = true; // �J�����u�[�����v���C���[�̉�]��Ǐ]����悤�ɐݒ�
				//}

				//// �J�����̌������L�����N�^�[�̕����Ɍ�����
				////UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();
				//if (Camera)
				//{
				//	FVector LookAtTarget = ShooterCharacter->GetActorLocation();
				//	FQuat CameraLookAtQuat = FQuat::FindBetweenVectors(Camera->GetForwardVector(), LookAtTarget - Camera->GetComponentLocation());
				//	Camera->SetWorldRotation(CameraLookAtQuat.Rotator());
				//}
			}
			//DrawDebugLine(GetWorld(), LineLocationStart, LineLocationEnd, FColor::Green, false, 1, 0, 1);

			//if (ShooterCharacter->GetMovementComponent()->IsFalling() == false || ShooterCharacter->GetVelocity().Size() <= 0) return;

			//float threshold = 0.1f;
			////float dotProduct = FVector::DotProduct(HitResult.Normal, FVector(0.f, 0.f, 1.f));
			//float dotProduct = FVector::DotProduct(HitResult.Normal, ShooterCharacter->GetActorUpVector());
			//if (dotProduct < threshold)
			//{
			//	//UE_LOG(LogTemp, Warning, TEXT("Dot Product: %f"), dotProduct);
			//	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Yellow, FString::Printf(TEXT("dotProduct: %f"), dotProduct));

			//	// 1. CharacterMovement�R���|�[�l���g���擾
			//	UCharacterMovementComponent* CharMovement = Cast<UCharacterMovementComponent>(ShooterCharacter->GetMovementComponent());
			//	if (!CharMovement) return;

			//	// 3. MovementMode��Flying�ɐݒ�
			//	CharMovement->SetMovementMode(MOVE_Flying);
			//	CharMovement->bOrientRotationToMovement = false;
			//	// ��: BrakingDecelerationFlying��600.0f�ɐݒ肷��ꍇ
			//	CharMovement->BrakingDecelerationFlying = 10000.0f;

			//	bWallRun = true;
			//	DefaultYaw = ShooterCharacter->GetActorRotation().Yaw;
			//	// 4. MakeRotFromXZ���g�p���ĉ�]�𓱏o
			//	FRotator NewRotation = FRotationMatrix::MakeFromXZ(ShooterCharacter->GetActorUpVector(), HitResult.Normal).Rotator();
			//	//FRotator NewRotation = FRotationMatrix::MakeFromXZ(ShooterCharacter->GetActorUpVector(), HitResult.Normal).Rotator() * 0.99f;

			//	FRotator CharacterRotationPrev = ShooterCharacter->GetActorRotation();
			//	FVector CharacterLocation = ShooterCharacter->GetActorLocation();

			//	// Yaw��-90�x��]������
			//	NewRotation.Roll = 0.f;
			//	NewRotation.Pitch = 89.9f;
			//	NewRotation.Yaw = -90;
			//	//NewRotation.Pitch = 0.1f;
			//	// ��ʂ�NewRotation�̒l��\��
			//	CharMovement->RotationRate = FRotator(100.f, 100.f, 0.f); // �������s�b�`�A���[�A���[���̏��ԂȂ̂ɒ���

			//	GEngine->AddOnScreenDebugMessage(3, 50.f, FColor::Blue, FString::Printf(TEXT("NewRotation: Pitch=%f, Yaw=%f, Roll=%f"), NewRotation.Pitch, NewRotation.Yaw, NewRotation.Roll));

			//	FQuat NewCharacterQuat = FQuat::FindBetweenVectors(ShooterCharacter->GetActorUpVector(), HitResult.Normal);
			//	//ShooterCharacter->SetActorRotation(NewCharacterQuat.Rotator());
			//	// 5. SetActorRotation���g�p���ĐV������]��K�p
			//	ShooterCharacter->SetActorRotation(NewRotation);

			//	// 1. �L�����N�^�[�̈ʒu���擾
			//	//FVector CharacterLocation = ShooterCharacter->GetActorLocation();
			//	FRotator CharacterRotation = ShooterCharacter->GetActorRotation();

			//	// 2. �J�����̌��݂̈ʒu���擾
			//	UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();
			//	if (!Camera) return;
			//	FVector CameraLocation = Camera->GetComponentLocation();
			//	FRotator CameraRotation = Camera->GetComponentRotation();

			//	// 3. �L�����N�^�[�𒆐S�Ƃ������[�J�����W�n�ɃJ�����̈ʒu��ϊ�
			//	FVector LocalPosition = CameraLocation - CharacterLocation;


			//	FQuat QuatCharacterRotation = CharacterRotation.Quaternion();
			//	FQuat QuatCharacterRotationPrev = CharacterRotationPrev.Quaternion();
			//	FQuat QuatNewCameraRotation = CameraRotation.Quaternion();

			//	FQuat CharacterDelta = QuatCharacterRotation * QuatCharacterRotationPrev.Inverse();
			//	//FQuat CharacterDelta = CharacterRotation * CharacterRotationPrev.GetInverse();

			//	FVector NewCameraPosition = CharacterLocation + CharacterDelta * LocalPosition;
			//	FQuat NewCameraRotation = CharacterDelta * QuatNewCameraRotation;

			//	Camera->SetWorldLocation(NewCameraPosition);

			//	Camera->SetWorldRotation(NewCameraRotation.Rotator());
		}
		else
		{
			DrawDebugLine(GetWorld(), LineLocationStart, LineLocationEnd, FColor::Red, false, 1, 0, 1);
		}
	}
	else
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwner());
		if (!ShooterCharacter) return;
		// 1. CharacterMovement�R���|�[�l���g���擾
		UCharacterMovementComponent* CharMovement = Cast<UCharacterMovementComponent>(ShooterCharacter->GetMovementComponent());
		if (!CharMovement) return;

		FHitResult HitResult;
		const FVector LineLocationStart = ShooterCharacter->GetActorLocation();
		const FVector LineLocationEnd = LineLocationStart + ShooterCharacter->GetActorUpVector() * -100.f;
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

		////CharMovement->RotationRate = FRotator(0.f, 0.f, 540.f); // �������s�b�`�A���[�A���[���̏��ԂȂ̂ɒ���
		FRotator CurrentRotation2 = ShooterCharacter->GetActorRotation();
		GEngine->AddOnScreenDebugMessage(4, 50.f, FColor::Orange, FString::Printf(TEXT("CurrentRotation: Pitch=%f, Yaw=%f, Roll=%f"), CurrentRotation2.Pitch, CurrentRotation2.Yaw, CurrentRotation2.Roll));




		// 1. �L�����N�^�[�̌��݂̃N�I�[�^�j�I�����擾
		FQuat CurrentCharacterQuat = ShooterCharacter->GetActorQuat();

		// 2. �L�����N�^�[�p�̐V�����N�I�[�^�j�I����]���쐬
		FQuat CharacterDeltaQuat = FQuat(FVector(0.0f, 0.0f, 1.0f), FMath::DegreesToRadians(MouseXValue));

		// 3. �L�����N�^�[�̃N�I�[�^�j�I���ƐV�����N�I�[�^�j�I����g�ݍ��킹��
		FQuat NewCharacterQuat = CurrentCharacterQuat * CharacterDeltaQuat;

		// 4. ���̑g�ݍ��킹���N�I�[�^�j�I�����L�����N�^�[�̉�]�ɓK�p
		ShooterCharacter->SetActorRotation(NewCharacterQuat.Rotator());

		// 5. �J�����p�̐V�����N�I�[�^�j�I����]���쐬
		// ���̗�ł́AZ���i���[�j�𒆐S��MouseYValue������]������N�I�[�^�j�I�����쐬���܂��B
		FQuat CameraDeltaQuat = FQuat(FVector(0.0f, 0.0f, 1.0f), FMath::DegreesToRadians(MouseXValue));


		// 2. ���͒lValue���g�p���āA�s�b�`��]�̃N�I�[�^�j�I�����v�Z
		FQuat PitchQuat = FQuat(FVector(0.0f, 1.0f, 0.0f), FMath::DegreesToRadians(MouseYValue));

		// 6. �J�����̌��݂̃N�I�[�^�j�I�����擾
		UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();

		if (Camera) {
			FQuat CurrentCameraQuat = Camera->GetComponentQuat();

			//// 7. �J�����̃N�I�[�^�j�I���ƐV�����N�I�[�^�j�I����g�ݍ��킹��
			//FQuat NewCameraQuat = CurrentCameraQuat * CameraDeltaQuat;

			//// 3. ���݂̃N�I�[�^�j�I���ƐV�����N�I�[�^�j�I����g�ݍ��킹��
			//FQuat NewQuat = CurrentCameraQuat * PitchQuat;
			 // 7. �J�����̃N�I�[�^�j�I���ƐV�����N�I�[�^�j�I����g�ݍ��킹��
			FQuat NewCameraQuat = CurrentCameraQuat * CameraDeltaQuat * PitchQuat;


			// 8. ���̑g�ݍ��킹���N�I�[�^�j�I�����J�����̉�]�ɓK�p
			Camera->SetWorldRotation(NewCameraQuat.Rotator());

			// 4. �V������]���J�����ɓK�p
			//Camera->SetWorldRotation(NewQuat.Rotator());
		}


		if (!bHit)
		{

			FRotator NewRotation = FRotationMatrix::MakeFromXZ(ShooterCharacter->GetActorUpVector() * -1.f, FVector(0.f, 0.f, 1.f)).Rotator() * 0.99f;

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

