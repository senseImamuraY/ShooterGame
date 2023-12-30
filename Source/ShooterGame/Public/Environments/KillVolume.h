// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Core/ShooterGameGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "KillVolume.generated.h"

class UBoxComponent;

UCLASS()
class SHOOTERGAME_API AKillVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKillVolume();

	/** Box CollisionÇÃÉTÉCÉYÇê›íËÇ∑ÇÈïœêî **/
	UPROPERTY(EditAnywhere, Category = "Kill Volume")
	FVector BoxExtent;

private:
	/** Default Scene Root **/
	UPROPERTY(VisibleAnywhere, Category = KillVolume, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	/** PlayerÇKillÇ∑ÇÈAreaÇê›íËÇ∑ÇÈBox Collision **/
	UPROPERTY(VisibleAnywhere, Category = KillVolume, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> KillVolume;

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ConstructionScript
	virtual void OnConstruction(const FTransform& Transform) override;
};
