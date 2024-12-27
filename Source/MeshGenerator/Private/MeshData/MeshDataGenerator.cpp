#include "MeshData/MeshDataGenerator.h"
#include "MeshGenerator.h"
#include "MeshData/MeshData.h"
#include "Noise.h"
#include "NoiseSettings.h"
#include "TextureUtils.h"
#include "QuadTree/QuadTree.h"
#include "QuadTree/QuadTreeNode.h"
#include "MarchingCubes/MarchingCubes.h"

/**
 *
 *	Rect Mesh
 *
 **/

FMeshData UMeshDataGenerator::RectMesh(FVector position, FVector size, FVector2D segments, float UVScale, FNoiseSettings* NoiseSettings, int heightMultiplier)
{
	FMeshData MeshData = FMeshData();
	MeshData.UVScale = UVScale;
	double start = FPlatformTime::Seconds();
	
	float xStep = size.X / segments.X;
	float yStep = size.Y / segments.Y; 
	
	for (int x = 0; x <= segments.X; x++) {
		for(int y = 0; y <= segments.Y; y++) {
			if(NoiseSettings != NULL) {
				FVector pos = FVector(position.X+(xStep*x),position.Y+(yStep*y),NoiseSettings->seed);
				FVector lpos = FVector((xStep*x),(yStep*y),0);
				MeshData.AddVertex(FVector(lpos.X,lpos.Y,UNoise::Evaluate2D(pos,NoiseSettings) * heightMultiplier));
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
	
	double end = FPlatformTime::Seconds();
	UE_LOG(MeshGenerator, Log, TEXT("MeshGenerator::RectMesh() ==> Verts:%d, Tris:%d  Runtime: %f"), MeshData.Vertices.Num(), MeshData.Triangles.Num(), end - start);
	return MeshData;
}

FMeshData UMeshDataGenerator::RectMesh(FNoiseMap2d* NoiseMap, FVector position, FVector size, int stepSize, float UVScale, int heightMultiplier)
{
	FMeshData MeshData = FMeshData();
	MeshData.UVScale = UVScale;
	double start = FPlatformTime::Seconds();

	FVector segments = size / stepSize;
	
	for (int x = 0; x <= segments.X; x++) {
		for(int y = 0; y <= segments.Y; y++) {
			if(NoiseMap != nullptr) {
				FVector lpos = FVector((stepSize*x),(stepSize*y),0);
				FIntVector2 pos = FIntVector2(position.X+(stepSize*x),position.Y+(stepSize*y));
				if (NoiseMap->Map.Contains(pos)) {
					MeshData.AddVertex(FVector(lpos.X,lpos.Y,NoiseMap->Map[pos] * heightMultiplier));
				}else {	UE_LOG(MeshGenerator,Error,TEXT("MeshGenerator::RectMesh() ==> NoiseMap does not contain Index: %s"),*pos.ToString()); return MeshData; }
			} else {
				MeshData.AddVertex (FVector((stepSize*x),(stepSize*y),0));
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
	
	double end = FPlatformTime::Seconds();
	UE_LOG(MeshGenerator, Log, TEXT("MeshGenerator::RectMesh() ==> Verts:%d, Tris:%d  Runtime: %f"), MeshData.Vertices.Num(), MeshData.Triangles.Num(), end - start);
	return MeshData;
}

FMeshData UMeshDataGenerator::RectMesh(UTexture2D* Texture, FVector position, FVector size, int StepSize, int TextureStepSize, float UVScale, int heightMultiplier)
{
	FMeshData MeshData = FMeshData();
	MeshData.UVScale = UVScale;
	double start = FPlatformTime::Seconds();

	FVector segments = size / StepSize;
	
	for (int x = 0; x <= segments.X; x++) {
		for(int y = 0; y <= segments.Y; y++) {
			if(Texture != nullptr) {
				FVector lpos = FVector((StepSize*x),(StepSize*y),0); //Mesh Local pos
				FIntVector2 spos = FIntVector2(lpos.X/TextureStepSize,lpos.Y/TextureStepSize); //Texture Sample Pos
				if (spos.X < 0 || spos.X > Texture->GetSizeX() || spos.Y < 0 || spos.Y > Texture->GetSizeY()) {
					UE_LOG(MeshGenerator,Error,TEXT("MeshGenerator::RectMesh() ==> Index: %s is outside the bounds of the source Texture."),*spos.ToString());
					return MeshData;	
				}
				float noise = UTextureUtils::SampleTextureR16f(Texture,spos);
				MeshData.AddVertex(FVector(lpos.X,lpos.Y,noise*heightMultiplier));	
			} else {
				MeshData.AddVertex (FVector((StepSize*x),(StepSize*y),0));
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
	
	double end = FPlatformTime::Seconds();
	UE_LOG(MeshGenerator, Log, TEXT("MeshGenerator::RectMesh() ==> Verts:%d, Tris:%d  Runtime: %f"), MeshData.Vertices.Num(), MeshData.Triangles.Num(), end - start);
	return MeshData;
}


/**
 *
 *	QuadTree Mesh
 *
 **/
FMeshData UMeshDataGenerator::QuadTreeMesh(UQuadTree* QTree, float UVScale, int depthFilter)
{
	FMeshData MeshData = FMeshData();
	MeshData.UVScale = UVScale;
	double start = FPlatformTime::Seconds();

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

		MeshData.AddVertex(FVector(lzeroX,lzeroY,0));
		int zeroIndex = MeshData.Vertices.Num() - 1;
		
		MeshData.AddVertex(FVector(lnegX,lposY,0));
		MeshData.AddVertex(FVector(lposX,lposY,0));
		MeshData.AddVertex(FVector(lposX,lnegY,0));
		MeshData.AddVertex(FVector(lnegX,lnegY,0));
		int index = zeroIndex + 4;

		if(QTree->Leaves[i].Neighbors[2] || fmod(posY, QTree->Size.Y) == 0) //North
		{
			MeshData.AddVertex(FVector(lzeroX,lposY,0));
			index++;
			MeshData.AddTriangle(zeroIndex+1,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+2,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+1,zeroIndex+2,zeroIndex);
		}
		if(QTree->Leaves[i].Neighbors[0] || fmod(posX, QTree->Size.X) == 0) // East
		{
			MeshData.AddVertex(FVector(lposX,lzeroY,0));
			index++;
			MeshData.AddTriangle(zeroIndex+2,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+3,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+2,zeroIndex+3,zeroIndex);
		}
		if(QTree->Leaves[i].Neighbors[3] || fmod(negY, QTree->Size.Y) == 0) // South 
		{
			MeshData.AddVertex(FVector(lzeroX,lnegY,0));
			index++;
			MeshData.AddTriangle(zeroIndex+3,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+4,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+3,zeroIndex+4,zeroIndex);
		}
		if(QTree->Leaves[i].Neighbors[1] || fmod(negX, QTree->Size.X) == 0) // West 
		{
			MeshData.AddVertex(FVector(lnegX,lzeroY,0));
			index++;
			MeshData.AddTriangle(zeroIndex+4,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+1,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+4,zeroIndex+1,zeroIndex);
		}
	}

	MeshData.CalculateTangents();

	double end = FPlatformTime::Seconds();
	UE_LOG(MeshGenerator, Log, TEXT("MeshGenerator::QuadTreeMesh() ==> Verts: %d, Tris: %d  Runtime: %f"), MeshData.Vertices.Num(), MeshData.Triangles.Num(), end - start);
	return MeshData;
}

FMeshData UMeshDataGenerator::QuadTreeMesh(UQuadTree* QTree, float UVScale, int depthFilter, FNoiseSettings* NoiseSettings, int heightMultiplier)
{
	FMeshData MeshData = FMeshData();
	MeshData.UVScale = UVScale;
	double start = FPlatformTime::Seconds();

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

		double z = NoiseSettings == NULL ? 0 : UNoise::Evaluate2D(FVector(QTree->Leaves[i].Center.X,QTree->Leaves[i].Center.Y,NoiseSettings->seed),NoiseSettings) * heightMultiplier;
		MeshData.AddVertex(FVector(lzeroX,lzeroY,z));
		int zeroIndex = MeshData.Vertices.Num() - 1;
		
		double z1 = NoiseSettings == NULL ? 0 : UNoise::Evaluate2D(FVector(negX,posY,NoiseSettings->seed),NoiseSettings) * heightMultiplier;
		MeshData.AddVertex(FVector(lnegX,lposY,z1));
		double z3 = NoiseSettings == NULL ? 0 : UNoise::Evaluate2D(FVector(posX,posY,NoiseSettings->seed),NoiseSettings) * heightMultiplier;
		MeshData.AddVertex(FVector(lposX,lposY,z3));
		double z5 = NoiseSettings == NULL ? 0 : UNoise::Evaluate2D(FVector(posX,negY,NoiseSettings->seed),NoiseSettings) * heightMultiplier;
		MeshData.AddVertex(FVector(lposX,lnegY,z5));
		double z7 = NoiseSettings == NULL ? 0 : UNoise::Evaluate2D(FVector(negX,negY,NoiseSettings->seed),NoiseSettings) * heightMultiplier;
		MeshData.AddVertex(FVector(lnegX,lnegY,z7));
		int index = zeroIndex + 4;

		if(QTree->Leaves[i].Neighbors[2] || fmod(posY, QTree->Size.Y) == 0) //North
		{
			double z2 = NoiseSettings == NULL ? 0 : UNoise::Evaluate2D(FVector(QTree->Leaves[i].Center.X,posY,NoiseSettings->seed),NoiseSettings) * heightMultiplier;
			MeshData.AddVertex(FVector(lzeroX,lposY,z2));
			index++;
			MeshData.AddTriangle(zeroIndex+1,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+2,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+1,zeroIndex+2,zeroIndex);
		}
		if(QTree->Leaves[i].Neighbors[0] || fmod(posX, QTree->Size.X) == 0) // East
		{
			double z4 = NoiseSettings == NULL ? 0 : UNoise::Evaluate2D(FVector(posX,QTree->Leaves[i].Center.Y,NoiseSettings->seed),NoiseSettings) * heightMultiplier;
			MeshData.AddVertex(FVector(lposX,lzeroY,z4));
			index++;
			MeshData.AddTriangle(zeroIndex+2,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+3,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+2,zeroIndex+3,zeroIndex);
		}
		if(QTree->Leaves[i].Neighbors[3] || fmod(negY, QTree->Size.Y) == 0) // South 
		{
			double z6 = NoiseSettings == NULL ? 0 : UNoise::Evaluate2D(FVector(QTree->Leaves[i].Center.X,negY,NoiseSettings->seed),NoiseSettings) * heightMultiplier;
			MeshData.AddVertex(FVector(lzeroX,lnegY,z6));
			index++;
			MeshData.AddTriangle(zeroIndex+3,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+4,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+3,zeroIndex+4,zeroIndex);
		}
		if(QTree->Leaves[i].Neighbors[1] || fmod(negX, QTree->Size.X) == 0) // West 
		{
			double z8 = NoiseSettings == NULL ? 0 : UNoise::Evaluate2D(FVector(negX,QTree->Leaves[i].Center.Y,NoiseSettings->seed),NoiseSettings) * heightMultiplier;
			MeshData.AddVertex(FVector(lnegX,lzeroY,z8));
			index++;
			MeshData.AddTriangle(zeroIndex+4,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+1,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+4,zeroIndex+1,zeroIndex);
		}
	}

	MeshData.CalculateTangents();

	double end = FPlatformTime::Seconds();
	UE_LOG(MeshGenerator, Log, TEXT("MeshGenerator::QuadTreeMesh() ==> Verts: %d, Tris: %d  Runtime: %f"), MeshData.Vertices.Num(), MeshData.Triangles.Num(), end - start);
	return MeshData;
}

FMeshData UMeshDataGenerator::QuadTreeMesh(FNoiseMap2d* NoiseMap, UQuadTree* QTree, float UVScale, int depthFilter, int heightMultiplier)
{
	FMeshData MeshData = FMeshData();
	MeshData.UVScale = UVScale;
	double start = FPlatformTime::Seconds();

	//Alignment Error
	if (QTree->Settings.MinSize / NoiseMap->StepSize < 2) {
		UE_LOG(MeshGenerator,Error,TEXT("MeshGenerator::OuadTreeMesh ==> The QuadTree.Settings.MinSize[%d] must be at minimum twice the FNoiseMap2D.StepSize[%d] to guarantee sample availability."),QTree->Settings.MinSize,NoiseMap->StepSize);
		return MeshData;	
	}

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

		double z = NoiseMap->Map[FIntVector2(QTree->Leaves[i].Center.X,QTree->Leaves[i].Center.Y)] * heightMultiplier;
		MeshData.AddVertex(FVector(lzeroX,lzeroY,z));
		int zeroIndex = MeshData.Vertices.Num() - 1;
		
		double z1 = NoiseMap->Map[FIntVector2(negX,posY)] * heightMultiplier;
		MeshData.AddVertex(FVector(lnegX,lposY,z1));
		double z3 = NoiseMap->Map[FIntVector2(posX,posY)] * heightMultiplier;
		MeshData.AddVertex(FVector(lposX,lposY,z3));
		double z5 = NoiseMap->Map[FIntVector2(posX,negY)] * heightMultiplier;
		MeshData.AddVertex(FVector(lposX,lnegY,z5));
		double z7 = NoiseMap->Map[FIntVector2(negX,negY)] * heightMultiplier;
		MeshData.AddVertex(FVector(lnegX,lnegY,z7));
		int index = zeroIndex + 4;

		if(QTree->Leaves[i].Neighbors[2] || fmod(posY, QTree->Size.Y) == 0) //North
		{
			double z2 = NoiseMap->Map[FIntVector2(QTree->Leaves[i].Center.X,posY)] * heightMultiplier;
			MeshData.AddVertex(FVector(lzeroX,lposY,z2));
			index++;
			MeshData.AddTriangle(zeroIndex+1,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+2,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+1,zeroIndex+2,zeroIndex);
		}
		if(QTree->Leaves[i].Neighbors[0] || fmod(posX, QTree->Size.X) == 0) // East
		{
			double z4 = NoiseMap->Map[FIntVector2(posX,QTree->Leaves[i].Center.Y)] * heightMultiplier;
			MeshData.AddVertex(FVector(lposX,lzeroY,z4));
			index++;
			MeshData.AddTriangle(zeroIndex+2,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+3,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+2,zeroIndex+3,zeroIndex);
		}
		if(QTree->Leaves[i].Neighbors[3] || fmod(negY, QTree->Size.Y) == 0) // South 
		{
			double z6 = NoiseMap->Map[FIntVector2(QTree->Leaves[i].Center.X,negY)] * heightMultiplier;
			MeshData.AddVertex(FVector(lzeroX,lnegY,z6));
			index++;
			MeshData.AddTriangle(zeroIndex+3,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+4,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+3,zeroIndex+4,zeroIndex);
		}
		if(QTree->Leaves[i].Neighbors[1] || fmod(negX, QTree->Size.X) == 0) // West 
		{
			double z8 = NoiseMap->Map[FIntVector2(negX,QTree->Leaves[i].Center.Y)] * heightMultiplier;
			MeshData.AddVertex(FVector(lnegX,lzeroY,z8));
			index++;
			MeshData.AddTriangle(zeroIndex+4,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+1,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+4,zeroIndex+1,zeroIndex);
		}
	}

	MeshData.CalculateTangents();

	double end = FPlatformTime::Seconds();
	UE_LOG(MeshGenerator, Log, TEXT("MeshGenerator::QuadTreeMesh() ==> Verts: %d, Tris: %d  Runtime: %f"), MeshData.Vertices.Num(), MeshData.Triangles.Num(), end - start);
	return MeshData;
}

FMeshData UMeshDataGenerator::QuadTreeMesh(UTexture2D* Texture, UQuadTree* QTree, int StepSize, float UVScale, int depthFilter, int heightMultiplier)
{
	FMeshData MeshData = FMeshData();
	MeshData.UVScale = UVScale;
	double start = FPlatformTime::Seconds();
	bool border = true;
	
	//Alignment Error
	if (QTree->Settings.MinSize / StepSize < 2) {
		UE_LOG(MeshGenerator,Error,TEXT("MeshGenerator::OuadTreeMesh ==> The QuadTree.Settings.MinSize[%d] must be at minimum twice the FNoiseMap2D.StepSize[%d] to guarantee sample availability."),QTree->Settings.MinSize,StepSize);
		return MeshData;	
	}
	//Texture Size error
	// if(!border && (Texture->GetSizeX() != QTree->Size.X/StepSize || Texture->GetSizeY() != QTree->Size.Y/StepSize)){
	// 	UE_LOG(MeshGenerator,Error,TEXT("MeshGenerator::OuadTreeMesh ==> The provided Texture %s is not of the expected size %s."),*FIntVector2(Texture->GetSizeX(),Texture->GetSizeY()).ToString(),*FIntVector2((QTree->Size.X/StepSize),(QTree->Size.Y/StepSize)).ToString());
	// }
	// if(border && (Texture->GetSizeX() != (QTree->Size.X/StepSize)+2 || Texture->GetSizeY() != (QTree->Size.Y/StepSize)+2)){
	// 	
	// 	UE_LOG(MeshGenerator,Error,TEXT("MeshGenerator::OuadTreeMesh ==> The provided Texture %s is not of the expected size %s."),*FIntVector2(Texture->GetSizeX(),Texture->GetSizeY()).ToString(),*FIntVector2((QTree->Size.X/StepSize)+2,(QTree->Size.Y/StepSize)+2).ToString());
	// }

	//Borders
	FVector posCorner = QTree->Position + QTree->Size;
	
	// for (int i = -StepSize; i < QTree->Size.X+StepSize; i+=StepSize){ //for each x add border y min and max verts
	// 	FVector negY = QTree->Position + FVector(i,-StepSize,0);
	// 	FVector posY = QTree->Position + FVector(i,QTree->Size.Y+StepSize,0);
	// 	MeshData.AddBorderVertex( QTree->Position + FVector(i,-StepSize,SampleTexture(Texture,FIntVector2(negY.X,negY.Y)/StepSize) * heightMultiplier));
	// 	MeshData.AddBorderVertex( QTree->Position + FVector(i,QTree->Size.Y+StepSize,SampleTexture(Texture,FIntVector2(posY.X,posY.Y)/StepSize) * heightMultiplier));
	// }
	// for (int j = 0; j < QTree->Size.Y; j+=StepSize)	{ //for each y add x min and max verts{
	// 	FVector negX = QTree->Position + FVector(-StepSize,j,0); 
	// 	FVector posX = QTree->Position + FVector(QTree->Size.Y+StepSize,j,0);
	// 	MeshData.AddBorderVertex( QTree->Position + FVector(-StepSize,j,SampleTexture(Texture,FIntVector2(negX.X,negX.Y)/StepSize) * heightMultiplier));
	// 	MeshData.AddBorderVertex( QTree->Position + FVector(QTree->Size.X+StepSize,j,SampleTexture(Texture,FIntVector2(posX.X,posX.Y)/StepSize) * heightMultiplier));	
	// }

	//QTree Bounds
	for (int i = 0; i < QTree->Leaves.Num(); i++) {
		if(depthFilter != 0 && QTree->Leaves[i].Depth < depthFilter){ continue; }

		//Corners
		double posX = QTree->Leaves[i].Center.X + QTree->Leaves[i].Size.X * 0.5f;
		double negX = QTree->Leaves[i].Center.X - QTree->Leaves[i].Size.X * 0.5f;
		double posY = QTree->Leaves[i].Center.Y + QTree->Leaves[i].Size.Y * 0.5f;
		double negY = QTree->Leaves[i].Center.Y - QTree->Leaves[i].Size.Y * 0.5f;

		double lzeroX = FMeshData::LocalizePos(QTree->Leaves[i].Center.X,QTree->Size.X) + (border ? 1 : 0);
		double lzeroY = FMeshData::LocalizePos(QTree->Leaves[i].Center.Y,QTree->Size.Y) + (border ? 1 : 0);
		double lposX = (posX == posCorner.X ? QTree->Size.X : FMeshData::LocalizePos(QTree->Leaves[i].Center.X + QTree->Leaves[i].Size.X * 0.5f,QTree->Size.X)) + (border ? 1 : 0);
		double lnegX = (negX == QTree->Position.X ? 0 : FMeshData::LocalizePos(QTree->Leaves[i].Center.X - QTree->Leaves[i].Size.X * 0.5f,QTree->Size.X)) + (border ? 1 : 0);
		double lposY = (posY == posCorner.Y ? QTree->Size.Y : FMeshData::LocalizePos(QTree->Leaves[i].Center.Y + QTree->Leaves[i].Size.Y * 0.5f,QTree->Size.Y)) + (border ? 1 : 0);
		double lnegY = (negY == QTree->Position.Y ? 0 : FMeshData::LocalizePos(QTree->Leaves[i].Center.Y - QTree->Leaves[i].Size.Y * 0.5f,QTree->Size.Y)) + (border ? 1 : 0);

		double z = UTextureUtils::SampleTextureR16f(Texture,FIntVector2(lzeroX,lzeroY)/StepSize) * heightMultiplier;
		// UE_LOG(LogTemp,Warning,TEXT("QT MESH : pos:%s - val:%f"),*(FIntVector2(lzeroX,lzeroY)/StepSize).ToString(),z);
		MeshData.AddVertex(FVector(lzeroX,lzeroY,z));
		int zeroIndex = MeshData.Vertices.Num() - 1;
		
		double z1 = UTextureUtils::SampleTextureR16f(Texture,FIntVector2(lnegX,lposY)/StepSize) * heightMultiplier;
		MeshData.AddVertex(FVector(lnegX,lposY,z1));
		double z3 = UTextureUtils::SampleTextureR16f(Texture,FIntVector2(lposX,lposY)/StepSize) * heightMultiplier;
		MeshData.AddVertex(FVector(lposX,lposY,z3));
		double z5 = UTextureUtils::SampleTextureR16f(Texture,FIntVector2(lposX,lnegY)/StepSize) * heightMultiplier;
		MeshData.AddVertex(FVector(lposX,lnegY,z5));
		double z7 = UTextureUtils::SampleTextureR16f(Texture,FIntVector2(lnegX,lnegY)/StepSize) * heightMultiplier;
		MeshData.AddVertex(FVector(lnegX,lnegY,z7));
		int index = zeroIndex + 4;

		if(QTree->Leaves[i].Neighbors[2] || fmod(posY, QTree->Size.Y) == 0) //North
		{
			double z2 = UTextureUtils::SampleTextureR16f(Texture,FIntVector2(lzeroX,lposY)/StepSize) * heightMultiplier;
			MeshData.AddVertex(FVector(lzeroX,lposY,z2));
			index++;
			MeshData.AddTriangle(zeroIndex+1,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+2,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+1,zeroIndex+2,zeroIndex);
		}
		if(QTree->Leaves[i].Neighbors[0] || fmod(posX, QTree->Size.X) == 0) // East
		{
			double z4 = UTextureUtils::SampleTextureR16f(Texture,FIntVector2(lposX,lzeroY)/StepSize) * heightMultiplier;
			MeshData.AddVertex(FVector(lposX,lzeroY,z4));
			index++;
			MeshData.AddTriangle(zeroIndex+2,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+3,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+2,zeroIndex+3,zeroIndex);
		}
		if(QTree->Leaves[i].Neighbors[3] || fmod(negY, QTree->Size.Y) == 0) // South 
		{
			double z6 = UTextureUtils::SampleTextureR16f(Texture,FIntVector2(lzeroX,lnegY)/StepSize) * heightMultiplier;
			MeshData.AddVertex(FVector(lzeroX,lnegY,z6));
			index++;
			MeshData.AddTriangle(zeroIndex+3,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+4,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+3,zeroIndex+4,zeroIndex);
		}
		if(QTree->Leaves[i].Neighbors[1] || fmod(negX, QTree->Size.X) == 0) // West 
		{
			double z8 = UTextureUtils::SampleTextureR16f(Texture,FIntVector2(lnegX,lzeroY)/StepSize) * heightMultiplier;
			MeshData.AddVertex(FVector(lnegX,lzeroY,z8));
			index++;
			MeshData.AddTriangle(zeroIndex+4,index,zeroIndex);
			MeshData.AddTriangle(index,zeroIndex+1,zeroIndex);			
		}else {
			MeshData.AddTriangle(zeroIndex+4,zeroIndex+1,zeroIndex);
		}
	}
	
	if (border){
		// for (int i = 0; i < MeshData.Vertices.Num(); i++){
		// 	if (MeshData.Vertices[i].X == (QTree->Position.X+QTree->Size.X)/StepSize) { //PosX
		// 		MeshData.BorderVertices.Add(FVector(MeshData.Vertices[i].X+1,MeshData.Vertices[i].Y,UTextureUtils::SampleTextureR16f(Texture,FIntVector2(MeshData.Vertices[i].X+1,MeshData.Vertices[i].Y)) * heightMultiplier));
		// 	}
		// 	if (MeshData.Vertices[i].Y == (QTree->Position.Y+QTree->Size.Y)/StepSize) { //PosY
		// 		MeshData.BorderVertices.Add(FVector(MeshData.Vertices[i].X,MeshData.Vertices[i].Y+1,UTextureUtils::SampleTextureR16f(Texture,FIntVector2(MeshData.Vertices[i].X,MeshData.Vertices[i].Y+1)) * heightMultiplier));
		// 	}
		//
		// 	if (MeshData.Vertices[i].X == 1) { //NegX
		// 		MeshData.BorderVertices.Add(FVector(MeshData.Vertices[i].X-1,MeshData.Vertices[i].Y,UTextureUtils::SampleTextureR16f(Texture,FIntVector2(MeshData.Vertices[i].X-1,MeshData.Vertices[i].Y)) * heightMultiplier));
		// 	}
		// 	if (MeshData.Vertices[i].Y == 1) { //NegY
		// 		MeshData.BorderVertices.Add(FVector(MeshData.Vertices[i].X,MeshData.Vertices[i].Y-1,UTextureUtils::SampleTextureR16f(Texture,FIntVector2(MeshData.Vertices[i].X,MeshData.Vertices[i].Y-1)) * heightMultiplier));
		// 	}
		// }

		MeshData.GenerateSmoothedNormalsWithBorders(MeshData.Normals,MeshData.Tangents);
	}else{
		MeshData.CalculateTangents();
	}

	double end = FPlatformTime::Seconds();
	UE_LOG(MeshGenerator, Log, TEXT("MeshGenerator::QuadTreeMesh() ==> Verts: %d, Tris: %d, BorderVerts: %i  Runtime: %f"), MeshData.Vertices.Num(), MeshData.Triangles.Num(),MeshData.BorderVertices.Num(), end - start);
	return MeshData;
}

// FMeshData UMeshDataGenerator::QuadTreeMesh(UTextureRenderTarget2D* RenderTarget, UQuadTree* QTree, int StepSize, float UVScale, int depthFilter, int heightMultiplier)
// {
// 	FMeshData MeshData = FMeshData();
// 	MeshData.UVScale = UVScale;
// 	double start = FPlatformTime::Seconds();
//
// 	//Alignment Error
// 	if (QTree->Settings.MinSize / StepSize < 2) {
// 		UE_LOG(MeshGenerator,Error,TEXT("MeshGenerator::OuadTreeMesh ==> The QuadTree.Settings.MinSize[%d] must be at minimum twice the FNoiseMap2D.StepSize[%d] to guarantee sample availability."),QTree->Settings.MinSize,StepSize);
// 		return MeshData;	
// 	}
//
// 	FVector posCorner = QTree->Position + QTree->Size;
// 	for (int i = 0; i < QTree->Leaves.Num(); i++) {
// 		if(depthFilter != 0 && QTree->Leaves[i].Depth < depthFilter){ continue; }
//
// 		//Corners
// 		double posX = QTree->Leaves[i].Center.X + QTree->Leaves[i].Size.X * 0.5f;
// 		double negX = QTree->Leaves[i].Center.X - QTree->Leaves[i].Size.X * 0.5f;
// 		double posY = QTree->Leaves[i].Center.Y + QTree->Leaves[i].Size.Y * 0.5f;
// 		double negY = QTree->Leaves[i].Center.Y - QTree->Leaves[i].Size.Y * 0.5f;
//
// 		double lzeroX = FMeshData::LocalizePos(QTree->Leaves[i].Center.X,QTree->Size.X);
// 		double lzeroY = FMeshData::LocalizePos(QTree->Leaves[i].Center.Y,QTree->Size.Y);
// 		double lposX = (posX == posCorner.X ? QTree->Size.X : FMeshData::LocalizePos(QTree->Leaves[i].Center.X + QTree->Leaves[i].Size.X * 0.5f,QTree->Size.X));
// 		double lnegX = (negX == QTree->Position.X ? 0 : FMeshData::LocalizePos(QTree->Leaves[i].Center.X - QTree->Leaves[i].Size.X * 0.5f,QTree->Size.X));
// 		double lposY = (posY == posCorner.Y ? QTree->Size.Y : FMeshData::LocalizePos(QTree->Leaves[i].Center.Y + QTree->Leaves[i].Size.Y * 0.5f,QTree->Size.Y));
// 		double lnegY = (negY == QTree->Position.Y ? 0 : FMeshData::LocalizePos(QTree->Leaves[i].Center.Y - QTree->Leaves[i].Size.Y * 0.5f,QTree->Size.Y));
//
// 		double z = SampleRenderTargetR16f(RenderTarget,FIntVector2(lzeroX,lzeroY)/StepSize) * heightMultiplier;
// 		// UE_LOG(LogTemp,Warning,TEXT("QT MESH : pos:%s - val:%f"),*(FIntVector2(lzeroX,lzeroY)/StepSize).ToString(),z);
// 		MeshData.AddVertex(FVector(lzeroX,lzeroY,z));
// 		int zeroIndex = MeshData.Vertices.Num() - 1;
// 		
// 		double z1 = SampleRenderTargetR16f(RenderTarget,FIntVector2(lnegX,lposY)/StepSize) * heightMultiplier;
// 		MeshData.AddVertex(FVector(lnegX,lposY,z1));
// 		double z3 = SampleRenderTargetR16f(RenderTarget,FIntVector2(lposX,lposY)/StepSize) * heightMultiplier;
// 		MeshData.AddVertex(FVector(lposX,lposY,z3));
// 		double z5 = SampleRenderTargetR16f(RenderTarget,FIntVector2(lposX,lnegY)/StepSize) * heightMultiplier;
// 		MeshData.AddVertex(FVector(lposX,lnegY,z5));
// 		double z7 = SampleRenderTargetR16f(RenderTarget,FIntVector2(lnegX,lnegY)/StepSize) * heightMultiplier;
// 		MeshData.AddVertex(FVector(lnegX,lnegY,z7));
// 		int index = zeroIndex + 4;
//
// 		if(QTree->Leaves[i].Neighbors[2] || fmod(posY, QTree->Size.Y) == 0) //North
// 		{
// 			double z2 = SampleRenderTargetR16f(RenderTarget,FIntVector2(lzeroX,lposY)/StepSize) * heightMultiplier;
// 			MeshData.AddVertex(FVector(lzeroX,lposY,z2));
// 			index++;
// 			MeshData.AddTriangle(zeroIndex+1,index,zeroIndex);
// 			MeshData.AddTriangle(index,zeroIndex+2,zeroIndex);			
// 		}else {
// 			MeshData.AddTriangle(zeroIndex+1,zeroIndex+2,zeroIndex);
// 		}
// 		if(QTree->Leaves[i].Neighbors[0] || fmod(posX, QTree->Size.X) == 0) // East
// 		{
// 			double z4 = SampleRenderTargetR16f(RenderTarget,FIntVector2(lposX,lzeroY)/StepSize) * heightMultiplier;
// 			MeshData.AddVertex(FVector(lposX,lzeroY,z4));
// 			index++;
// 			MeshData.AddTriangle(zeroIndex+2,index,zeroIndex);
// 			MeshData.AddTriangle(index,zeroIndex+3,zeroIndex);			
// 		}else {
// 			MeshData.AddTriangle(zeroIndex+2,zeroIndex+3,zeroIndex);
// 		}
// 		if(QTree->Leaves[i].Neighbors[3] || fmod(negY, QTree->Size.Y) == 0) // South 
// 		{
// 			double z6 = SampleRenderTargetR16f(RenderTarget,FIntVector2(lzeroX,lnegY)/StepSize) * heightMultiplier;
// 			MeshData.AddVertex(FVector(lzeroX,lnegY,z6));
// 			index++;
// 			MeshData.AddTriangle(zeroIndex+3,index,zeroIndex);
// 			MeshData.AddTriangle(index,zeroIndex+4,zeroIndex);			
// 		}else {
// 			MeshData.AddTriangle(zeroIndex+3,zeroIndex+4,zeroIndex);
// 		}
// 		if(QTree->Leaves[i].Neighbors[1] || fmod(negX, QTree->Size.X) == 0) // West 
// 		{
// 			double z8 = SampleRenderTargetR16f(RenderTarget,FIntVector2(lnegX,lzeroY)/StepSize) * heightMultiplier;
// 			MeshData.AddVertex(FVector(lnegX,lzeroY,z8));
// 			index++;
// 			MeshData.AddTriangle(zeroIndex+4,index,zeroIndex);
// 			MeshData.AddTriangle(index,zeroIndex+1,zeroIndex);			
// 		}else {
// 			MeshData.AddTriangle(zeroIndex+4,zeroIndex+1,zeroIndex);
// 		}
// 	}
//
// 	MeshData.CalculateTangents();
//
// 	double end = FPlatformTime::Seconds();
// 	UE_LOG(MeshGenerator, Log, TEXT("MeshGenerator::QuadTreeMesh() ==> Verts: %d, Tris: %d  Runtime: %f"), MeshData.Vertices.Num(), MeshData.Triangles.Num(), end - start);
// 	return MeshData;
// }


/**
 *
 *	MarchingCubes
 *
 **/

FMeshData UMeshDataGenerator::MarchingCubes(FVector position, FVector size, int stepSize, FNoiseSettings* NoiseSettings, float UVScale, FMarchingCubesSettings MarchingCubesSettings)
{
    FMeshData MeshData = FMeshData();
    MeshData.UVScale = UVScale;
    double start = FPlatformTime::Seconds();

	//Alignment error
	if (FMath::Modulo(size.X,stepSize) != 0 || FMath::Modulo(size.Y, stepSize) != 0 || FMath::Modulo(size.Z, stepSize) != 0){
		UE_LOG(MeshGenerator,Error,TEXT("MeshGenerator::MarchingCubes ==> The MapSize[(%f,%f,%f)] must be evenly divisible by the StepSize[%d]."),size.X,size.Y,size.Z,stepSize);
		return MeshData;
	}

    for (int x = 0; x < size.X; x += stepSize) {
        for (int y = 0; y < size.Y; y += stepSize) {
            for (int z = 0; z < size.Z; z += stepSize) {
                FVector pos = FVector(x, y, z);

                TArray<float> cubeValues;
                for (int i = 0; i < UMarchingCubes::cornerOffsets.Num(); i++) {
                	FIntVector mp =(FIntVector) (pos + UMarchingCubes::cornerOffsets[i]*stepSize);
                	FIntVector s = (FIntVector)position+(FIntVector)(pos + UMarchingCubes::cornerOffsets[i]*stepSize);
                    if (MarchingCubesSettings.renderSides && (mp.X >= size.X || mp.Y >= size.Y || mp.Z >= size.Z || mp.X <= 0 || mp.Y <= 0 || mp.Z <= 0)) {
                        cubeValues.Add(MarchingCubesSettings.iso);
                    } else {
						cubeValues.Add(UNoise::Evaluate3D((FVector)s,NoiseSettings));
                    }
                }

                int cubeIndex = 0;
                for (int i = 0; i < cubeValues.Num(); i++) {
                    if (cubeValues[i] < MarchingCubesSettings.iso) {
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

                    if (MarchingCubesSettings.interpolate) {
                        a = Interp(UMarchingCubes::cornerOffsets[e00] * stepSize, cubeValues[e00],
                                   UMarchingCubes::cornerOffsets[e01] * stepSize, cubeValues[e01], MarchingCubesSettings.iso) + pos;
                        b = Interp(UMarchingCubes::cornerOffsets[e10] * stepSize, cubeValues[e10],
                                   UMarchingCubes::cornerOffsets[e11] * stepSize, cubeValues[e11], MarchingCubesSettings.iso) + pos;
                        c = Interp(UMarchingCubes::cornerOffsets[e20] * stepSize, cubeValues[e20],
                                   UMarchingCubes::cornerOffsets[e21] * stepSize, cubeValues[e21], MarchingCubesSettings.iso) + pos;
                    } else {
                        a = Default(UMarchingCubes::cornerOffsets[e00] * stepSize, UMarchingCubes::cornerOffsets[e01] * stepSize) + pos;
                        b = Default(UMarchingCubes::cornerOffsets[e10] * stepSize, UMarchingCubes::cornerOffsets[e11] * stepSize) + pos;
                        c = Default(UMarchingCubes::cornerOffsets[e20] * stepSize, UMarchingCubes::cornerOffsets[e21] * stepSize) + pos;
                    }
                    MeshData.AddTriangle(a, b, c);
                }
            }
        }
    }

    MeshData.CalculateTangents();

    double end = FPlatformTime::Seconds();
    UE_LOG(MeshGenerator, Log, TEXT("MeshGenerator::MarchingCubes ==> Verts: %d, Tris: %d,  Runtime: %f"), MeshData.Vertices.Num(), MeshData.Triangles.Num(), end - start);

    return MeshData;
}

FMeshData UMeshDataGenerator::MarchingCubes(FNoiseMap3d* NoiseMap, FVector position, FVector size, int stepSize, float UVScale,FMarchingCubesSettings MarchingCubesSettings)
{
    FMeshData MeshData = FMeshData();
    MeshData.UVScale = UVScale;
    double start = FPlatformTime::Seconds();

	//Alignment error
	if (FMath::Modulo(size.X,stepSize) != 0 || FMath::Modulo(size.Y, stepSize) != 0 || FMath::Modulo(size.Z, stepSize) != 0){
		UE_LOG(MeshGenerator,Error,TEXT("MeshGenerator::MarchingCubes ==> The MapSize[(%f,%f,%f)] must be evenly divisible by the StepSize[%d]."),size.X,size.Y,size.Z,stepSize);
		return MeshData;
	}

    for (int x = 0; x < size.X; x += stepSize) {
        for (int y = 0; y < size.Y; y += stepSize) {
            for (int z = 0; z < size.Z; z += stepSize) {
                FVector pos = FVector(x, y, z);

                TArray<float> cubeValues;
                for (int i = 0; i < UMarchingCubes::cornerOffsets.Num(); i++) {
                	FIntVector mp =(FIntVector) (pos + UMarchingCubes::cornerOffsets[i]*stepSize);
                	FIntVector s = (FIntVector)position+(FIntVector)(pos + UMarchingCubes::cornerOffsets[i]*stepSize);
                    if (MarchingCubesSettings.renderSides && (mp.X >= size.X || mp.Y >= size.Y || mp.Z >= size.Z || mp.X <= 0 || mp.Y <= 0 || mp.Z <= 0)) {
                        cubeValues.Add(MarchingCubesSettings.iso);
                    } else {
                    	if (!NoiseMap->Map.Contains(s)) {
                    		UE_LOG(MeshGenerator,Error,TEXT("MeshGenerator::MarchingCubes => invalid map index %s"),*s.ToString());
                    	}else{
							cubeValues.Add(NoiseMap->Map[s]);
                    	}
                    }
                }

                int cubeIndex = 0;
                for (int i = 0; i < cubeValues.Num(); i++) {
                    if (cubeValues[i] < MarchingCubesSettings.iso) {
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

                    if (MarchingCubesSettings.interpolate) {
                        a = Interp(UMarchingCubes::cornerOffsets[e00] * stepSize, cubeValues[e00],
                                   UMarchingCubes::cornerOffsets[e01] * stepSize, cubeValues[e01], MarchingCubesSettings.iso) + pos;
                        b = Interp(UMarchingCubes::cornerOffsets[e10] * stepSize, cubeValues[e10],
                                   UMarchingCubes::cornerOffsets[e11] * stepSize, cubeValues[e11], MarchingCubesSettings.iso) + pos;
                        c = Interp(UMarchingCubes::cornerOffsets[e20] * stepSize, cubeValues[e20],
                                   UMarchingCubes::cornerOffsets[e21] * stepSize, cubeValues[e21], MarchingCubesSettings.iso) + pos;
                    } else {
                        a = Default(UMarchingCubes::cornerOffsets[e00] * stepSize, UMarchingCubes::cornerOffsets[e01] * stepSize) + pos;
                        b = Default(UMarchingCubes::cornerOffsets[e10] * stepSize, UMarchingCubes::cornerOffsets[e11] * stepSize) + pos;
                        c = Default(UMarchingCubes::cornerOffsets[e20] * stepSize, UMarchingCubes::cornerOffsets[e21] * stepSize) + pos;
                    }
                    MeshData.AddTriangle(a, b, c);
                }
            }
        }
    }

    MeshData.CalculateTangents();

    double end = FPlatformTime::Seconds();
    UE_LOG(MeshGenerator, Log, TEXT("MeshGenerator::MarchingCubes ==> Verts: %d, Tris: %d,  Runtime: %f"), MeshData.Vertices.Num(), MeshData.Triangles.Num(), end - start);

    return MeshData;
}

FVector UMeshDataGenerator::Interp(FVector edgeVertex1, float valueAtVertex1, FVector edgeVertex2, float valueAtVertex2, float isoLevel) {
	return (edgeVertex1 + (isoLevel - valueAtVertex1) * (edgeVertex2 - edgeVertex1) / (valueAtVertex2 - valueAtVertex1));
}

FVector UMeshDataGenerator::Default(FVector edgeVertex1, FVector edgeVertex2) {
	return (edgeVertex1 + edgeVertex2) / 2;
}
