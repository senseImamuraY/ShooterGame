// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WallRunComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTERGAME_API UWallRunComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWallRunComponent();

	void WallRun();
	float MouseXValue;
	float MouseYValue;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	bool bWallRun;
	float PreviousYaw;
	float DefaultYaw;

	FVector InitialCameraLocation;
	FRotator InitialCameraRotation;

	FVector HitWallNormal;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE bool GetIsWallRunning() const { return bWallRun; }
	FORCEINLINE FVector GetHitWallNormal() const { return HitWallNormal; }
};
