// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemManager.h"
#include "Item.h"
#include "UDatabaseGameInstance.h"
#include "Kismet/GameplayStatics.h"

AItemManager::AItemManager()
{
	PrimaryActorTick.bCanEverTick = true;
	SQLiteManager = CreateDefaultSubobject<USQLiteManager>("SQLiteManager");
	DefaultMesh = CreateDefaultSubobject<UStaticMesh>("DefaultMesh");
	TestItemDefinition = FItemDefinition();
}

void AItemManager::GetLevelItems()
{
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemBase::StaticClass(), ActorList);
	Items.Empty();
	for (AActor* Actor : ActorList)	{
		AItemBase* Item = Cast<AItemBase>(Actor);
		Items.Add(Item);
	}
}

void AItemManager::LoadItemDefinitions()
{
	if(SQLiteManager == nullptr){
		SQLiteManager = NewObject<USQLiteManager>();
	}
	SQLiteManager->Settings.Path = DatabasePath;
	if(!SQLiteManager->Connect()) {
		UE_LOG(LogTemp, Error, TEXT("SQLite connection failed"));
		return;
	}

	for (AItemBase* Item : Items) {
		Item->LoadFromDatabase(SQLiteManager);
		Item->Update();
	}

	SQLiteManager->Disconnect();
	SQLiteManager = nullptr;
}

void AItemManager::ClearItemDefinitions()
{
	for (AItemBase* Item : Items) {
		Item->Definition = FItemDefinition();
		if(Item->IsA(AContainer::StaticClass())) {
			Item->Definition.Mesh = DefaultContainerMesh;
		} else {
			Item->Definition.Mesh = DefaultMesh;
			Item->Definition.Scale = Item->Definition.Scale * 0.5f;
		}
		Item->Update();
	}
}

void AItemManager::LoadTestItem()
{
	if(SQLiteManager == nullptr){
		SQLiteManager = NewObject<USQLiteManager>();
	}
	SQLiteManager->Settings.Path = DatabasePath;
	if(!SQLiteManager->Connect()) {
		UE_LOG(LogTemp, Error, TEXT("SQLite connection failed"));
		return;
	}

	AItemBase* TestItem = NewObject<AItemBase>();
	TestItem->Id = TestItemDefinition.Id;
	TestItem->LoadFromDatabase(SQLiteManager);
	TestItemDefinition = TestItem->Definition;

	SQLiteManager->Disconnect();
	SQLiteManager = nullptr;
}

void AItemManager::AddTestItem()
{
	if(SQLiteManager == nullptr){
		SQLiteManager = NewObject<USQLiteManager>();
	}
	SQLiteManager->Settings.Path = DatabasePath;
	if(!SQLiteManager->Connect()) {
		UE_LOG(LogTemp, Error, TEXT("SQLite connection failed"));
		return;
	}

	AItemBase* TestItem = NewObject<AItemBase>();
	TestItem->Definition = TestItemDefinition;
	TestItem->AddToDatabase(SQLiteManager);	

	SQLiteManager->Disconnect();
	SQLiteManager = nullptr;
}

void AItemManager::UpdateTestItem()
{
	if(SQLiteManager == nullptr){
		SQLiteManager = NewObject<USQLiteManager>();
	}
	SQLiteManager->Settings.Path = DatabasePath;
	if(!SQLiteManager->Connect()) {
		UE_LOG(LogTemp, Error, TEXT("SQLite connection failed"));
		return;
	}

	AItemBase* TestItem = NewObject<AItemBase>();
	TestItem->Definition = TestItemDefinition;
	TestItem->UpdateDatabase(SQLiteManager);	

	SQLiteManager->Disconnect();
	SQLiteManager = nullptr;
}

// void AItemManager::BeginPlay()
// {
// 	Super::BeginPlay();
// }
//
// void AItemManager::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
// }
