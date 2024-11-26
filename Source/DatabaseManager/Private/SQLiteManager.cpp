#include "SQLiteManager.h"
#include "SQLiteDatabase.h"
#include "UDatabaseGameInstance.h"

USQLiteManager::USQLiteManager()
{
	Settings = FSQLiteSettings();
	Database = nullptr;
}

bool USQLiteManager::Connect()
{
	if(Database != nullptr)	{ return Database->IsValid(); }
	
	Database = new FSQLiteDatabase();
	FString Path = FPaths::ProjectContentDir() + Settings.Path;
	
	if(Database->Open(*Path,ESQLiteDatabaseOpenMode::ReadWrite)) {
		UE_LOG(LogTemp, Log, TEXT("SQLiteManager => Opened Connection to %s"),*Settings.Path);
		return true;		
	}
	return false;	
}

bool USQLiteManager::Disconnect()
{
	if(Database == nullptr || Database->IsValid() == false)	{ 
		UE_LOG(LogTemp, Warning, TEXT("SQLiteManager => Cannot Disconnect. No Open Connection to %s"),*Settings.Path);
		return true;
	}
	if(Database->Close()) {
		UE_LOG(LogTemp, Log, TEXT("SQLiteManager => Closed Connection to %s"),*Settings.Path);
		delete Database;
		return true;
	}
	return false;
}

bool USQLiteManager::IsConnected() const
{
	if(Database != nullptr && Database->IsValid())	{ return true; }
	return false;
}

FSQLitePreparedStatement* USQLiteManager::CreatePreparedStatement(const FString& Query) const
{
	FSQLitePreparedStatement* PreparedStatement = new FSQLitePreparedStatement();
	if(PreparedStatement->Create(*Database,*Query,ESQLitePreparedStatementFlags::None)) {
		return PreparedStatement;
	}
	return nullptr;
}

bool USQLiteManager::RunAction(FSQLitePreparedStatement* PreparedStatement)
{
	bool success = PreparedStatement->Execute();

	PreparedStatement->Destroy();
	delete PreparedStatement;

	return success;
}

USQLiteManager* USQLiteManager::GetGIDatabaseManager(UWorld* World)
{
	UDatabaseGameInstance* GameInstance = Cast<UDatabaseGameInstance>(World->GetGameInstance());
	if(GameInstance != nullptr && GameInstance->DatabaseManager != nullptr) {
		return GameInstance->DatabaseManager;
	}
	return nullptr;	
}
