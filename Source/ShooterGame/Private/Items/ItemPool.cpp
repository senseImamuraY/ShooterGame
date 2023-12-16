// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemPool.h"
#include "../Public/Items/Item.h"
#include "../Public/Enemies/Enemy.h"


UItemPool::UItemPool()
{
	static ConstructorHelpers::FObjectFinder<UClass> BP_Ammo9mm(TEXT("Blueprint'/Game/ShooterGame/Blueprints/Items/BP_Ammo9mm.BP_Ammo9mm_C'"));
	static ConstructorHelpers::FObjectFinder<UClass> BP_AmmoShells(TEXT("Blueprint'/Game/ShooterGame/Blueprints/Items/BP_AmmoShells.BP_AmmoShells_C'"));
	//static ConstructorHelpers::FObjectFinder<UClass> BP_Potion(TEXT("/Script/Engine.Blueprint'/Game/ShooterGame/Blueprints/Items/BP_Potion.BP_Potion'"));
	static ConstructorHelpers::FObjectFinder<UClass> BP_Potion(TEXT("Blueprint'/Game/ShooterGame/Blueprints/Items/BP_Potion.BP_Potion_C'"));


	if (BP_Ammo9mm.Succeeded())
	{
		ItemClasses.Add(BP_Ammo9mm.Object);
		Ammo9mmClass = BP_Ammo9mm.Object;
	}

	if (BP_AmmoShells.Succeeded())
	{
		ItemClasses.Add(BP_AmmoShells.Object);
		AmmoShellsClass = BP_AmmoShells.Object;
	}

	if (BP_Potion.Succeeded())
	{
		ItemClasses.Add(BP_Potion.Object);
		PotionClass = BP_Potion.Object;
	}
}

void UItemPool::Initialize(UWorld* World)
{
	WorldReference = World;

	InitializeItemPool(Ammo9mmClass, 10);
	InitializeItemPool(AmmoShellsClass, 10);
	InitializeItemPool(PotionClass, 10);
}

void UItemPool::InitializeItemPool(UClass* ItemClass, int32 ItemNum)
{
	if (!ItemClass) return;

	TArray<AItem*>& ItemArray = ItemPools.FindOrAdd(ItemClass);
	for (int32 i = 0; i < ItemNum; i++)
	{
		AItem* NewItem = WorldReference->SpawnActor<AItem>(ItemClass, FVector(0.f, 0.f, 100.f), FRotator::ZeroRotator);
		if (NewItem)
		{
			NewItem->OnItemReturnRequested.AddDynamic(this, &UItemPool::ReturnItem);
			NewItem->SetActorHiddenInGame(true);
			NewItem->SetActorEnableCollision(false);
			NewItem->SetActorTickEnabled(false);
			//NewItem->SetActorHiddenInGame(false);
			//NewItem->SetActorEnableCollision(true);
			//NewItem->SetActorTickEnabled(true);

			NewItem->SetItemState(EItemState::EIS_InPool);
			ItemArray.Add(NewItem);

			//// 重力を有効にする
			//UPrimitiveComponent* PrimaryComponent = Cast<UPrimitiveComponent>(NewItem->GetRootComponent());
			//if (PrimaryComponent)
			//{
			//	//PrimaryComponent->SetSimulatePhysics(true);
			//	//PrimaryComponent->SetEnableGravity(true);
			//}
		}
	}
}

AItem* UItemPool::SpawnItem()
{
	return nullptr;
}

TSubclassOf<AActor> UItemPool::GetRandomItemClass()
{
	// 0から99までのランダムな数値を生成
	int32 RandomNumber = FMath::RandRange(0, 99);

	// 10% の確率で Potion を返す
	if (RandomNumber < 10)
	{
		return PotionClass;
		return nullptr;

	}
	// 20% の確率で AmmoShells を返す
	else if (RandomNumber < 30)
	{
		return AmmoShellsClass;
	}
	// 30% の確率で Ammo9mm を返す
	else if (RandomNumber < 60)
	{
		return Ammo9mmClass;
	}
	// それ以外の場合は何も返さない
	else
	{
		return nullptr;
	}
}

AItem* UItemPool::GetItem(UClass* ItemClass)
{
	TArray<AItem*>* ItemArray = ItemPools.Find(ItemClass);

	if (ItemArray && ItemArray->Num() > 0)
	{
		// 配列の内容をログに出力
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Yellow, FString::Printf(TEXT("ItemArray Size: %d"), ItemArray->Num()));
			for (int32 i = 0; i < ItemArray->Num(); ++i)
			{
				AItem* CurrentItem = (*ItemArray)[i];
				FString ItemStatus = CurrentItem ? CurrentItem->GetName() : TEXT("nullptr");
				GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Yellow, FString::Printf(TEXT("Item %d: %s"), i, *ItemStatus));
			}
		}
		AItem* ItemToReturn = ItemArray->Pop();

		//ItemToReturn->SetActorHiddenInGame(true);
		//ItemToReturn->SetActorEnableCollision(false);
		//ItemToReturn->SetActorTickEnabled(false);
		//ItemToReturn->SetItemState(EItemState::EIS_InPool);
		ItemToReturn->SetItemState(EItemState::EIS_Pickup);
		ItemToReturn->SetActorHiddenInGame(false);
		ItemToReturn->SetActorEnableCollision(true);
		ItemToReturn->SetActorTickEnabled(true);
		// 重力を有効にする
		UPrimitiveComponent* PrimaryComponent = Cast<UPrimitiveComponent>(ItemToReturn->GetRootComponent());
		if (PrimaryComponent)
		{
			//PrimaryComponent->SetSimulatePhysics(false);
			//PrimaryComponent->SetEnableGravity(false);	
			PrimaryComponent->SetSimulatePhysics(true);
			PrimaryComponent->SetEnableGravity(true);
			//PrimaryComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		}

		//// ログ出力
		if (GEngine)
		{
			FString ItemName = ItemToReturn->GetName();
			int32 RemainingItems = ItemArray->Num();
			GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("Item Returned: %s, Remaining in Pool: %d"), *ItemName, RemainingItems));
		}

		return ItemToReturn;
	}

	return nullptr;
}

void UItemPool::ReturnItem(AItem* Item)
{
	if (!Item) return;

	TArray<AItem*>& ItemArray = ItemPools.FindOrAdd(Item->GetClass());
	Item->SetActorHiddenInGame(true);
	Item->SetActorEnableCollision(false);
	Item->SetActorTickEnabled(false);
	Item->SetItemState(EItemState::EIS_InPool);
	ItemArray.Add(Item);

	// ログ出力
	if (GEngine)
	{
		FString ItemName = Item->GetName();
		int32 RemainingItems = ItemArray.Num();
		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Item Returned: %s, Remaining in Pool: %d"), *ItemName, RemainingItems));
	}
}

