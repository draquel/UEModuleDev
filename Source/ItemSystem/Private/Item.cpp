#include "Item.h"

#include "InventoryComponent.h"
#include "SQLiteManager.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"

AItemBase::AItemBase()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(Mesh);
}

void AItemBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	Load();
}

inline void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	Load();
}

void AItemBase::Load()
{
	// LoadFromDataTable();
	if(Id > 0) {
		SQLiteConnection = USQLiteManager::GetGIDatabaseManager(GetWorld());
		if(SQLiteConnection != nullptr){ LoadFromDatabase(SQLiteConnection); }
		// else { UE_LOG(LogTemp, Error, TEXT("AItemBase::Database Load Error")); }
		Update();
	}
}

void AItemBase::LoadFromDataTable()
{
	if(ItemDataTable == nullptr) {
		UE_LOG(LogTemp,Error,TEXT("No Item Data Table configured."));
		return;
	}
	Definition = *ItemDataTable->FindRow<FItemDefinition>("Id",FString::FromInt(Id),true);
}

void AItemBase::AddToDatabase(USQLiteManager* SQLite)
{
	if(SQLite == nullptr)	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::Database Add Error"));
		return;
	}
	FSQLitePreparedStatement* PreparedStatement = SQLite->CreatePreparedStatement(TEXT("INSERT INTO items (name,type,icon,mesh,stackable,mass,scale,overlapRadius) VALUES(@Name,@Type,@Icon,@Mesh,@Stackable,@Mass,@Scale,@OverlapRadius)"));
	PreparedStatement->SetBindingValueByName(TEXT("@Name"),Definition.Name);
	PreparedStatement->SetBindingValueByName(TEXT("@Type"),Definition.Type.GetValue());
	PreparedStatement->SetBindingValueByName(TEXT("@Icon"),Definition.Icon == nullptr ? "" : Definition.Icon->GetPathName());
	PreparedStatement->SetBindingValueByName(TEXT("@Mesh"),Definition.Mesh == nullptr ? "" : Definition.Mesh->GetPathName());
	PreparedStatement->SetBindingValueByName(TEXT("@Stackable"),Definition.StackSize);
	PreparedStatement->SetBindingValueByName(TEXT("@Mass"),Definition.Mass);
	PreparedStatement->SetBindingValueByName(TEXT("@Scale"),Definition.Scale);
	PreparedStatement->SetBindingValueByName(TEXT("@OverlapRadius"),Definition.OverlapRadius);
	SQLite->RunAction(PreparedStatement);
	
	FSQLitePreparedStatement* IdPreparedStatement = SQLite->CreatePreparedStatement(TEXT("SELECT last_insert_rowid();"));
	while(IdPreparedStatement->Step() == ESQLitePreparedStatementStepResult::Row) {
		IdPreparedStatement->GetColumnValueByIndex(0,Definition.Id);
		Id = Definition.Id;
	}
	
	UE_LOG(LogTemp,Log,TEXT("Added Item '%s' to Database"),*Definition.Name.ToString());
}

void AItemBase::UpdateDatabase(USQLiteManager* SQLite)
{
	if(SQLite == nullptr)	{
		UE_LOG(LogTemp, Error, TEXT("AItemBase::Database Update Error"));
		return;
	}
	FSQLitePreparedStatement* PreparedStatement = SQLite->CreatePreparedStatement(TEXT("UPDATE items SET name=@Name,type=@Type,icon=@Icon,mesh=@Mesh,stackable=@Stackable,mass=@Mass,scale=@Scale,overlapRadius=@OverlapRadius WHERE id = @Id"));
	PreparedStatement->SetBindingValueByName(TEXT("@Id"),Definition.Id);
	PreparedStatement->SetBindingValueByName(TEXT("@Name"),Definition.Name);
	PreparedStatement->SetBindingValueByName(TEXT("@Type"),Definition.Type.GetValue());
	PreparedStatement->SetBindingValueByName(TEXT("@Icon"),Definition.Icon == nullptr ? "" : Definition.Icon->GetPathName());
	PreparedStatement->SetBindingValueByName(TEXT("@Mesh"),Definition.Mesh == nullptr ? "" : Definition.Mesh->GetPathName());
	PreparedStatement->SetBindingValueByName(TEXT("@Stackable"),Definition.StackSize);
	PreparedStatement->SetBindingValueByName(TEXT("@Mass"),Definition.Mass);
	PreparedStatement->SetBindingValueByName(TEXT("@Scale"),Definition.Scale);
	PreparedStatement->SetBindingValueByName(TEXT("@OverlapRadius"),Definition.OverlapRadius);
	SQLite->RunAction(PreparedStatement);
	UE_LOG(LogTemp,Log,TEXT("Item '%s' Database Record Updated."),*Definition.Name.ToString());
}

void AItemBase::LoadFromDatabase(USQLiteManager* SQLite)
{
	if(SQLite == nullptr || !SQLite->IsConnected())	{
		UE_LOG(LogTemp, Error, TEXT("Item Database Connection Unavailable"));
		return;
	}
	FSQLitePreparedStatement* PreparedStatement = SQLite->CreatePreparedStatement(TEXT("SELECT * FROM items WHERE id = @Id"));
	PreparedStatement->SetBindingValueByName(TEXT("@Id"),Id);
	
	uint8 type;
	FString IconPath;
	FString MeshPath;
	uint8 stackable;	
	while(PreparedStatement->Step() == ESQLitePreparedStatementStepResult::Row) {
		if(	PreparedStatement->GetColumnValueByName(TEXT("id"),Definition.Id) &&
			PreparedStatement->GetColumnValueByName(TEXT("name"),Definition.Name) &&
			PreparedStatement->GetColumnValueByName(TEXT("type"),type) &&
			PreparedStatement->GetColumnValueByName(TEXT("icon"),IconPath) &&
			PreparedStatement->GetColumnValueByName(TEXT("mesh"),MeshPath) &&
			PreparedStatement->GetColumnValueByName(TEXT("stackable"),stackable) &&
			PreparedStatement->GetColumnValueByName(TEXT("scale"),Definition.Scale) &&
			PreparedStatement->GetColumnValueByName(TEXT("mass"),Definition.Mass) &&
			PreparedStatement->GetColumnValueByName(TEXT("overlapRadius"),Definition.OverlapRadius) ) {
			Definition.Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *MeshPath));
			Definition.Icon = IconPath == "" ? nullptr : Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *IconPath));
			Definition.Type = static_cast<TEnumAsByte<EItemType>>(type);
			if(stackable > 1){
				Definition.Stackable = true;
				Definition.StackSize = stackable;
			} else {
				Definition.Stackable = false;
				Definition.StackSize = 1;
			}
			break;
		}
	}
	
	PreparedStatement->Destroy();
	delete PreparedStatement;
}

void AItemBase::Update()
{
	int massMultiplier = 10;
	Mesh->SetRelativeScale3D(FVector(Definition.Scale,Definition.Scale,Definition.Scale));
	Mesh->SetStaticMesh(Definition.Mesh);
	Mesh->SetMassOverrideInKg(NAME_None, Definition.Mass * massMultiplier,true);

	Sphere->SetSphereRadius(Definition.OverlapRadius);
	CenterSphereOnMesh();
}

void AItemBase::CenterSphereOnMesh() const
{
	if (!Mesh || !Mesh->GetStaticMesh() || !Sphere) {
		return; 
	}

	FBox BoundingBox = Mesh->GetStaticMesh()->GetBoundingBox();
	FVector MeshCenter = BoundingBox.GetCenter();
	Sphere->SetRelativeLocation(MeshCenter);
}

// ITEM

AItem::AItem()
{
	UPrimitiveComponent* PhysicsBody = Cast<UPrimitiveComponent>(Mesh);
	PhysicsBody->SetSimulatePhysics(true);
}

void AItem::Pickup_Implementation(UInventoryComponent* InventoryComponent)
{
	// IPickupable::Pickup_Implementation(InventoryComponent);
	InventoryComponent->AddItem(Definition);
	Destroy();
}

// void AItem::Drop_Implementation(UWorld* World, UInventoryComponent* InventoryComponent, int SocketIndex)
// {
// 	if(InventoryComponent->isEmpty()){ return; }
// 	FItemSlot Slot = InventoryComponent->RemoveItem(SocketIndex);
// 	AActor* Parent = InventoryComponent->GetOwner();
// 	FVector SpawnLocation = (Parent->GetActorForwardVector() * 200) + Parent->GetActorLocation();
// 	AItem* Spawn = World->SpawnActor<AItem>(AItem::StaticClass(),SpawnLocation,FRotator(),FActorSpawnParameters());
// 	Spawn->Definition = Slot.ItemDefinition;
// 	Spawn->Id = Slot.ItemDefinition.Id;
// 	Spawn->Update();
// }

AContainer::AContainer() : AItemBase() {
	
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

// CONTAINER

void AContainer::Interact_Implementation(AActor* Interactor)
{
	// IInteractable::Interact_Implementation(Interactor);
	//trigger item interface:
	InventoryComponent->DebugList();
	InventoryComponent->OnLoot.Broadcast();
}

bool AContainer::CanInteract(AActor* Interactor) const
{
	return true;
}

AWeapon::AWeapon()
{
	UPrimitiveComponent* PhysicsBody = Cast<UPrimitiveComponent>(Mesh);
	PhysicsBody->SetSimulatePhysics(true);	
}
void AWeapon::Pickup_Implementation(UInventoryComponent* InventoryComponent)
{
	// IPickupable::Pickup_Implementation(InventoryComponent);
	InventoryComponent->AddItem(Definition);
	Destroy();
}
void AWeapon::AddToDatabase(USQLiteManager* SQLite)
{
	Super::AddToDatabase(SQLite);
	FSQLitePreparedStatement* PreparedStatement = SQLite->CreatePreparedStatement(TEXT("INSERT INTO weapons (item,type,attack_power) VALUES(@item,@type,@attack)"));
	PreparedStatement->SetBindingValueByName(TEXT("@item"),Definition.Id);
	PreparedStatement->SetBindingValueByName(TEXT("@type"),WeaponDetails.Type.GetValue());
	PreparedStatement->SetBindingValueByName(TEXT("@attack"),WeaponDetails.AttackPower);
	SQLite->RunAction(PreparedStatement);
}

void AWeapon::UpdateDatabase(USQLiteManager* SQLite)
{
	Super::UpdateDatabase(SQLite);
	FSQLitePreparedStatement* PreparedStatement = SQLite->CreatePreparedStatement(TEXT("UPDATE weapons SET type=@type,attack_power=@attack WHERE item = @item"));
	PreparedStatement->SetBindingValueByName(TEXT("@item"),Definition.Id);
	PreparedStatement->SetBindingValueByName(TEXT("@type"),WeaponDetails.Type.GetValue());
	PreparedStatement->SetBindingValueByName(TEXT("@attack"),WeaponDetails.AttackPower);
	SQLite->RunAction(PreparedStatement);
}

void AWeapon::LoadFromDatabase(USQLiteManager* SQLite)
{
	UE_LOG(LogTemp,Log,TEXT("Loading weapons from database"));
	Super::LoadFromDatabase(SQLite);
	FSQLitePreparedStatement* PreparedStatement = SQLite->CreatePreparedStatement(TEXT("SELECT * FROM weapons WHERE item = @item"));
	PreparedStatement->SetBindingValueByName(TEXT("@item"),Id);

	uint8 type;
	while(PreparedStatement->Step() == ESQLitePreparedStatementStepResult::Row)	{
		if(PreparedStatement->GetColumnValueByName(TEXT("type"),type) &&	PreparedStatement->GetColumnValueByName(TEXT("attack_power"),WeaponDetails.AttackPower))	{
			WeaponDetails.Type = static_cast<TEnumAsByte<EWeaponType>>(type);
			break;
		}
	}
	
	PreparedStatement->Destroy();
	delete PreparedStatement;
}

void AWeapon::LoadTest()
{
	if(Id > 0) {
		UE_LOG(LogTemp,Log,TEXT("Load Test"));
		// SQLiteConnection = USQLiteManager::GetGIDatabaseManager(GetWorld());
		if(SQLiteConnection != nullptr){ LoadFromDatabase(SQLiteConnection); }
		else { UE_LOG(LogTemp, Error, TEXT("AWeapon::Database Load Error")); }
		Update();
	}
}

void AWeapon::UpdateDefinition()
{
	if(Id > 0) {
		UE_LOG(LogTemp,Log,TEXT("Update Test"));
		// SQLiteConnection = USQLiteManager::GetGIDatabaseManager(GetWorld());
		if(SQLiteConnection != nullptr){ UpdateDatabase(SQLiteConnection); }
		else { UE_LOG(LogTemp, Error, TEXT("AWeapon::Database Load Error")); }
		// Update();
	}
}


void AAmmunition::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Display, TEXT("Ammunition::OnOverlap"));
}

//AMMUNITION

AAmmunition::AAmmunition()
{
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	MovementComponent->ProjectileGravityScale = 0.66f;
	MovementComponent->Bounciness = 0.0f;

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	
	Mesh->OnComponentHit.AddDynamic(this, &AAmmunition::OnHit);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAmmunition::OnOverlap);
}

void AAmmunition::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Display, TEXT("Ammunition::OnHit"));
	MovementComponent->StopMovementImmediately();
}


