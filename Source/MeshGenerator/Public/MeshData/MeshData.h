#pragma once

#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "AI/NavigationSystemBase.h"
#include "Components/DynamicMeshComponent.h"
#include "DynamicMesh/MeshAttributeUtil.h"

struct FProcMeshTangent;


struct MESHGENERATOR_API FMeshData
{
public:
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector2D> UV0;
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;

	float UVScale;

	FMeshData()
	{
		UVScale = 1;
		Reset();
	}

	void Reset()
	{
		Vertices.Reset();
		Triangles.Reset();
		UV0.Reset();
		Normals.Reset();
		Tangents.Reset();
	}

	void AddTriangle(int a, int b, int c)
	{
		Triangles.Add(a);
		Triangles.Add(b);
		Triangles.Add(c);	
	}

	void AddTriangle(FVector a, FVector b, FVector c)
	{
		AddVertex(a);
		AddVertex(b);
		AddVertex(c);
		int vertIndex = Vertices.Num()-1;
		AddTriangle(vertIndex-2, vertIndex-1, vertIndex);
	}	

	void AddVertex(FVector Position)
	{
		Vertices.Add(Position);
		UV0.Add(FVector2D(Position.X,Position.Y)*UVScale);
	}

	void CalculateTangents()
	{
		UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices,Triangles,UV0,Normals,Tangents);
	}

	void CreateProceduralMesh(UProceduralMeshComponent* ProceduralMeshComponent, int section = 0, bool collision = true) const
	{
		ProceduralMeshComponent->ClearMeshSection(section);
		ProceduralMeshComponent->CreateMeshSection(section,Vertices,Triangles,Normals,UV0,TArray<FColor>(),Tangents,collision);
	}

	void CreateDynamicMesh(UDynamicMeshComponent* DynamicMeshComponent, bool collision = true) const
	{
		FDynamicMesh3 DynamicMesh (true,true,true,true);
		DynamicMesh.EnableAttributes();
		DynamicMesh.EnableVertexUVs(FVector2f::Zero());
	
		for (int i = 0; i < Vertices.Num(); i++) {
			int vId = DynamicMesh.AppendVertex(Vertices[i]);
			DynamicMesh.SetVertexNormal(vId,(FVector3f)Normals[i]);
			DynamicMesh.SetVertexUV(vId,(FVector2f)UV0[i]);
		}
		for(int i = 0; i < Triangles.Num(); i+=3) {
			DynamicMesh.AppendTriangle(Triangles[i],Triangles[i+1],Triangles[i+2]);
		}
	
		// UE::Geometry::FMeshNormals::QuickComputeVertexNormals(DynamicMesh);
		AverageVertexNormalsByUVPosition(DynamicMesh);
		UE::Geometry::CopyVertexUVsToOverlay(DynamicMesh, *DynamicMesh.Attributes()->PrimaryUV());	
	
		DynamicMeshComponent->GetDynamicMesh()->SetMesh(DynamicMesh);
		if(collision){ DynamicMeshComponent->UpdateCollision(); }
	}

	void ConfigureForNavigation(UProceduralMeshComponent* Component)
	{
		Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Component->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
		Component->SetCanEverAffectNavigation(true);
		Component->UpdateNavigationBounds();
		Component->MarkRenderStateDirty();
		FNavigationSystem::UpdateComponentData(*Component);
	}

	void DebugDrawVertices(const UWorld* World, float lifeTime = 15.0f)
	{
		FlushPersistentDebugLines(World);
		for (FVector Vert : Vertices) {
			DrawDebugSphere(World,Vert,2.0f,8,FColor::Red,true,lifeTime, 0u, 0.0f);
		}
	}

	static double LocalizePos(float input, float size)
	{
		int res = FMath::Modulo(input,size);
		if(input < 0) {
			res += size;
		}
		return res;
	}

	static void AverageVertexNormalsByUVPosition(FDynamicMesh3& Mesh)
	{
		if (!Mesh.HasAttributes()){	return;	}
		TMap<FVector2f, TPair<FVector3f, int32>> UVPositionToNormalSumAndCount;

		for (auto VertexID : Mesh.VertexIndicesItr()) {
			FVector3f VertexNormal = Mesh.GetVertexNormal(VertexID);
			FVector2f UVPosition = Mesh.GetVertexUV(VertexID);
			auto& NormalSumAndCount = UVPositionToNormalSumAndCount.FindOrAdd(UVPosition);
		
			NormalSumAndCount.Key += VertexNormal;
			NormalSumAndCount.Value += 1;
		}

		for (int32 VertexID : Mesh.VertexIndicesItr()) {
			FVector2f UVPosition = Mesh.GetVertexUV(VertexID);
			const auto& NormalSumAndCount = UVPositionToNormalSumAndCount[UVPosition];
			FVector3f AveragedNormal = NormalSumAndCount.Key / (double)NormalSumAndCount.Value;
		
			AveragedNormal.Normalize();
			Mesh.SetVertexNormal(VertexID, AveragedNormal);
		}
	}
};
