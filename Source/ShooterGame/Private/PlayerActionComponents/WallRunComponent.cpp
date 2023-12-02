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

void UWallRunComponent::BeginPlay()
{
	Super::BeginPlay();
	

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwner());

	if (!ShooterCharacter) return;

	UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();

	// �Q�[���J�n����Ǒ���J�n���ɁA�v���C���[����J�����ւ̃x�N�g���̑傫����ۑ�
	InitialCameraDistance = FVector::Dist(Camera->GetComponentLocation(), ShooterCharacter->GetActorLocation());
	bFirstRayCast = true;
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
		Params.AddIgnoredActor(ShooterCharacter->GetEquippedWeapon());
		

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			LineLocationStart,
			LineLocationEnd,
			ECollisionChannel::ECC_Visibility,
			Params);

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
				// ����Ȃ��悤�ɂ���
				CharMovement->BrakingDecelerationFlying = 10000.0f;
	
				bWallRunning = true; 


				// �L�����N�^�[�̈ʒu�Ɖ�]���擾
				FVector CharacterLocation = ShooterCharacter->GetActorLocation();
				FRotator CharacterRotation = ShooterCharacter->GetActorRotation();

				UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();

				// �J�����̑��Έʒu�Ɖ�]���擾
				FVector CameraRelativeLocation = Camera->GetComponentLocation();
				FRotator CameraRelativeRotation = Camera->GetComponentRotation();

				// �L�����N�^�[�̈ʒu�Ɖ�]�����O�ɏo��
				GEngine->AddOnScreenDebugMessage(11, 50.f, FColor::White, FString::Printf(TEXT("Character Location: %s"), *CharacterLocation.ToString()));
				GEngine->AddOnScreenDebugMessage(12, 50.f, FColor::White, FString::Printf(TEXT("Character Rotation: %s"), *CharacterRotation.ToString()));

				// �J�����̑��Έʒu�Ɖ�]�����O�ɏo��
				GEngine->AddOnScreenDebugMessage(13, 50.f, FColor::White, FString::Printf(TEXT("Camera Relative Location: %s"), *CameraRelativeLocation.ToString()));
				GEngine->AddOnScreenDebugMessage(14, 50.f, FColor::White, FString::Printf(TEXT("Camera Relative Rotation: %s"), *CameraRelativeRotation.ToString()));


				FRotator NewRotation = FRotationMatrix::MakeFromXZ(ShooterCharacter->GetActorUpVector(), HitResult.Normal).Rotator();

				// Yaw��-90�x��]������B
				NewRotation.Roll = 0.1f;
				NewRotation.Pitch = 89.9f;		

				ShooterCharacter->SetActorRotation(NewRotation);
				
				

				//// �J�����̌��݂̉�]���擾
				//FRotator CameraRotator = Camera->GetComponentRotation();

				//// �s�b�`�̒l��-90�x����90�x�̊Ԃɐ���
				//CameraRotator.Pitch = FMath::Clamp(CameraRotator.Pitch, -90.0f, 90.0f);

				//// �V������]���J�����ɐݒ�
				//Camera->SetWorldRotation(CameraRotator);
				FRotator NewRotation2 = FRotationMatrix::MakeFromXZ(ShooterCharacter->GetActorUpVector(), HitResult.Normal).Rotator();

				Camera->AddWorldRotation(NewRotation2);
				//Camera->SetRelativeRotation(NewRotation);

				FVector RotatedVector = NewRotation2.RotateVector(Camera->GetComponentLocation() - ShooterCharacter->GetActorLocation());
				//Camera->SetRelativeLocation(RotatedVector);
				Camera->SetWorldLocation(ShooterCharacter->GetActorLocation() + RotatedVector);
				



					// �L�����N�^�[�̈ʒu�Ɖ�]�����O�ɏo��
				GEngine->AddOnScreenDebugMessage(15, 50.f, FColor::Black, FString::Printf(TEXT("Character Location: %s"), *CharacterLocation.ToString()));
				GEngine->AddOnScreenDebugMessage(16, 50.f, FColor::Black, FString::Printf(TEXT("Character Rotation: %s"), *CharacterRotation.ToString()));

				// �J�����̑��Έʒu�Ɖ�]�����O�ɏo��
				GEngine->AddOnScreenDebugMessage(17, 50.f, FColor::Black, FString::Printf(TEXT("Camera Relative Location: %s"), *CameraRelativeLocation.ToString()));
				GEngine->AddOnScreenDebugMessage(18, 50.f, FColor::Black, FString::Printf(TEXT("Camera Relative Rotation: %s"), *CameraRelativeRotation.ToString()));

			}
		}
	}
	else
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwner());
		if (!ShooterCharacter) return;

		UCharacterMovementComponent* CharMovement = Cast<UCharacterMovementComponent>(ShooterCharacter->GetMovementComponent());
		if (!CharMovement) return;



		// �L�����N�^�[�̈ʒu�Ɖ�]���擾
		FVector CharacterLocation = ShooterCharacter->GetActorLocation();
		FRotator CharacterRotation = ShooterCharacter->GetActorRotation();

		UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();

		// �J�����̑��Έʒu�Ɖ�]���擾
		FVector CameraRelativeLocation = Camera->GetComponentLocation();
		FRotator CameraRelativeRotation = Camera->GetComponentRotation();

		// �L�����N�^�[�̈ʒu�Ɖ�]�����O�ɏo��
		GEngine->AddOnScreenDebugMessage(11, 50.f, FColor::White, FString::Printf(TEXT("Character Location: %s"), *CharacterLocation.ToString()));
		GEngine->AddOnScreenDebugMessage(12, 50.f, FColor::White, FString::Printf(TEXT("Character Rotation: %s"), *CharacterRotation.ToString()));

		// �J�����̑��Έʒu�Ɖ�]�����O�ɏo��
		GEngine->AddOnScreenDebugMessage(13, 50.f, FColor::White, FString::Printf(TEXT("Camera Relative Location: %s"), *CameraRelativeLocation.ToString()));
		GEngine->AddOnScreenDebugMessage(14, 50.f, FColor::White, FString::Printf(TEXT("Camera Relative Rotation: %s"), *CameraRelativeRotation.ToString()));



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

		FRotator PreviousRotation = ShooterCharacter->GetActorRotation();


		// �L�����N�^�[�̉�]���X�V
		FQuat CharacterYawRotation(FVector::UpVector, FMath::DegreesToRadians(MouseXValue));
		ShooterCharacter->AddActorLocalRotation(CharacterYawRotation);

		//UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();
		
		if (Camera) {
			FRotator CurrentCharacterRotation = ShooterCharacter->GetActorRotation();
			FVector CharacterToCamera = Camera->GetComponentLocation() - ShooterCharacter->GetActorLocation();
			FQuat CameraYawRotation(ShooterCharacter->GetActorUpVector(), FMath::DegreesToRadians(MouseXValue));
			FQuat CameraPitchRotation(ShooterCharacter->GetActorRightVector(), FMath::DegreesToRadians(MouseYValue));
			FQuat CombinedRotation = CameraYawRotation * CameraPitchRotation;
		

			FRotator CombinedRotator = CombinedRotation.Rotator();
			CombinedRotation = CombinedRotator.Quaternion();

			FVector RotatedVector = CombinedRotation.RotateVector(CharacterToCamera);
			FVector NewCameraLocation = ShooterCharacter->GetActorLocation() + RotatedVector;
			//FVector NewCameraLocation = ShooterCharacter->GetActorLocation() + RotatedVector;
			//Camera->SetWorldLocation(CombinedRotation.RotateVector(Camera->GetComponentLocation()));

			FVector HitObjectNormal = HitResult.Normal;
			FVector NormalizedCharacterToCamera = CharacterToCamera.GetSafeNormal();
			FVector NormalizedNewCameraLocation = NewCameraLocation.GetSafeNormal();
			//FVector TraceHitPointToCamera2 = NormalizedNewCameraLocation - HitResult.ImpactPoint;

			GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString::Printf(TEXT("NormalizedNewCameraLocation: %s"), *NormalizedNewCameraLocation.ToString()));
			GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Blue, FString::Printf(TEXT("HitObjectNormal: %s"), *HitObjectNormal.ToString()));

			// ���K�����ꂽ�x�N�g�����擾 ���Ȃ�������K�v�Ȃ��B
			//FVector NormalizedHitObjectNormal = HitObjectNormal;
			//FVector NormalizedHitObjectNormal = HitObjectNormal + HitResult.ImpactPoint;
			GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Green, FString::Printf(TEXT("ImpactPoint: %s"), *HitResult.ImpactPoint.ToString()));

			FVector NormalizedRotatedCharacterToCamera = RotatedVector.GetSafeNormal();
			GEngine->AddOnScreenDebugMessage(4, 5.f, FColor::Orange, FString::Printf(TEXT("NormalizedTraceHitPointToCamera: %s"), *NormalizedRotatedCharacterToCamera.ToString()));
			GEngine->AddOnScreenDebugMessage(4, 5.f, FColor::Orange, FString::Printf(TEXT("NormalizedNewCameraLocation: %s"), *NormalizedNewCameraLocation.ToString()));
			GEngine->AddOnScreenDebugMessage(6, 5.f, FColor::Orange, FString::Printf(TEXT("RotatedVector: %s"), *RotatedVector.ToString()));

			//// �h�b�g�ς��v�Z
			//float DotProduct = FVector::DotProduct(NormalizedHitObjectNormal, NormalizedTraceHitPointToCamera);
			//GEngine->AddOnScreenDebugMessage(7, 5.f, FColor::Cyan, FString::Printf(TEXT("DotProduct: %f"), DotProduct));

			//// �p�x�i���W�A���j���v�Z
			//float AngleRadians = acosf(DotProduct);
			//GEngine->AddOnScreenDebugMessage(8, 5.f, FColor::Cyan, FString::Printf(TEXT("AngleRadians: %f"), AngleRadians));	

			//float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);
			//// �p�x��x�ɕϊ�
			//GEngine->AddOnScreenDebugMessage(5, 5.f, FColor::Cyan, FString::Printf(TEXT("AngleDegrees: %f"), AngleDegrees));
			// �L�����N�^�[�̑O���x�N�g�����擾
			FVector ForwardVector = ShooterCharacter->GetActorForwardVector();

			// �������̃x�N�g�����v�Z
			FVector BackwardVector = -ForwardVector;


			// �h�b�g�ς��v�Z
			float DotProduct = FVector::DotProduct(BackwardVector, NormalizedRotatedCharacterToCamera);
			GEngine->AddOnScreenDebugMessage(7, 5.f, FColor::Cyan, FString::Printf(TEXT("DotProduct: %f"), DotProduct));

			// �O�ς��v�Z
			FVector CrossProduct = FVector::CrossProduct(BackwardVector, NormalizedRotatedCharacterToCamera);

			// �p�x�i���W�A���j���v�Z
			float AngleRadians = acosf(DotProduct);
			GEngine->AddOnScreenDebugMessage(8, 5.f, FColor::Cyan, FString::Printf(TEXT("AngleRadians: %f"), AngleRadians));

			//float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);
			//// �p�x��x�ɕϊ�
			//GEngine->AddOnScreenDebugMessage(9, 5.f, FColor::Cyan, FString::Printf(TEXT("AngleDegrees: %f"), AngleDegrees));

			// �p�x��x�ɕϊ�
			float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);

			// �O�ς�Z�����Ɋ�Â��Ċp�x�ɕ�����t����
			if (CrossProduct.Z < 0)
			{
				AngleDegrees = -AngleDegrees;
			}
			GEngine->AddOnScreenDebugMessage(9, 5.f, FColor::Cyan, FString::Printf(TEXT("AngleDegrees: %f"), AngleDegrees));


			FHitResult CharacterToCameraHitResult = FHitResult();
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(ShooterCharacter);
			CollisionParams.AddIgnoredActor(ShooterCharacter->GetEquippedWeapon());

			//if (AngleDegrees <= 90 && AngleDegrees >= -90)
			{

				Camera->AddWorldRotation(CombinedRotation);

				bool hitBlock = GetWorld()->LineTraceSingleByChannel(
					CharacterToCameraHitResult,
					ShooterCharacter->GetActorLocation(),
					ShooterCharacter->GetActorLocation() + NormalizedRotatedCharacterToCamera * InitialCameraDistance,
					ECC_Visibility, CollisionParams);
				// �v���C���[����J�����ւ̃��C�L���X�g
				if (hitBlock && !bFirstRayCast)
				{
					if (CharacterToCameraHitResult.GetActor() != ShooterCharacter)
					{
						// �q�b�g�����ʒu�ɃJ�������ړ�
						FVector NewCameraLocation2 = CharacterToCameraHitResult.ImpactPoint;
						Camera->SetWorldLocation(NewCameraLocation2);
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
				// �f�o�b�O���C���̐F��ݒ�
				FColor LineColor = hitBlock ? FColor::Green : FColor::Red;
				// �f�o�b�O���C���̕`��
				DrawDebugLine(
					GetWorld(),
					ShooterCharacter->GetActorLocation(),
					ShooterCharacter->GetActorLocation() + NormalizedRotatedCharacterToCamera * InitialCameraDistance,
					LineColor,
					true, // �i���I�ȃ��C���ł͂Ȃ�
					5.0f, // 5�b�ԕ\��
					0, // �f�v�X�D��x
					1.0f // ���C���̑���
				);
			}
			//else
			//{

			//	// �����𖞂����Ă��Ȃ��ꍇ�A���̉�]�ɖ߂�
			//	ShooterCharacter->SetActorRotation(PreviousRotation);
			//	
			//	//Camera->SetWorldLocation(ShooterCharacter->GetActorLocation() + (ShooterCharacter->GetActorUpVector() * InitialCameraDistance));

			//	float ExcessAngle;
			//	// �v���X�}�C�i�X90�x�𒴂����������p�x�𒲐�
			//	if (AngleDegrees >= 0)
			//	{
			//		ExcessAngle = AngleDegrees - 90.f;
			//	}
			//	else
			//	{
			//		ExcessAngle = AngleDegrees + 90.f;
			//	}

			//	//ExcessAngle *= -1;
			//	
			//	//FRotator CurrentCharacterRotation = ShooterCharacter->GetActorRotation();
			//	//FVector CharacterToCamera = Camera->GetComponentLocation() - ShooterCharacter->GetActorLocation();
			//	//FQuat CameraYawRotation(ShooterCharacter->GetActorUpVector(), FMath::DegreesToRadians(MouseXValue));
			//	//FQuat CameraPitchRotation(ShooterCharacter->GetActorRightVector(), FMath::DegreesToRadians(MouseYValue));
			//	//FQuat CombinedRotation = CameraYawRotation * CameraPitchRotation;


			//	//FRotator CombinedRotator = CombinedRotation.Rotator();
			//	//CombinedRotation = CombinedRotator.Quaternion();
			//	FVector RotationAxis;
			//	if (AngleDegrees >= 0)
			//	{
			//		RotationAxis = FVector::CrossProduct(NormalizedRotatedCharacterToCamera, BackwardVector);
			//	}
			//	else
			//	{
			//		RotationAxis = FVector::CrossProduct(BackwardVector, NormalizedRotatedCharacterToCamera);
			//	}

			//	//FVector RotationAxis = FVector::CrossProduct(NormalizedRotatedCharacterToCamera, BackwardVector);

			//	// ��]���Ɗp�x����N�I�[�^�j�I�����쐬
			//	FQuat RotationQuat = FQuat(RotationAxis, ExcessAngle);

			//	//FRotator ExcessRotator = RotationQuat.Rotator();
			//	//RotationQuat = ExcessRotator.Quaternion();

			//	Camera->AddWorldRotation(RotationQuat);

			//	// �N�I�[�^�j�I�����g�p���ăx�N�g��A����]
			//	FVector RotatedVectorA = RotationQuat.RotateVector(NormalizedRotatedCharacterToCamera);

			//	// �V�����ʒu�܂��͕������v�Z
			//	FVector NewPositionOrDirection = RotatedVectorA;

			//	Camera->SetWorldLocation(ShooterCharacter->GetActorLocation() + (NewPositionOrDirection * InitialCameraDistance));

			//	//FRotator AdjustedRotation = ShooterCharacter->GetActorRotation() - FRotator(0.0f, 0.0f, ExcessAngle);
			//	////ShooterCharacter->SetActorRotation(AdjustedRotation);

			//	//FVector AdjustedRotateVector = AdjustedRotation.RotateVector(ShooterCharacter->GetActorUpVector() - CharacterToCamera);
			//	//FVector AdjustedCameraLocation = ShooterCharacter->GetActorLocation() + AdjustedRotateVector;
			//	//Camera->SetWorldLocation(ShooterCharacter->GetActorUpVector);
			//}
		}

		if (!bHit)
		{


			// �L�����N�^�[�̈ʒu�Ɖ�]�����O�ɏo��
			GEngine->AddOnScreenDebugMessage(15, 50.f, FColor::Black, FString::Printf(TEXT("Character Location: %s"), *CharacterLocation.ToString()));
			GEngine->AddOnScreenDebugMessage(16, 50.f, FColor::Black, FString::Printf(TEXT("Character Rotation: %s"), *CharacterRotation.ToString()));

			// �J�����̑��Έʒu�Ɖ�]�����O�ɏo��
			GEngine->AddOnScreenDebugMessage(17, 50.f, FColor::Black, FString::Printf(TEXT("Camera Relative Location: %s"), *CameraRelativeLocation.ToString()));
			GEngine->AddOnScreenDebugMessage(18, 50.f, FColor::Black, FString::Printf(TEXT("Camera Relative Rotation: %s"), *CameraRelativeRotation.ToString()));

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
				//Camera->SetRelativeRotation(InitialCameraRotation);
				//Camera->SetRelativeLocation(InitialCameraLocation);
			}

			// ������ɗ͂�^����x�N�g�����`
			FVector LaunchVelocity(50, 0, 600);

			// �L�����N�^�[��ł��グ��
			ShooterCharacter->LaunchCharacter(LaunchVelocity, true, true);

			bFirstRayCast = true;
		}



	}
}

void UWallRunComponent::EnableWallRun()
{
	bCanWallRun = true;
}


//			//// �J�����ƕǂ̖@���x�N�g���Ƃ̊Ԃ̊p�x���v�Z
//			//	float angle = FMath::Acos(FVector::DotProduct(HitObjectNormal, TraceHitPointToCamera.GetSafeNormal())) * FMath::RadiansToDegrees;
//
//				// �p�x�Ɋ�Â��ċ����𒲐�
//// �p�x�Ɋ�Â��ċ����𒲐�
//				float distanceModifier = FMath::Clamp((180.0f - AngleDegrees) / 90.0f, 0.0f, 1.0f);
//				//FVector MaxDistance = CharacterToCamera;
//				FVector MaxDistance = CharacterToCamera.GetSafeNormal() * InitialCameraDistance;
//				FVector MinDistance = (CharacterToCamera.GetSafeNormal() * InitialCameraDistance) * 0.2;
//				////FVector MinDistance = TraceHitPointToCamera;
//				////FVector MinDistance = TraceHitPointToCamera * 0.95f;
//				//FVector newDistance = FMath::Lerp(MaxDistance, MinDistance, distanceModifier);
//				float MaxDistanceLength = MaxDistance.Size();
//				float MinDistanceLength = MinDistance.Size();
//
//				// �����̒�������
//				float newDistanceLength = FMath::Lerp(MaxDistanceLength, MinDistanceLength, distanceModifier);
//
//				// �J�����̐V�����ʒu���v�Z
//				//FVector newCameraLocation = ShooterCharacter->GetActorLocation() + CharacterToCamera.GetSafeNormal() * newDistanceLength;
//				//FVector newCameraLocation = ShooterCharacter->GetActorLocation() + CharacterToCamera.GetSafeNormal() * InitialCameraDistance;
//


//
//			// �p�x��90�x�ȏォ�ǂ������`�F�b�N
//if (AngleDegrees >= 90.0f)
//{
//	//float OrthographicProjectionTheta = AngleDegrees - 90.f;
//	//float Cosine = FMath::Cos(AngleRadians);
//
//	//FVector RadiusVector = HitObjectNormal * InitialCameraDistance;
//	//// �v���C���[��Right�x�N�g�����擾
//	//FVector PlayerRightVector = ShooterCharacter->GetActorRightVector();
//
//	//// 90�x��]��\���l�������쐬�i���W�A���ɕϊ��j
//	//FQuat RotationQuat = FQuat(PlayerRightVector, FMath::DegreesToRadians(90.0f));
//
//	//// RadiusVector����]
//	//FVector RotatedRadiusVector = RotationQuat.RotateVector(RadiusVector);
//	//FVector TraceHitPointToCamera = NewCameraLocation - HitResult.ImpactPoint;
//
//	//float OrthographicProjectionDotProduct = FVector::DotProduct(RotatedRadiusVector, TraceHitPointToCamera);
//
//	//float SizeSquaredOfRotatedRadiusVector = RotatedRadiusVector.SizeSquared();
//
//	//FVector P = OrthographicProjectionDotProduct / SizeSquaredOfRotatedRadiusVector * RotatedRadiusVector;
//
//	////float OrthographicProjectionDotProduct = FVector::DotProduct(NormalizedHitObjectNormal, NormalizedTraceHitPointToCamera);
//
//
////				// �J�����̈ʒu���X�V
//				//Camera->SetWorldLocation(P + HitResult.ImpactPoint);
//	Camera->SetWorldLocation(NewCameraLocation);
//	Camera->AddWorldRotation(CombinedRotation);
//}

		//
			//if (AngleDegrees >= 110 || AngleDegrees <= -110) 
			//{
			//	float OrthographicProjectionTheta = AngleDegrees - 90.f;
			//	float Cosine = FMath::Cos(OrthographicProjectionTheta);
			//	
			//	FVector RadiusVector = HitObjectNormal * InitialCameraDistance;
			//	// �v���C���[��Right�x�N�g�����擾
			//	FVector PlayerRightVector = ShooterCharacter->GetActorRightVector();
			//	
			//	// 90�x��]��\���l�������쐬�i���W�A���ɕϊ��j
			//	FQuat RotationQuat = FQuat(PlayerRightVector, FMath::DegreesToRadians(90.0f));
			//	
			//	// RadiusVector����]
			//	FVector RotatedRadiusVector = RotationQuat.RotateVector(RadiusVector);
			//	FVector TraceHitPointToCamera = NewCameraLocation - HitResult.ImpactPoint;
			//	
			//	float OrthographicProjectionDotProduct = FVector::DotProduct(RotatedRadiusVector, TraceHitPointToCamera);
			//	
			//	float SizeSquaredOfRotatedRadiusVector = RotatedRadiusVector.SizeSquared();
			//	
			//	FVector P = OrthographicProjectionDotProduct / SizeSquaredOfRotatedRadiusVector * RotatedRadiusVector;
			//	
			//	//float OrthographicProjectionDotProduct = FVector::DotProduct(NormalizedHitObjectNormal, NormalizedTraceHitPointToCamera);
			//	
			//	if (AngleDegrees >= 120 || AngleDegrees <= -120) 
			//	{
			//		//Camera->SetRelativeRotation(InitialCameraRotation);
			//		//Camera->SetRelativeLocation(InitialCameraLocation);
			//	}
			//	else
			//	{
			//		// �J�����̈ʒu���X�V
			//		//Camera->SetRelativeLocation(P);
			//		Camera->SetWorldLocation(P + HitResult.ImpactPoint);
			//		//Camera->SetWorldLocation(NewCameraLocation);
			//		Camera->AddWorldRotation(CombinedRotation);
			//	}
			//}
			//else
			//USpringArmComponent* MyCameraArm = Cast<USpringArmComponent>(ShooterCharacter->GetComponentByClass(USpringArmComponent::StaticClass()));
			//if (MyCameraArm)
			//{
			//	MyCameraArm->TargetArmLength = 100.f;
			//}