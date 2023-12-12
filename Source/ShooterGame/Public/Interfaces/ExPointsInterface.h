// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ExPointsInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UExPointsInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SHOOTERGAME_API IExPointsInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// この関数をBlueprintNativeEventとして宣言し、C++でのオーバーライドを許可します。
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Experience Points")
	void CalculateExPoints(float AddedExPoints);
};
