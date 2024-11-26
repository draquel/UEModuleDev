#pragma once

#include "CoreMinimal.h"
#include "SQLiteManager.h"
#include "UObject/Object.h"
#include "SQLiteExample.generated.h"

class USQLiteManager;

USTRUCT(BlueprintType)
struct FExPlayer : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 id = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 score = 0;
};

UCLASS()
class DATABASEMANAGER_API ASQLiteExample : public AActor
{
	GENERATED_BODY()

protected:

	const FString DatabaseFilePath = FPaths::ProjectContentDir() + "Database/data.db3";

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	USQLiteManager* Database;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSQLiteSettings DBSettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FExPlayer TestPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 UpdateId = 0;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 RemoveId = 0;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void BeginDestroy() override;
	
	ASQLiteExample();

	UFUNCTION(CallInEditor)
	void ShowPlayers() const;

	UFUNCTION(CallInEditor)
	void RunAdd() const;

	UFUNCTION(CallInEditor)
	void RunRemove() const;

	UFUNCTION(CallInEditor)
	void RunUpdate();


protected:
	//SQL Interactions
	
	FExPlayer* GetPlayerByName(const FString& PlayerName) const;
	FExPlayer* GetPlayerById(int64 id) const;
	TArray<FExPlayer*> GetPlayers() const;

	bool AddPlayer(const FString& PlayerName) const;
	bool UpdatePlayer(FExPlayer* Player) const;
	bool UpdatePlayers(TArray<FExPlayer*> Players);

	bool DeletePlayer(const FExPlayer* Player) const;
};
