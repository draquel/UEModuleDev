// Fill out your copyright notice in the Description page of Project Settings.


#include "UDatabaseGameInstance.h"

UDatabaseGameInstance::UDatabaseGameInstance()
{
	DatabaseSettings = FSQLiteSettings("Database/data.db3");
	DatabaseManager = CreateDefaultSubobject<USQLiteManager>("SQLiteManager");
	DatabaseManager->Settings = DatabaseSettings;
}

void UDatabaseGameInstance::Init()
{
	Super::Init();

	if(DatabaseManager != nullptr && !DatabaseManager->IsConnected()) {		
		DatabaseManager->Connect();
	}
}

void UDatabaseGameInstance::BeginDestroy()
{
	Super::BeginDestroy();
	if(DatabaseManager != nullptr && DatabaseManager->IsConnected()) {
		DatabaseManager->Disconnect();
	}
}
