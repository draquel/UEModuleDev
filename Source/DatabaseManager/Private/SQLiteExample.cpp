#include "SQLiteExample.h"

#include "DSP/AudioDebuggingUtilities.h"


ASQLiteExample::ASQLiteExample()
{
	DBSettings = FSQLiteSettings();
	Database = CreateDefaultSubobject<USQLiteManager>("SQLiteManager");
	TestPlayer = FExPlayer();
}

void ASQLiteExample::BeginPlay()
{
	Super::BeginPlay();
	Database->Settings.Path = DBSettings.Path;
	Database->Connect();
}

void ASQLiteExample::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Database->Disconnect();
	Super::EndPlay(EndPlayReason);
}

void ASQLiteExample::BeginDestroy()
{
	Super::BeginDestroy();
}

// Drivers

void ASQLiteExample::ShowPlayers() const
{
	if(!Database->IsConnected()){ return; }
	
	float duration = 5.f;
	UE_LOG(LogTemp, Display, TEXT("ASQLiteExample::ShowPlayers() => Begin"));
	GEngine->AddOnScreenDebugMessage(0, duration, FColor::Yellow, TEXT("ASQLiteExample::ShowPlayers() => Begin"));
	
	TArray<FExPlayer*> players =  GetPlayers();

	int i = 0;
	for (FExPlayer* player : players) {
		GEngine->AddOnScreenDebugMessage(1+i, duration, FColor::Yellow, FString::Printf(TEXT(" ==> [%lld] - %s"), player->id, *player->name));
		UE_LOG(LogTemp, Display, TEXT(" ==> [%lld] - %s"), player->id, *player->name);
		i++;
	}
	
	UE_LOG(LogTemp, Display, TEXT("ASQLiteExample::ShowPlayers() => End"));
	GEngine->AddOnScreenDebugMessage(players.Num()+1, duration, FColor::Yellow, TEXT("ASQLiteExample::ShowPlayers() => End"));
}

void ASQLiteExample::RunAdd() const
{
	UE_LOG(LogTemp, Display, TEXT("ASQLiteExample::CreateTestPlayer() => Begin"));
	if(AddPlayer(TestPlayer.name))	{
		ShowPlayers();
	}
	UE_LOG(LogTemp, Display, TEXT("ASQLiteExample::CreateTestPlayer() => End"));
}

void ASQLiteExample::RunRemove() const
{
	UE_LOG(LogTemp, Display, TEXT("ASQLiteExample::RunRemove => Begin"));
	FExPlayer* remove = GetPlayerById(RemoveId);
	if(DeletePlayer(remove)) {
		ShowPlayers();
	}
	UE_LOG(LogTemp, Display, TEXT("ASQLiteExample::RunRemove => End"));
}

void ASQLiteExample::RunUpdate()
{
	UE_LOG(LogTemp, Display, TEXT("ASQLiteExample::RunUpdate => Begin"));
	FExPlayer* update = GetPlayerById(UpdateId);
	update->name = TestPlayer.name;
	update->score = TestPlayer.score;
	if(UpdatePlayer(update)) {
		ShowPlayers();
	}
	UE_LOG(LogTemp, Display, TEXT("ASQLiteExample::RunUpdate => End"));
}


//SQL Select Queries

FExPlayer* ASQLiteExample::GetPlayerByName(const FString& PlayerName) const
{
	FSQLitePreparedStatement* PreparedStatement = Database->CreatePreparedStatement(TEXT("SELECT * FROM ex_players WHERE name = @Name;"));
	PreparedStatement->SetBindingValueByName(TEXT("@Name"), PlayerName);

	FExPlayer* player = new FExPlayer();
	bool found = false;
	while(PreparedStatement->Step() == ESQLitePreparedStatementStepResult::Row) {
		if(PreparedStatement->GetColumnValueByName(TEXT("id"),player->id) &&	PreparedStatement->GetColumnValueByName(TEXT("name"),player->name) && PreparedStatement->GetColumnValueByName(TEXT("score"),player->score)) {
			found = true;
			break;
		}
	}
	
	PreparedStatement->Destroy();
	delete PreparedStatement;
	return found ? player : nullptr;
}

FExPlayer* ASQLiteExample::GetPlayerById(const int64 id) const
{
	FSQLitePreparedStatement* PreparedStatement = Database->CreatePreparedStatement(TEXT("SELECT * FROM ex_players WHERE id = @Id;"));
	PreparedStatement->SetBindingValueByName(TEXT("@Id"), id);

	FExPlayer* player = new FExPlayer();
	bool found = false;
	while(PreparedStatement->Step() == ESQLitePreparedStatementStepResult::Row) {
		if(PreparedStatement->GetColumnValueByName(TEXT("id"),player->id) &&	PreparedStatement->GetColumnValueByName(TEXT("name"),player->name) && PreparedStatement->GetColumnValueByName(TEXT("score"),player->score)) {
			found = true;
			break;
		}
	}
	
	PreparedStatement->Destroy();
	delete PreparedStatement;
	return found ? player : nullptr;
}

TArray<FExPlayer*> ASQLiteExample::GetPlayers() const
{
	FSQLitePreparedStatement* PreparedStatement = Database->CreatePreparedStatement(TEXT("SELECT * FROM ex_players;"));
	TArray<FExPlayer*> players; 
	while(PreparedStatement->Step() == ESQLitePreparedStatementStepResult::Row) {
		FExPlayer* player = new FExPlayer();
		if(PreparedStatement->GetColumnValueByName(TEXT("id"),player->id) &&	PreparedStatement->GetColumnValueByName(TEXT("name"),player->name) && PreparedStatement->GetColumnValueByName(TEXT("score"),player->score)) {
			players.Add(player);
		}
	}
	
	PreparedStatement->Destroy();
	delete PreparedStatement;
	return players;
}

// SQL Action Queries

bool ASQLiteExample::AddPlayer(const FString& PlayerName) const
{
	FSQLitePreparedStatement* PreparedStatement = Database->CreatePreparedStatement(TEXT("INSERT INTO ex_players (name,score) VALUES(@Name,0);"));
	PreparedStatement->SetBindingValueByName(TEXT("@Name"), PlayerName);
	return Database->RunAction(PreparedStatement);	
}

bool ASQLiteExample::UpdatePlayer(FExPlayer* Player) const
{
	FSQLitePreparedStatement* PreparedStatement = Database->CreatePreparedStatement(TEXT("UPDATE ex_players SET name = @Name, score = @Score WHERE id = @Id;"));
	PreparedStatement->SetBindingValueByName(TEXT("@Id"), Player->id);
	PreparedStatement->SetBindingValueByName(TEXT("@Name"), Player->name);
	PreparedStatement->SetBindingValueByName(TEXT("@Score"), Player->score);
	return Database->RunAction(PreparedStatement);
}

bool ASQLiteExample::DeletePlayer(const FExPlayer* Player) const
{
	FSQLitePreparedStatement* PreparedStatement = Database->CreatePreparedStatement(TEXT("DELETE FROM ex_players WHERE id = @Id;"));
	PreparedStatement->SetBindingValueByName(TEXT("@Id"), Player->id);
	return Database->RunAction(PreparedStatement);	
}
