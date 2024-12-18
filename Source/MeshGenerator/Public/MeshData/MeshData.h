#pragma once

#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "MeshGenerator.h"
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
	TArray<FVector> BorderVertices;

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

	void AddBorderVertex(FVector Position)
	{
		BorderVertices.Add(Position);
	}

	void CalculateTangents()
	{
		RemoveRedundantVertices(Vertices, UV0, Triangles);
		UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices,Triangles,UV0,Normals,Tangents);
	}

	void CreateProceduralMesh(UProceduralMeshComponent* ProceduralMeshComponent, int section = 0, bool collision = true) const
	{
		double start = FPlatformTime::Seconds();
		
		ProceduralMeshComponent->ClearMeshSection(section);
		ProceduralMeshComponent->CreateMeshSection(section,Vertices,Triangles,Normals,UV0,TArray<FColor>(),Tangents,collision);
		
		double end = FPlatformTime::Seconds();
		UE_LOG(MeshGenerator, Verbose, TEXT("MeshData::CreateProceduralMesh() => Runtime: %fs "), (end - start));
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

	//I think this needs review
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
	
	void GenerateSmoothedNormalsWithBorders(
		// const TArray<FVector>& Vertices,          // Main mesh vertices
		// const TArray<int32>& Triangles,           // Triangles for main mesh + border
		// const TArray<FVector2D>& UV0,             // UVs for main mesh + border
		// const TArray<FVector>& BorderVertices,    // Border vertices for smoothing
		TArray<FVector>& OutNormals,              // Output normals for main mesh vertices only
		TArray<FProcMeshTangent>& OutTangents     // Output tangents for main mesh vertices only
	)
	{
		// Combine main vertices and border vertices
		TArray<FVector> CombinedVertices = Vertices;
		CombinedVertices.Append(BorderVertices);

		// Create combined triangles that refer to the combined vertex array
		TArray<int32> CombinedTriangles = Triangles;

		// Ensure output arrays are cleared
		OutNormals.Empty();
		OutTangents.Empty();

		// Create a temporary array for normals, one entry per combined vertex
		TArray<FVector> CombinedNormals;
		CombinedNormals.SetNumZeroed(CombinedVertices.Num());

		// Create a temporary array to count contributions to each normal
		TArray<int32> NormalCounts;
		NormalCounts.Init(0, CombinedVertices.Num());

		// Calculate face normals and accumulate at each vertex
		for (int32 i = 0; i < CombinedTriangles.Num(); i += 3)
		{
			int32 Index0 = CombinedTriangles[i];
			int32 Index1 = CombinedTriangles[i + 1];
			int32 Index2 = CombinedTriangles[i + 2];

			if (CombinedVertices.IsValidIndex(Index0) &&
				CombinedVertices.IsValidIndex(Index1) &&
				CombinedVertices.IsValidIndex(Index2))
			{
				FVector Vertex0 = CombinedVertices[Index0];
				FVector Vertex1 = CombinedVertices[Index1];
				FVector Vertex2 = CombinedVertices[Index2];

				// Compute the face normal
				FVector Edge1 = Vertex1 - Vertex0;
				FVector Edge2 = Vertex2 - Vertex0;
				FVector FaceNormal = FVector::CrossProduct(Edge1, Edge2).GetSafeNormal();

				// Accumulate face normal for each vertex
				CombinedNormals[Index0] += FaceNormal;
				CombinedNormals[Index1] += FaceNormal;
				CombinedNormals[Index2] += FaceNormal;

				// Increment normal counts
				NormalCounts[Index0]++;
				NormalCounts[Index1]++;
				NormalCounts[Index2]++;
			}
		}

		// Normalize the accumulated normals
		for (int32 i = 0; i < CombinedNormals.Num(); i++)
		{
			if (NormalCounts[i] > 0)
			{
				CombinedNormals[i] = CombinedNormals[i].GetSafeNormal();
			}
		}

		// Copy the normals corresponding to the main mesh vertices only
		OutNormals.Append(CombinedNormals.GetData(), Vertices.Num());

		// Generate tangents using the normals for the main mesh vertices
		TArray<FVector2D> MainUV0 = UV0;  // Assuming UV0 only corresponds to the main mesh vertices
		UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, OutNormals, OutTangents);
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

	static void RemoveRedundantVertices(TArray<FVector>& Vertices, TArray<FVector2D>& UVs, TArray<int32>& Triangles)
	{
		TArray<FVector> UniqueVertices;
		TArray<FVector2D> UniqueUVs;
		TMap<FVector, int32> VertexToIndexMap; // Maps a vertex to its new index
		TArray<int32> RemappedTriangles;

		// Identify unique vertices and UVs
		for (int32 i = 0; i < Vertices.Num(); ++i)
		{
			const FVector& Vertex = Vertices[i];
			const FVector2D& UV = UVs[i];

			// Check if this vertex is already in the map
			int32* ExistingIndex = VertexToIndexMap.Find(Vertex);
			if (ExistingIndex)
			{
				// Check if UV matches as well
				if (UniqueUVs[*ExistingIndex] == UV)
				{
					// Exact match, no need to add new vertex
					continue;
				}
				else
				{
					// UV conflict: Create a new vertex to preserve UVs
					int32 NewIndex = UniqueVertices.Add(Vertex);
					UniqueUVs.Add(UV);
					VertexToIndexMap.Add(Vertex, NewIndex);
				}
			}
			else
			{
				// Add new unique vertex and UV
				int32 NewIndex = UniqueVertices.Add(Vertex);
				UniqueUVs.Add(UV);
				VertexToIndexMap.Add(Vertex, NewIndex);
			}
		}

		// Remap triangle indices
		for (int32 i = 0; i < Triangles.Num(); ++i)
		{
			const int32 OldIndex = Triangles[i];
			const FVector& OldVertex = Vertices[OldIndex];

			// Find the new index for the old vertex
			const int32* NewIndex = VertexToIndexMap.Find(OldVertex);
			if (NewIndex)
			{
				RemappedTriangles.Add(*NewIndex);
			}
			else
			{
				// This should not happen if the logic above is correct
				check(false && "Vertex remapping failed");
			}
		}

		// Replace old data with optimized data
		Vertices = UniqueVertices;
		UVs = UniqueUVs;
		Triangles = RemappedTriangles;
	}
};

