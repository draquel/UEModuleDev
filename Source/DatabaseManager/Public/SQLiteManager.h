#pragma once

#include "CoreMinimal.h"
#include "SQLiteDatabase.h"
#include "UObject/Object.h"
#include "SQLiteManager.generated.h"

class FSQLitePreparedStatement;
class FSQLiteDatabase;

USTRUCT(BlueprintType)
struct DATABASEMANAGER_API FSQLiteSettings{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString Path;

	FSQLiteSettings()
	{
		Path = "database.db";
	}
	FSQLiteSettings(FString path)
	{
		Path = path;
	}
};

UCLASS(BlueprintType)
class DATABASEMANAGER_API USQLiteManager : public UObject
{
	GENERATED_BODY()

protected:

	FSQLiteDatabase* Database;

public:

	USQLiteManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSQLiteSettings Settings;

	UFUNCTION(BlueprintCallable)
	bool Connect();
	UFUNCTION(BlueprintCallable)
	bool Disconnect();

	UFUNCTION(BlueprintCallable)
	bool IsConnected() const;

	FSQLitePreparedStatement* CreatePreparedStatement(const FString& Query) const;
	static bool RunAction(FSQLitePreparedStatement* PreparedStatement);

	UFUNCTION(BlueprintCallable)
	static USQLiteManager* GetGIDatabaseManager(UWorld* World);
};

