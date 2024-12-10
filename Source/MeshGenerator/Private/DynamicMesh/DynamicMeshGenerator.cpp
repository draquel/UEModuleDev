#include "DynamicMeshGenerator.h"

#include "Components/DynamicMeshComponent.h"
#include "Noise.h"
#include "QuadTree/QuadTree.h"
#include "DynamicMesh/MeshAttributeUtil.h"
#include "DynamicMesh/MeshNormals.h"
#include "MeshData/MeshData.h"


struct FNoiseSettings;

FDynamicMesh3 DynamicMeshGenerator::RectMesh(FVector position, FVector size, FVector2D segments, float UVScale, FNoiseSettings* NoiseSettings, int heightMultiplier)
{
	FDynamicMesh3 DynamicMesh (true,true,true,true);
	DynamicMesh.EnableAttributes();
	DynamicMesh.EnableVertexUVs(FVector2f::Zero());
	
	float xStep = size.X / segments.X;
	float yStep = size.Y / segments.Y; 

	int vertIndex = 0;
	for (int x = 0; x <= segments.X; x++) {
		for(int y = 0; y <= segments.Y; y++) {
			if(NoiseSettings != NULL) {
				FVector pos = FVector(position.X+(xStep*x),position.Y+(yStep*y),NoiseSettings->seed);
				FVector lpos = FVector((xStep*x),(yStep*y),0);
				DynamicMesh.AppendVertex(FVector(lpos.X,lpos.Y,UNoise::Evaluate2D(pos,NoiseSettings) * heightMultiplier));
				DynamicMesh.SetVertexUV(vertIndex,FVector2f(lpos.X,lpos.Y)*UVScale);
			} else {
				DynamicMesh.AppendVertex(FVector((xStep*x),(yStep*y),0));
				DynamicMesh.SetVertexUV(vertIndex,FVector2f((xStep*x),(yStep*y))*UVScale);
			}
			vertIndex++;
		}
	}
	for (int x = 0; x < segments.X; x++) {
		for(int y = 0; y < segments.Y; y++)	{
			int topLeft = y * (segments.X + 1) + x;
			int bottomLeft = (y + 1) * (segments.Y + 1) + x;
			int topRight = topLeft + 1;
			int bottomRight = bottomLeft + 1;
			
			DynamicMesh.AppendTriangle(topLeft,topRight,bottomLeft);
			DynamicMesh.AppendTriangle(bottomLeft,topRight,bottomRight);		
		}
	}
	
	UE::Geometry::FMeshNormals::QuickComputeVertexNormals(DynamicMesh);
	DynamicMeshGenerator::AverageVertexNormalsByUVPosition(DynamicMesh);
	UE::Geometry::CopyVertexUVsToOverlay(DynamicMesh, *DynamicMesh.Attributes()->PrimaryUV());	

	return DynamicMesh;
}

FDynamicMesh3 DynamicMeshGenerator::QuadTreeMesh(QuadTree* QTree, FVector playerPos, float UVScale, int depthFilter, FNoiseSettings* NoiseSettings, int heightMultiplier)
{
	FDynamicMesh3 DynamicMesh (true,true,true,true);
	DynamicMesh.EnableAttributes();
	DynamicMesh.EnableVertexNormals(FVector3f::Zero());
	DynamicMesh.EnableVertexUVs(FVector2f::Zero());

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

		int zeroIndex = DynamicMesh.VertexCount();
		double z = NoiseSettings == NULL ? 0 : UNoise::Evaluate2D(FVector(QTree->Leaves[i].Center.X,QTree->Leaves[i].Center.Y,NoiseSettings->seed),NoiseSettings) * heightMultiplier;
		DynamicMesh.AppendVertex(FVector(lzeroX,lzeroY,z));
		DynamicMesh.SetVertexUV(zeroIndex,FVector2f(lzeroX,lzeroY)*UVScale);
		
		double z1 = NoiseSettings == NULL ? 0 : UNoise::Evaluate2D(FVector(negX,posY,NoiseSettings->seed),NoiseSettings) * heightMultiplier;
		DynamicMesh.AppendVertex(FVector(lnegX,lposY,z1));
		DynamicMesh.SetVertexUV(zeroIndex+1,FVector2f(lnegX,lposY)*UVScale);	
		double z3 = NoiseSettings == NULL ? 0 : UNoise::Evaluate2D(FVector(posX,posY,NoiseSettings->seed),NoiseSettings) * heightMultiplier;
		DynamicMesh.AppendVertex(FVector(lposX,lposY,z3));
		DynamicMesh.SetVertexUV(zeroIndex+2,FVector2f(lposX,lposY)*UVScale);
		double z5 = NoiseSettings == NULL ? 0 : UNoise::Evaluate2D(FVector(posX,negY,NoiseSettings->seed),NoiseSettings) * heightMultiplier;
		DynamicMesh.AppendVertex(FVector(lposX,lnegY,z5));
		DynamicMesh.SetVertexUV(zeroIndex+3,FVector2f(lposX,lnegY)*UVScale);
		double z7 = NoiseSettings == NULL ? 0 : UNoise::Evaluate2D(FVector(negX,negY,NoiseSettings->seed),NoiseSettings) * heightMultiplier;
		DynamicMesh.AppendVertex(FVector(lnegX,lnegY,z7));
		DynamicMesh.SetVertexUV(zeroIndex+4,FVector2f(lnegX,lnegY)*UVScale);
		int index = zeroIndex + 4;

		if(QTree->Leaves[i].Neighbors[2] ||	(fmod(posY, QTree->Size.Y) == 0 /*&& FVector::Distance(playerPos,QTree->Leaves[i].Center+FVector(0,QTree->Leaves[i].Size.Y*2,0)) > QTree->Leaves[i].Size.Y*QTree->Settings.DistanceModifier*/)) //North
		{
			double z2 = NoiseSettings == NULL ? 0 : UNoise::Evaluate2D(FVector(QTree->Leaves[i].Center.X,posY,NoiseSettings->seed),NoiseSettings) * heightMultiplier;
			index++;
			DynamicMesh.AppendVertex(FVector(lzeroX,lposY,z2));
			DynamicMesh.SetVertexUV(index,FVector2f(lzeroX,lposY)*UVScale);
			DynamicMesh.AppendTriangle(zeroIndex+1,index,zeroIndex);
			DynamicMesh.AppendTriangle(index,zeroIndex+2,zeroIndex);			
		}else {
			DynamicMesh.AppendTriangle(zeroIndex+1,zeroIndex+2,zeroIndex);
		}
		if(QTree->Leaves[i].Neighbors[0] || fmod(posX, QTree->Size.X) == 0) // East
		{
			double z4 = NoiseSettings == NULL ? 0 : UNoise::Evaluate2D(FVector(posX,QTree->Leaves[i].Center.Y,NoiseSettings->seed),NoiseSettings) * heightMultiplier;
			index++;
			DynamicMesh.AppendVertex(FVector(lposX,lzeroY,z4));
			DynamicMesh.SetVertexUV(index,FVector2f(lposX,lzeroY)*UVScale);
			DynamicMesh.AppendTriangle(zeroIndex+2,index,zeroIndex);
			DynamicMesh.AppendTriangle(index,zeroIndex+3,zeroIndex);			
		}else {
			DynamicMesh.AppendTriangle(zeroIndex+2,zeroIndex+3,zeroIndex);
		}
		if(QTree->Leaves[i].Neighbors[3] || fmod(negY, QTree->Size.Y) == 0) // South 
		{
			double z6 = NoiseSettings == NULL ? 0 : UNoise::Evaluate2D(FVector(QTree->Leaves[i].Center.X,negY,NoiseSettings->seed),NoiseSettings) * heightMultiplier;
			index++;
			DynamicMesh.AppendVertex(FVector(lzeroX,lnegY,z6));
			DynamicMesh.SetVertexUV(index,FVector2f(lzeroX,lnegY)*UVScale);
			DynamicMesh.AppendTriangle(zeroIndex+3,index,zeroIndex);
			DynamicMesh.AppendTriangle(index,zeroIndex+4,zeroIndex);			
		}else {
			DynamicMesh.AppendTriangle(zeroIndex+3,zeroIndex+4,zeroIndex);
		}
		if(QTree->Leaves[i].Neighbors[1] || fmod(negX, QTree->Size.X) == 0) // West 
		{
			double z8 = NoiseSettings == NULL ? 0 : UNoise::Evaluate2D(FVector(negX,QTree->Leaves[i].Center.Y,NoiseSettings->seed),NoiseSettings) * heightMultiplier;
			index++;
			DynamicMesh.AppendVertex(FVector(lnegX,lzeroY,z8));
			DynamicMesh.SetVertexUV(index,FVector2f(lnegX,lzeroY)*UVScale);
			DynamicMesh.AppendTriangle(zeroIndex+4,index,zeroIndex);
			DynamicMesh.AppendTriangle(index,zeroIndex+1,zeroIndex);			
		}else {
			DynamicMesh.AppendTriangle(zeroIndex+4,zeroIndex+1,zeroIndex);
		}
	}

	UE::Geometry::FMeshNormals::QuickComputeVertexNormals(DynamicMesh);
	DynamicMeshGenerator::AverageVertexNormalsByUVPosition(DynamicMesh);
	
	UE::Geometry::CopyVertexNormalsToOverlay(DynamicMesh,*DynamicMesh.Attributes()->PrimaryNormals());
	UE::Geometry::CopyVertexUVsToOverlay(DynamicMesh, *DynamicMesh.Attributes()->PrimaryUV());
	
	return DynamicMesh;
}

void DynamicMeshGenerator::AverageVertexNormalsByUVPosition(FDynamicMesh3& Mesh)
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