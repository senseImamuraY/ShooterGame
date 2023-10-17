#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PlayerLevelSystem.generated.h"

USTRUCT(BlueprintType)
struct FPlayerExP : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerExp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerAttackPower;
};

UCLASS()
class SHOOTERGAME_API UPlayerLevelSystem : public UObject
{
	GENERATED_BODY()

public:
	UPlayerLevelSystem();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	class UDataTable* PlayerExpDataTable;
};
