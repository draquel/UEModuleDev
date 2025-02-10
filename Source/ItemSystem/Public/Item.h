#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "Pickupable.h"
#include "SQLiteManager.h"
#include "Item.generated.h"

class UArrowComponent;
class UProjectileMovementComponent;
class USphereComponent;

UENUM(BlueprintType)
enum EItemType{
	Consumable UMETA(DisplayName="Consumable"),
	Armor UMETA(DisplayName="Armor"),
	Weapon UMETA(DisplayName="Weapon"),
	Ammunition UMETA(DisplayName="Ammunition"),
	Spell UMETA(DisplayName="Spell"),
	Quest UMETA(DisplayName="Quest"),
	Material UMETA(DisplayName="Material"),
	Container UMETA(DisplayName="Container"),
	Junk UMETA(DisplayName="Junk"),
	Common UMETA(DisplayName="Common")
};

UENUM(BlueprintType)
enum EWeaponType{
	NoWeapon UMETA(DisplayName="None"),
	One_Hand UMETA(DisplayName="One Hand"),
	Off_Hand UMETA(DisplayName="Off Hand"),
	Two_Hand UMETA(DisplayName="Two Hand"),
	Bow UMETA(DisplayName="Bow"),
	Shield UMETA(DisplayName="Shield"),
};

USTRUCT(BlueprintType)
struct FItemDefinition : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int64 Id;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TEnumAsByte<EItemType> Type;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool Stackable = false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 StackSize = 25;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float Mass = 1.0;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float Scale = 1.0;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float OverlapRadius = 75;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* Icon;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UStaticMesh* Mesh;
	
	FItemDefinition()	{
		Id = 0;
		Name = "";
		Icon = nullptr;
		Mesh = nullptr;
		Type = Junk;
	}	
};

USTRUCT(BlueprintType)
struct FWeaponDetails{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TEnumAsByte<EWeaponType> Type;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float AttackPower;

	FWeaponDetails() {
		Type = One_Hand;
		AttackPower = 1;
	};
};

USTRUCT(BlueprintType)
struct FWeaponDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FItemDefinition Item;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FWeaponDetails WeaponDetails;

	FWeaponDefinition()
	{
		Item = FItemDefinition();
		WeaponDetails = FWeaponDetails();
	}
};

UCLASS(BlueprintType)
class AItemBase : public AActor
{
	GENERATED_BODY()

public:
	AItemBase();

	FString DatabasePath = "Database/data.db3";
	
	UPROPERTY()
	USQLiteManager* SQLiteConnection;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UDataTable* ItemDataTable;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* Sphere;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemDefinition Definition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Id;

	UFUNCTION(BlueprintCallable)
	void Load();

	virtual void AddToDatabase(USQLiteManager* SQLiteConnection);
	virtual void UpdateDatabase(USQLiteManager* SQLiteConnection);
	virtual void LoadFromDatabase(USQLiteManager* SQLiteConnection);

	void LoadFromDataTable();

	UFUNCTION(BlueprintCallable)
	void Update();

	void CenterSphereOnMesh() const;
};

UCLASS(BlueprintType)
class AItem : public AItemBase, public IPickupable{

	GENERATED_BODY()

	public:
		AItem();
	
		// UFUNCTION(BlueprintCallable)
		virtual void Pickup_Implementation(UInventoryComponent* InventoryComponent) override;
		// static void Drop_Implementation(UWorld* World, UInventoryComponent* InventoryComponent, int SocketIndex);
		// virtual void Drop(UInventoryComponent* InventoryComponent) override;
};


UCLASS(BlueprintType)
class AContainer : public AItemBase, public IInteractable
{
	GENERATED_BODY()

public:

	AContainer();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UInventoryComponent* InventoryComponent;

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual bool CanInteract(AActor* Interactor) const override;
};

UCLASS(BlueprintType)
class AWeapon : public AItemBase, public IPickupable
{
	GENERATED_BODY()
public:

	AWeapon();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FWeaponDetails WeaponDetails;

	virtual void AddToDatabase(USQLiteManager* SQLiteConnection) override;
	virtual void UpdateDatabase(USQLiteManager* SQLiteConnection) override;
	virtual void LoadFromDatabase(USQLiteManager* SQLiteConnection) override;

	UFUNCTION(CallInEditor)
	void LoadTest();

	UFUNCTION(CallInEditor)
	void UpdateDefinition();
	
	// virtual void Interact_Implementation(AActor* Interactor) override;
	virtual void Pickup_Implementation(UInventoryComponent* InventoryComponent) override;
	// virtual bool CanInteract(AActor* Interactor) const override;
};




UCLASS(BlueprintType)
class AAmmunition : public AItemBase
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	AAmmunition();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* Arrow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProjectileMovementComponent* MovementComponent;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};