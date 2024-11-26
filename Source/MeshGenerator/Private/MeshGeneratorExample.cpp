#include "MeshGeneratorExample.h"

#include "MeshData/MeshDataGenerator.h"


// Sets default values
AMeshGeneratorExample::AMeshGeneratorExample()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	
	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh");
	Mesh->SetCanEverAffectNavigation(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMeshGeneratorExample::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMeshGeneratorExample::Generate()
{
	Mesh->SetMaterial(0,Material);
	FMeshData Data = UMeshDataGenerator::MarchingCubes(GetActorLocation(),Size,UVScale,&NoiseSettings,isoLevel,interpolate);
	Data.CreateProceduralMesh(Mesh);
	DebugDraw();
	Data.Reset();
}

// Called every frame
void AMeshGeneratorExample::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// void AMeshGeneratorExample::OnConstruction(const FTransform& Transform)
// {
// 	Super::OnConstruction(Transform);
// }

void AMeshGeneratorExample::DebugDraw()
{
	FlushPersistentDebugLines(GetWorld());
	float life = 0.0f;
	FColor color = FColor::Red;
	DebugDrawBounds(life,color);
	DebugDrawPositionMarker(life,color);
}
void AMeshGeneratorExample::DebugDrawBounds(float life, FColor color)
{
	FVector extends = FVector(Size.X/2,Size.Y/2,Size.Z/2);
	FVector pos = GetActorLocation() + FVector(Size.X/2,Size.Y/2,Size.Z/2);
	DrawDebugBox(GetWorld(),pos,extends,color,true,life,0u,0.0f);
}

void AMeshGeneratorExample::DebugDrawPositionMarker(float life, FColor color)
{
	FVector extends = FVector(Size.X * 0.0125f,Size.Y * 0.0125f, Size.Z/2);
	FVector displace = FVector(Size.X * 0.0125f,Size.Y * 0.0125f,Size.Z/2);
	FVector pos = GetActorLocation()  + displace;
	DrawDebugSolidBox(GetWorld(),pos,extends,color,true,life,0);
}

void AMeshGeneratorExample::Clear()
{
	Mesh->ClearMeshSection(0);
	FlushPersistentDebugLines(GetWorld());
}
