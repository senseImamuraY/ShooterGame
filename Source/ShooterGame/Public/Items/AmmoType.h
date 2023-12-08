#pragma once

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_9mm UMETA(DisplayName = "9mm"),
	EAT_Shells UMETA(DisplayName = "Shells"),

	EAT_MAX UMETA(DisplayName = "Default Max")
};

