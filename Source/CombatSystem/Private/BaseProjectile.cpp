// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseProjectile.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"


// Sets default values
ABaseProjectile::ABaseProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = Sphere;

	Arrow = CreateDefaultSubobject<UArrowComponent>("Arrow");
	Arrow->SetupAttachment(RootComponent);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent");
	
}

void ABaseProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Destroy();
}

// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	MovementComponent->InitialSpeed = InitialSpeed;
	MovementComponent->MaxSpeed = MaxSpeed;
	MovementComponent->ProjectileGravityScale = Gravity;

	Sphere->OnComponentHit.AddDynamic(this, &ABaseProjectile::OnHit);
}

void ABaseProjectile::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	
}


// Called every frame
void ABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

