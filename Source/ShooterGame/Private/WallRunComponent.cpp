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

				// 1. CharacterMovementコンポーネントを取得
				UCharacterMovementComponent* CharMovement = Cast<UCharacterMovementComponent>(ShooterCharacter->GetMovementComponent());
				if (!CharMovement) return;

				// 3. MovementModeをFlyingに設定
				CharMovement->SetMovementMode(MOVE_Flying);
				CharMovement->bOrientRotationToMovement = false;
				// 例: BrakingDecelerationFlyingを600.0fに設定する場合
				CharMovement->BrakingDecelerationFlying = 10000.0f;

				bWallRun = true;
				DefaultYaw = ShooterCharacter->GetActorRotation().Yaw;
				// 4. MakeRotFromXZを使用して回転を導出
				FRotator NewRotation = FRotationMatrix::MakeFromXZ(ShooterCharacter->GetActorUpVector(), HitResult.Normal).Rotator();
				//FRotator NewRotation = FRotationMatrix::MakeFromXZ(ShooterCharacter->GetActorUpVector(), HitResult.Normal).Rotator() * 0.99f;

				//FRotator CharacterRotationPrev = ShooterCharacter->GetActorRotation();
				//FVector CharacterLocation = ShooterCharacter->GetActorLocation();

				// Yawを-90度回転させる
				NewRotation.Roll = 0.f;
				NewRotation.Pitch = 89.9f;		
				//NewRotation.Yaw = -90;
				//NewRotation.Pitch = 0.1f;
				// 画面にNewRotationの値を表示
				CharMovement->RotationRate = FRotator(100.f, 100.f, 0.f); // 引数がピッチ、ヨー、ロールの順番なのに注意

				GEngine->AddOnScreenDebugMessage(3, 50.f, FColor::Blue, FString::Printf(TEXT("NewRotation: Pitch=%f, Yaw=%f, Roll=%f"), NewRotation.Pitch, NewRotation.Yaw, NewRotation.Roll));

				FQuat NewCharacterQuat = FQuat::FindBetweenVectors(ShooterCharacter->GetActorUpVector(), HitResult.Normal);
				//ShooterCharacter->SetActorRotation(NewCharacterQuat.Rotator());
				// 5. SetActorRotationを使用して新しい回転を適用
				ShooterCharacter->SetActorRotation(NewRotation);

				// 1. キャラクターの位置を取得
				//FVector CharacterLocation = ShooterCharacter->GetActorLocation();
				FRotator CharacterRotation = ShooterCharacter->GetActorRotation();

				// 2. カメラの現在の位置を取得
				//UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();
				//if (!Camera) return;
				//FVector CameraLocation = Camera->GetComponentLocation();
				//FRotator CameraRotation = Camera->GetComponentRotation();

				// 3. キャラクターを中心としたローカル座標系にカメラの位置を変換
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

				// 4. クオータニオンを使用してローカル座標系の位置を回転
				FQuat RotationQuat = FQuat(NewCharacterQuat);
				//FVector RotatedLocalPosition = RotationQuat.RotateVector(LocalPosition);

				// 5. 回転後の位置をワールド座標系に変換
				//FVector NewCameraLocation = CharacterLocation + RotatedLocalPosition;

				// 6. カメラの位置を更新
				//Camera->SetWorldLocation(NewCameraLocation);

				//// カメラの向きをキャラクターの方向に向ける
				//FVector LookAtTarget = ShooterCharacter->GetActorLocation();
				//FQuat CameraLookAtQuat = FQuat::FindBetweenVectors(Camera->GetForwardVector(), LookAtTarget - Camera->GetComponentLocation());
				//Camera->SetWorldRotation(CameraLookAtQuat.Rotator());

		//		FQuat NewCharacterQuat = FQuat::FindBetweenVectors(ShooterCharacter->GetActorUpVector(), -HitResult.Normal);
		//		//ShooterCharacter->SetActorRotation(NewCharacterQuat.Rotator());

		//		// 2. キャラクターの回転を基に、カメラの新しい位置を計算
		//		FVector CameraOffset = ShooterCharacter->GetCameraBoom()->SocketOffset;
		//		FVector NewCameraPosition = ShooterCharacter->GetActorLocation() + NewCharacterQuat.RotateVector(CameraOffset + FVector(0.f, 50.f, 70.f));

				UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();
				if (Camera) {
					//NewRotation *= -1;
					//Camera->SetWorldRotation(NewRotation);
					//FVector CameraOffset = ShooterCharacter->GetActorForwardVector() * -1 * 180.f; // CameraDistanceはカメラがキャラクターからどれだけ離れているかの距離です。
					////Camera->SocketOffset = FVector(0.f, 50.f, 70.f);
					//Camera->SetWorldLocation(ShooterCharacter->GetActorLocation() + CameraOffset + FVector(0.f, 50.f, 70.f));
		// CameraBoomの向きをキャラクターの向きに合わせる
					FVector LookAtTarget = ShooterCharacter->GetActorLocation();
					//FQuat CameraLookAtQuat = FQuat::FindBetweenVectors(Camera->GetForwardVector(), LookAtTarget - NewCameraPosition);

					//Camera->SetWorldLocation(NewCameraPosition);
					//Camera->SetWorldRotation(CameraLookAtQuat.Rotator());
			
				}
				
					 // キャラクターの回転を設定
				//FQuat NewCharacterQuat = FQuat::FindBetweenVectors(ShooterCharacter->GetActorUpVector(), -HitResult.Normal);
				////ShooterCharacter->SetActorRotation(NewCharacterQuat.Rotator());

				//// カメラブームの設定を調整
				//USpringArmComponent* CameraBoom = ShooterCharacter->GetCameraBoom();
				//if (CameraBoom)
				//{
				//	CameraBoom->TargetArmLength = 230.f; // カメラとキャラクターの距離を設定
				//	CameraBoom->SocketOffset = FVector(0.f, 35.f, 80.f); // オフセットを設定
				//	CameraBoom->bUsePawnControlRotation = true; // カメラブームがプレイヤーの回転を追従するように設定
				//}

				//// カメラの向きをキャラクターの方向に向ける
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

			//	// 1. CharacterMovementコンポーネントを取得
			//	UCharacterMovementComponent* CharMovement = Cast<UCharacterMovementComponent>(ShooterCharacter->GetMovementComponent());
			//	if (!CharMovement) return;

			//	// 3. MovementModeをFlyingに設定
			//	CharMovement->SetMovementMode(MOVE_Flying);
			//	CharMovement->bOrientRotationToMovement = false;
			//	// 例: BrakingDecelerationFlyingを600.0fに設定する場合
			//	CharMovement->BrakingDecelerationFlying = 10000.0f;

			//	bWallRun = true;
			//	DefaultYaw = ShooterCharacter->GetActorRotation().Yaw;
			//	// 4. MakeRotFromXZを使用して回転を導出
			//	FRotator NewRotation = FRotationMatrix::MakeFromXZ(ShooterCharacter->GetActorUpVector(), HitResult.Normal).Rotator();
			//	//FRotator NewRotation = FRotationMatrix::MakeFromXZ(ShooterCharacter->GetActorUpVector(), HitResult.Normal).Rotator() * 0.99f;

			//	FRotator CharacterRotationPrev = ShooterCharacter->GetActorRotation();
			//	FVector CharacterLocation = ShooterCharacter->GetActorLocation();

			//	// Yawを-90度回転させる
			//	NewRotation.Roll = 0.f;
			//	NewRotation.Pitch = 89.9f;
			//	NewRotation.Yaw = -90;
			//	//NewRotation.Pitch = 0.1f;
			//	// 画面にNewRotationの値を表示
			//	CharMovement->RotationRate = FRotator(100.f, 100.f, 0.f); // 引数がピッチ、ヨー、ロールの順番なのに注意

			//	GEngine->AddOnScreenDebugMessage(3, 50.f, FColor::Blue, FString::Printf(TEXT("NewRotation: Pitch=%f, Yaw=%f, Roll=%f"), NewRotation.Pitch, NewRotation.Yaw, NewRotation.Roll));

			//	FQuat NewCharacterQuat = FQuat::FindBetweenVectors(ShooterCharacter->GetActorUpVector(), HitResult.Normal);
			//	//ShooterCharacter->SetActorRotation(NewCharacterQuat.Rotator());
			//	// 5. SetActorRotationを使用して新しい回転を適用
			//	ShooterCharacter->SetActorRotation(NewRotation);

			//	// 1. キャラクターの位置を取得
			//	//FVector CharacterLocation = ShooterCharacter->GetActorLocation();
			//	FRotator CharacterRotation = ShooterCharacter->GetActorRotation();

			//	// 2. カメラの現在の位置を取得
			//	UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();
			//	if (!Camera) return;
			//	FVector CameraLocation = Camera->GetComponentLocation();
			//	FRotator CameraRotation = Camera->GetComponentRotation();

			//	// 3. キャラクターを中心としたローカル座標系にカメラの位置を変換
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
		// 1. CharacterMovementコンポーネントを取得
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

		////CharMovement->RotationRate = FRotator(0.f, 0.f, 540.f); // 引数がピッチ、ヨー、ロールの順番なのに注意
		FRotator CurrentRotation2 = ShooterCharacter->GetActorRotation();
		GEngine->AddOnScreenDebugMessage(4, 50.f, FColor::Orange, FString::Printf(TEXT("CurrentRotation: Pitch=%f, Yaw=%f, Roll=%f"), CurrentRotation2.Pitch, CurrentRotation2.Yaw, CurrentRotation2.Roll));




		// 1. キャラクターの現在のクオータニオンを取得
		FQuat CurrentCharacterQuat = ShooterCharacter->GetActorQuat();

		// 2. キャラクター用の新しいクオータニオン回転を作成
		FQuat CharacterDeltaQuat = FQuat(FVector(0.0f, 0.0f, 1.0f), FMath::DegreesToRadians(MouseXValue));

		// 3. キャラクターのクオータニオンと新しいクオータニオンを組み合わせる
		FQuat NewCharacterQuat = CurrentCharacterQuat * CharacterDeltaQuat;

		// 4. この組み合わせたクオータニオンをキャラクターの回転に適用
		ShooterCharacter->SetActorRotation(NewCharacterQuat.Rotator());

		// 5. カメラ用の新しいクオータニオン回転を作成
		// この例では、Z軸（ヨー）を中心にMouseYValueだけ回転させるクオータニオンを作成します。
		FQuat CameraDeltaQuat = FQuat(FVector(0.0f, 0.0f, 1.0f), FMath::DegreesToRadians(MouseXValue));


		// 2. 入力値Valueを使用して、ピッチ回転のクオータニオンを計算
		FQuat PitchQuat = FQuat(FVector(0.0f, 1.0f, 0.0f), FMath::DegreesToRadians(MouseYValue));

		// 6. カメラの現在のクオータニオンを取得
		UCameraComponent* Camera = ShooterCharacter->FindComponentByClass<UCameraComponent>();

		if (Camera) {
			FQuat CurrentCameraQuat = Camera->GetComponentQuat();

			//// 7. カメラのクオータニオンと新しいクオータニオンを組み合わせる
			//FQuat NewCameraQuat = CurrentCameraQuat * CameraDeltaQuat;

			//// 3. 現在のクオータニオンと新しいクオータニオンを組み合わせる
			//FQuat NewQuat = CurrentCameraQuat * PitchQuat;
			 // 7. カメラのクオータニオンと新しいクオータニオンを組み合わせる
			FQuat NewCameraQuat = CurrentCameraQuat * CameraDeltaQuat * PitchQuat;


			// 8. この組み合わせたクオータニオンをカメラの回転に適用
			Camera->SetWorldRotation(NewCameraQuat.Rotator());

			// 4. 新しい回転をカメラに適用
			//Camera->SetWorldRotation(NewQuat.Rotator());
		}


		if (!bHit)
		{

			FRotator NewRotation = FRotationMatrix::MakeFromXZ(ShooterCharacter->GetActorUpVector() * -1.f, FVector(0.f, 0.f, 1.f)).Rotator() * 0.99f;

			// Yawを-90度回転させる
			//NewRotation.Roll = 0.01f;
			// 画面にNewRotationの値を表示
			NewRotation.Pitch = 0.1f;
			// 5. SetActorRotationを使用して新しい回転を適用
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

				CharMovement->RotationRate = FRotator(0.f, 540.f, 0.f); // 引数がピッチ、ヨー、ロールの順番なのに注意

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

