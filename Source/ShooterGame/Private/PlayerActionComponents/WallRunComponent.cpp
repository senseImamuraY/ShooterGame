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

	// ゲーム開始時や壁走り開始時に、プレイヤーからカメラへのベクトルの大きさを保存
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
				// 滑らないようにする
				CharMovement->BrakingDecelerationFlying = 10000.0f;
	
				bWallRunning = true; 


				// キャラクターの位置と回転を取得
				FVector CharacterLocation = ShooterCharacter->GetActorLocation();
				FRotator CharacterRotation = ShooterCharacter->GetActorRotation();

				UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();

				// カメラの相対位置と回転を取得
				FVector CameraRelativeLocation = Camera->GetComponentLocation();
				FRotator CameraRelativeRotation = Camera->GetComponentRotation();

				// キャラクターの位置と回転をログに出力
				GEngine->AddOnScreenDebugMessage(11, 50.f, FColor::White, FString::Printf(TEXT("Character Location: %s"), *CharacterLocation.ToString()));
				GEngine->AddOnScreenDebugMessage(12, 50.f, FColor::White, FString::Printf(TEXT("Character Rotation: %s"), *CharacterRotation.ToString()));

				// カメラの相対位置と回転をログに出力
				GEngine->AddOnScreenDebugMessage(13, 50.f, FColor::White, FString::Printf(TEXT("Camera Relative Location: %s"), *CameraRelativeLocation.ToString()));
				GEngine->AddOnScreenDebugMessage(14, 50.f, FColor::White, FString::Printf(TEXT("Camera Relative Rotation: %s"), *CameraRelativeRotation.ToString()));


				FRotator NewRotation = FRotationMatrix::MakeFromXZ(ShooterCharacter->GetActorUpVector(), HitResult.Normal).Rotator();

				// Yawを-90度回転させる。
				NewRotation.Roll = 0.1f;
				NewRotation.Pitch = 89.9f;		

				ShooterCharacter->SetActorRotation(NewRotation);
				
				

				//// カメラの現在の回転を取得
				//FRotator CameraRotator = Camera->GetComponentRotation();

				//// ピッチの値を-90度から90度の間に制限
				//CameraRotator.Pitch = FMath::Clamp(CameraRotator.Pitch, -90.0f, 90.0f);

				//// 新しい回転をカメラに設定
				//Camera->SetWorldRotation(CameraRotator);
				FRotator NewRotation2 = FRotationMatrix::MakeFromXZ(ShooterCharacter->GetActorUpVector(), HitResult.Normal).Rotator();

				Camera->AddWorldRotation(NewRotation2);
				//Camera->SetRelativeRotation(NewRotation);

				FVector RotatedVector = NewRotation2.RotateVector(Camera->GetComponentLocation() - ShooterCharacter->GetActorLocation());
				//Camera->SetRelativeLocation(RotatedVector);
				Camera->SetWorldLocation(ShooterCharacter->GetActorLocation() + RotatedVector);
				



					// キャラクターの位置と回転をログに出力
				GEngine->AddOnScreenDebugMessage(15, 50.f, FColor::Black, FString::Printf(TEXT("Character Location: %s"), *CharacterLocation.ToString()));
				GEngine->AddOnScreenDebugMessage(16, 50.f, FColor::Black, FString::Printf(TEXT("Character Rotation: %s"), *CharacterRotation.ToString()));

				// カメラの相対位置と回転をログに出力
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



		// キャラクターの位置と回転を取得
		FVector CharacterLocation = ShooterCharacter->GetActorLocation();
		FRotator CharacterRotation = ShooterCharacter->GetActorRotation();

		UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();

		// カメラの相対位置と回転を取得
		FVector CameraRelativeLocation = Camera->GetComponentLocation();
		FRotator CameraRelativeRotation = Camera->GetComponentRotation();

		// キャラクターの位置と回転をログに出力
		GEngine->AddOnScreenDebugMessage(11, 50.f, FColor::White, FString::Printf(TEXT("Character Location: %s"), *CharacterLocation.ToString()));
		GEngine->AddOnScreenDebugMessage(12, 50.f, FColor::White, FString::Printf(TEXT("Character Rotation: %s"), *CharacterRotation.ToString()));

		// カメラの相対位置と回転をログに出力
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


		// キャラクターの回転を更新
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

			// 正規化されたベクトルを取得 おなじだから必要なし。
			//FVector NormalizedHitObjectNormal = HitObjectNormal;
			//FVector NormalizedHitObjectNormal = HitObjectNormal + HitResult.ImpactPoint;
			GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Green, FString::Printf(TEXT("ImpactPoint: %s"), *HitResult.ImpactPoint.ToString()));

			FVector NormalizedRotatedCharacterToCamera = RotatedVector.GetSafeNormal();
			GEngine->AddOnScreenDebugMessage(4, 5.f, FColor::Orange, FString::Printf(TEXT("NormalizedTraceHitPointToCamera: %s"), *NormalizedRotatedCharacterToCamera.ToString()));
			GEngine->AddOnScreenDebugMessage(4, 5.f, FColor::Orange, FString::Printf(TEXT("NormalizedNewCameraLocation: %s"), *NormalizedNewCameraLocation.ToString()));
			GEngine->AddOnScreenDebugMessage(6, 5.f, FColor::Orange, FString::Printf(TEXT("RotatedVector: %s"), *RotatedVector.ToString()));

			//// ドット積を計算
			//float DotProduct = FVector::DotProduct(NormalizedHitObjectNormal, NormalizedTraceHitPointToCamera);
			//GEngine->AddOnScreenDebugMessage(7, 5.f, FColor::Cyan, FString::Printf(TEXT("DotProduct: %f"), DotProduct));

			//// 角度（ラジアン）を計算
			//float AngleRadians = acosf(DotProduct);
			//GEngine->AddOnScreenDebugMessage(8, 5.f, FColor::Cyan, FString::Printf(TEXT("AngleRadians: %f"), AngleRadians));	

			//float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);
			//// 角度を度に変換
			//GEngine->AddOnScreenDebugMessage(5, 5.f, FColor::Cyan, FString::Printf(TEXT("AngleDegrees: %f"), AngleDegrees));
			// キャラクターの前方ベクトルを取得
			FVector ForwardVector = ShooterCharacter->GetActorForwardVector();

			// 後ろ向きのベクトルを計算
			FVector BackwardVector = -ForwardVector;


			// ドット積を計算
			float DotProduct = FVector::DotProduct(BackwardVector, NormalizedRotatedCharacterToCamera);
			GEngine->AddOnScreenDebugMessage(7, 5.f, FColor::Cyan, FString::Printf(TEXT("DotProduct: %f"), DotProduct));

			// 外積を計算
			FVector CrossProduct = FVector::CrossProduct(BackwardVector, NormalizedRotatedCharacterToCamera);

			// 角度（ラジアン）を計算
			float AngleRadians = acosf(DotProduct);
			GEngine->AddOnScreenDebugMessage(8, 5.f, FColor::Cyan, FString::Printf(TEXT("AngleRadians: %f"), AngleRadians));

			//float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);
			//// 角度を度に変換
			//GEngine->AddOnScreenDebugMessage(9, 5.f, FColor::Cyan, FString::Printf(TEXT("AngleDegrees: %f"), AngleDegrees));

			// 角度を度に変換
			float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);

			// 外積のZ成分に基づいて角度に符号を付ける
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
				// プレイヤーからカメラへのレイキャスト
				if (hitBlock && !bFirstRayCast)
				{
					if (CharacterToCameraHitResult.GetActor() != ShooterCharacter)
					{
						// ヒットした位置にカメラを移動
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
				// デバッグラインの色を設定
				FColor LineColor = hitBlock ? FColor::Green : FColor::Red;
				// デバッグラインの描画
				DrawDebugLine(
					GetWorld(),
					ShooterCharacter->GetActorLocation(),
					ShooterCharacter->GetActorLocation() + NormalizedRotatedCharacterToCamera * InitialCameraDistance,
					LineColor,
					true, // 永続的なラインではない
					5.0f, // 5秒間表示
					0, // デプス優先度
					1.0f // ラインの太さ
				);
			}
			//else
			//{

			//	// 条件を満たしていない場合、元の回転に戻す
			//	ShooterCharacter->SetActorRotation(PreviousRotation);
			//	
			//	//Camera->SetWorldLocation(ShooterCharacter->GetActorLocation() + (ShooterCharacter->GetActorUpVector() * InitialCameraDistance));

			//	float ExcessAngle;
			//	// プラスマイナス90度を超えた分だけ角度を調整
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

			//	// 回転軸と角度からクオータニオンを作成
			//	FQuat RotationQuat = FQuat(RotationAxis, ExcessAngle);

			//	//FRotator ExcessRotator = RotationQuat.Rotator();
			//	//RotationQuat = ExcessRotator.Quaternion();

			//	Camera->AddWorldRotation(RotationQuat);

			//	// クオータニオンを使用してベクトルAを回転
			//	FVector RotatedVectorA = RotationQuat.RotateVector(NormalizedRotatedCharacterToCamera);

			//	// 新しい位置または方向を計算
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


			// キャラクターの位置と回転をログに出力
			GEngine->AddOnScreenDebugMessage(15, 50.f, FColor::Black, FString::Printf(TEXT("Character Location: %s"), *CharacterLocation.ToString()));
			GEngine->AddOnScreenDebugMessage(16, 50.f, FColor::Black, FString::Printf(TEXT("Character Rotation: %s"), *CharacterRotation.ToString()));

			// カメラの相対位置と回転をログに出力
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

			// 上方向に力を与えるベクトルを定義
			FVector LaunchVelocity(50, 0, 600);

			// キャラクターを打ち上げる
			ShooterCharacter->LaunchCharacter(LaunchVelocity, true, true);

			bFirstRayCast = true;
		}



	}
}

void UWallRunComponent::EnableWallRun()
{
	bCanWallRun = true;
}


//			//// カメラと壁の法線ベクトルとの間の角度を計算
//			//	float angle = FMath::Acos(FVector::DotProduct(HitObjectNormal, TraceHitPointToCamera.GetSafeNormal())) * FMath::RadiansToDegrees;
//
//				// 角度に基づいて距離を調整
//// 角度に基づいて距離を調整
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
//				// 距離の長さを補間
//				float newDistanceLength = FMath::Lerp(MaxDistanceLength, MinDistanceLength, distanceModifier);
//
//				// カメラの新しい位置を計算
//				//FVector newCameraLocation = ShooterCharacter->GetActorLocation() + CharacterToCamera.GetSafeNormal() * newDistanceLength;
//				//FVector newCameraLocation = ShooterCharacter->GetActorLocation() + CharacterToCamera.GetSafeNormal() * InitialCameraDistance;
//


//
//			// 角度が90度以上かどうかをチェック
//if (AngleDegrees >= 90.0f)
//{
//	//float OrthographicProjectionTheta = AngleDegrees - 90.f;
//	//float Cosine = FMath::Cos(AngleRadians);
//
//	//FVector RadiusVector = HitObjectNormal * InitialCameraDistance;
//	//// プレイヤーのRightベクトルを取得
//	//FVector PlayerRightVector = ShooterCharacter->GetActorRightVector();
//
//	//// 90度回転を表す四元数を作成（ラジアンに変換）
//	//FQuat RotationQuat = FQuat(PlayerRightVector, FMath::DegreesToRadians(90.0f));
//
//	//// RadiusVectorを回転
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
////				// カメラの位置を更新
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
			//	// プレイヤーのRightベクトルを取得
			//	FVector PlayerRightVector = ShooterCharacter->GetActorRightVector();
			//	
			//	// 90度回転を表す四元数を作成（ラジアンに変換）
			//	FQuat RotationQuat = FQuat(PlayerRightVector, FMath::DegreesToRadians(90.0f));
			//	
			//	// RadiusVectorを回転
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
			//		// カメラの位置を更新
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