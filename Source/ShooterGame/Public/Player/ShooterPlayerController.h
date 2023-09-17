// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "InputMappingContext.h" // 追加
#include "InputAction.h" // 追加
#include "ShooterPlayerController.generated.h"



class UInputAction;

UCLASS()
class SHOOTERGAME_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AShooterPlayerController();

	// APlayerControllerからOverrideする
	virtual void SetupInputComponent() override;

protected:
	virtual void BeginPlay() override;

	// Pauseメニューを表示する
	void DispPause(const FInputActionValue& Value);

private:
	// Overall HUO Overlay Blueprint Classへの参照
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> HUDOverlayClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widgets, meta = (AllowPrivateAccess = "true"))
	UUserWidget* HUDOverlay;

	/** MappingContext */
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Pause Input Action */
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> PauseAction;

	class AInGameHUD* InGameHUDInstance;

	int CalledNum;
};
