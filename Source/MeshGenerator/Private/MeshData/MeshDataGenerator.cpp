#include "MeshData/MeshDataGenerator.h"
#include "MeshGenerator.h"
#include "MeshData/MeshData.h"
#include "Noise.h"
#include "NoiseSettings.h"
#include "QuadTree/QuadTree.h"
#include "QuadTree/QuadTreeNode.h"
#include "MarchingCubes/MarchingCubes.h"


FMeshData UMeshDataGenerator::RectMesh(FVector position, FVector size, FVector2D segments, float UVScale, FNoiseSettings* NoiseSettings, int heightMultiplier)
{
	FMeshData MeshData = FMeshData();
	MeshData.UVScale = UVScale;
	
	float xStep = size.X / segments.X;
	float yStep = size.Y / segments.Y; 
	
	for (int x = 0; x <= segments.X; x++) {
		for(int y = 0; y <= segments.Y; y++) {
			if(NoiseSettings != NULL) {
				FVector pos = FVector(position.X+(xStep*x),position.Y+(yStep*y),NoiseSettings->seed);
				FVector lpos = FVector((xStep*x),(yStep*y),0);
				MeshData.AddVertex(FVector(lpos.X,lpos.Y,UNoise::EvaluateHeight(pos,NoiseSettings,heightMultiplier)));
			} else {
				MeshData.AddVertex (FVector((xStep*x),(yStep*y),0));
			}

			if(x < segments.X && y < segments.Y) {
				int topLeft = y * (segments.X + 1) + x;
				int bottomLeft = (y + 1) * (segments.Y + 1) + x;
				int topRight = topLeft + 1;
				int bottomRight = bottomLeft + 1;
			
				MeshData.Triangles.Add(topLeft);
				MeshData.Triangles.Add(topRight);
				MeshData.Triangles.Add(bottomLeft);
			
				MeshData.Triangles.Add(bottomLeft);
				MeshData.Triangles.Add(topRight);
				MeshData.Triangles.Add(bottomRight);	
			}
		}
	}

	MeshData.CalculateTangents();

	return MeshData;
}

FMeshData UMeshDataGenerator::QuadTreeMesh(QuadTree* QTree, float UVScale, int depthFilter, FNoiseSettings* NoiseSettings, int heightMultiplier)
{
	FMeshData MeshData = FMeshData();
	MeshData.UVScale = UVScale;
	FVector posCorner = QTree->Position + QTree->Size;
	for (int i = 0; i < QTree->Leaves.Num(); i++) {
		if(depthFilter != 0 && QTree->Leaves[i].Depth < depthFilter){ continue; }

		//Corners
		double posX = QTree->Leaves[i].Center.X + QTree->Leaves[i].Size.X * 0.5f;
		double negX = QTree->Leaves[i].Center.X - QTree->Leaves[i].Size.X * 0.5f;
		double posY = QTree->Leaves[i].Center.Y + QTree->Leaves[i].Size.Y * 0.5f;
		double negY = QTree->Leaves[i].Center.Y - QTree->Leaves[i].Size.Y * 0.5f;

		double lzeroX = FMeshData::LocalizePos(QTree->Leaves[i].Center.X,QTree->Size.X);
		double lzeroY = FMeshData::LocalizePos(QTree->Leaves[i].Center.Y,QTree->Size.Y);
		double lposX = (posX == posCorner.X ? QTree->Size.X : FMeshData::LocalizePos(QTree->Leaves[i].Center.X + QTree->Leaves[i].Size.X * 0.5f,QTree->Size.X));
		double lnegX = (negX == QTree->Position.X ? 0 : FMeshData::LocalizePos(QTree->Leaves[i].Center.X - QTree->Leaves[i].Size.X * 0.5f,QTree->Size.X));
		double lposY = (posY == posCorner.Y ? QTree->Size.Y : FMeshData::LocalizePos(QTree->Leaves[i].Center.Y + QTree->Leaves[i].Size.Y * 0.5f,QTree->Size.Y));
		double lnegY = (negY == QTree->Position.Y ? 0 : FMeshData::LocalizePos(QTree->Leaves[i].Center.Y - QTree->Leaves[i].Size.Y * 0.5f,QTree->Size.Y));

		double z = NoiseSettings == NULL ? 0 : UNoise::EvaluateHeight(FVector(QTree->Leaves[i].Center.X,QTree->Leaves[i].Center.Y,NoiseSettings->seed),NoiseSettings,heightMultiplier);
		MeshData.AddVertex(FVector(lzeroX,lzeroY,z));
		int zeroIndex = MeshData.Vertices.Num() - 1;
		
		double z1 = NoiseSettings == NULL ? 0 : UNoise::EvaluateHeight(FVector(negX,posY,NoiseSettings->seed),NoiseSettings,heightMultiplier);
		MeshData.AddVertex(FVector(lnegX,lposY,z1));
		double z3 = NoiseSettings == NULL ? 0 : UNoise::EvaluateHeight(FVector(posX,posY,NoiseSettings->seed),NoiseSettings,heightMultiplier);
		MeshData.AddVertex(FVector(lposX,lposY,z3));
		double z5 = NoiseSettings == NULL ? 0 : UNoise::EvaluateHeight(FVector(posX,negY,NoiseSettings->seed),NoiseSettings,heightMultiplier);
		MeshData.AddVertex(FVector(lposX,lnegY,z5));
		double z7 = NoiseSettings == NULL ? 0 : UNoise::EvaluateHeight(FVector(negX,negY,NoiseSettings->seed),NoiseSettings,heightMultiplier);
		MeshData.AddVertex(FVector(lnegX,lnegY,z7));
		int index = zeroIndex + 4;

		if(QTree->Leaves[i].Neighbors[2] || fmod(posY, QTree->Size.Y) == 0) //North
		{
			double z2 = NoiseSettings == NULL ? 0 : UNoise::EvaluateHeight(FVector(QTree->Leaves[i].Center.X,posY,NoiseSettings->seed),NoiseSettings,heightMultiplier);
			MeshData.AddVertex(FVector(lzeroX,lposY,z2));
			index++;
			MeshData.AddTriangle(zeroIndex+1,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+2,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+1,zeroIndex+2,zeroIndex);
		}
		if(QTree->Leaves[i].Neighbors[0] || fmod(posX, QTree->Size.X) == 0) // East
		{
			double z4 = NoiseSettings == NULL ? 0 : UNoise::EvaluateHeight(FVector(posX,QTree->Leaves[i].Center.Y,NoiseSettings->seed),NoiseSettings,heightMultiplier);
			MeshData.AddVertex(FVector(lposX,lzeroY,z4));
			index++;
			MeshData.AddTriangle(zeroIndex+2,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+3,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+2,zeroIndex+3,zeroIndex);
		}
		if(QTree->Leaves[i].Neighbors[3] || fmod(negY, QTree->Size.Y) == 0) // South 
		{
			double z6 = NoiseSettings == NULL ? 0 : UNoise::EvaluateHeight(FVector(QTree->Leaves[i].Center.X,negY,NoiseSettings->seed),NoiseSettings,heightMultiplier);
			MeshData.AddVertex(FVector(lzeroX,lnegY,z6));
			index++;
			MeshData.AddTriangle(zeroIndex+3,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+4,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+3,zeroIndex+4,zeroIndex);
		}
		if(QTree->Leaves[i].Neighbors[1] || fmod(negX, QTree->Size.X) == 0) // West 
		{
			double z8 = NoiseSettings == NULL ? 0 : UNoise::EvaluateHeight(FVector(negX,QTree->Leaves[i].Center.Y,NoiseSettings->seed),NoiseSettings,heightMultiplier);
			MeshData.AddVertex(FVector(lnegX,lzeroY,z8));
			index++;
			MeshData.AddTriangle(zeroIndex+4,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+1,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+4,zeroIndex+1,zeroIndex);
		}
	}

	MeshData.CalculateTangents();

	return MeshData;
}

FMeshData UMeshDataGenerator::MarchingCubes(FNoiseMap3d NoiseMap, FVector position, FVector size, float UVScale, float isoLevel, bool interpolate, bool renderSides)
{
	double start = FPlatformTime::Seconds();
	int stepSize = 100;
	FMeshData MeshData = FMeshData();
	MeshData.UVScale = UVScale;

	// FNoiseMap3d NoiseMap = UNoise::GenerateMap3D((FIntVector)position, (FIntVector)size+FIntVector(1,1,1), NoiseSettings, NoiseDensityFunction::Floor);

	for (int x = 0; x < size.X-1; x++) {
		for (int y = 0; y < size.Y-1; y++) {
			for (int z = 0; z < size.Z-1; z++) {
				FVector pos = FVector(x,y,z);
				FVector sample = (position + pos);
				
				TArray<float> cubeValues = TArray<float>();
				for (int i = 0; i < UMarchingCubes::cornerOffsets.Num(); i++) {
					FIntVector mp =(FIntVector) (pos + UMarchingCubes::cornerOffsets[i]);
					FIntVector s = (FIntVector)(sample + UMarchingCubes::cornerOffsets[i]);
					if (renderSides && (mp.X >= size.X-1 || mp.Y >= size.Y-1 || mp.Z >= size.Z-1 || mp.X <= 0 || mp.Y <= 0 || mp.Z <= 0)) { cubeValues.Add(isoLevel); }
					else{ cubeValues.Add(NoiseMap.Map[s]); }
				}

				int cubeIndex = 0;
				for (int i = 0; i < cubeValues.Num(); i++){
					if (cubeValues[i] < isoLevel) {
						cubeIndex |= 1 << i;
					}
				}
				
				TArray<int> edges = UMarchingCubes::triTable[cubeIndex];

				for (int i = 0; edges[i] != -1; i += 3) {
					int e00 = UMarchingCubes::edgeConnections[edges[i]][0];
					int e01 = UMarchingCubes::edgeConnections[edges[i]][1];

					int e10 = UMarchingCubes::edgeConnections[edges[i + 1]][0];
					int e11 = UMarchingCubes::edgeConnections[edges[i + 1]][1];

					int e20 = UMarchingCubes::edgeConnections[edges[i + 2]][0];
					int e21 = UMarchingCubes::edgeConnections[edges[i + 2]][1];

					FVector a;
					FVector b;
					FVector c;
					
					if (interpolate) {
						a = Interp(UMarchingCubes::cornerOffsets[e00], cubeValues[e00], UMarchingCubes::cornerOffsets[e01], cubeValues[e01], isoLevel) + pos;
						b = Interp(UMarchingCubes::cornerOffsets[e10], cubeValues[e10], UMarchingCubes::cornerOffsets[e11], cubeValues[e11], isoLevel) + pos;
						c = Interp(UMarchingCubes::cornerOffsets[e20], cubeValues[e20], UMarchingCubes::cornerOffsets[e21], cubeValues[e21], isoLevel) + pos;
					} else {
						a = Default(UMarchingCubes::cornerOffsets[e00],UMarchingCubes::cornerOffsets[e01]) + pos;
						b = Default(UMarchingCubes::cornerOffsets[e10],UMarchingCubes::cornerOffsets[e11]) + pos;
						c = Default(UMarchingCubes::cornerOffsets[e20],UMarchingCubes::cornerOffsets[e21]) + pos;
					}
					MeshData.AddTriangle(a, b, c);
				}
			}
		}
	}

	//SUPER EXPENSIVE HERE (and everywhere)--- needs alternatives / optimization???
	MeshData.CalculateTangents();
	
	double end = FPlatformTime::Seconds();
	UE_LOG(MeshGenerator, Log, TEXT("MarchingCubes() - V:%d  Runtime: %f"), MeshData.Vertices.Num(), end - start);
	
	return MeshData;
}
FVector UMeshDataGenerator::Interp(FVector edgeVertex1, float valueAtVertex1, FVector edgeVertex2, float valueAtVertex2, float isoLevel) {
	return (edgeVertex1 + (isoLevel - valueAtVertex1) * (edgeVertex2 - edgeVertex1) / (valueAtVertex2 - valueAtVertex1));
}

FVector UMeshDataGenerator::Default(FVector edgeVertex1, FVector edgeVertex2) {
	return (edgeVertex1 + edgeVertex2) / 2;
}