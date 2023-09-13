// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "InputMappingContext.h" // �ǉ�
#include "InputAction.h" // �ǉ�
#include "ShooterPlayerController.generated.h"



class UInputAction;

UCLASS()
class SHOOTERGAME_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AShooterPlayerController();

	// APlayerController����Override����
	virtual void SetupInputComponent() override;

protected:
	virtual void BeginPlay() override;

	// Pause���j���[��\������
	void DispPause(const FInputActionValue& Value);

private:
	// Overall HUO Overlay Blueprint Class�ւ̎Q��
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
