// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h" 
#include "Components/SphereComponent.h"     
#include "Goal.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class SHOOTERGAME_API AGoal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGoal();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Spawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// GoalópÇÃStaticMesh  
	UPROPERTY(VisibleAnywhere, Category = Goal, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Goal;

	class UNiagaraComponent* NiagaraComp;

	// ê⁄êGîªíËópÇÃCollision
	UPROPERTY(VisibleAnywhere, Category = Goal, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> Sphere;

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	

};
