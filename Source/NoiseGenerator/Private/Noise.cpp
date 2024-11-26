#include "Noise.h"
#include "FastNoiseLite.h"
#include "NoiseSettings.h"
#include "Poisson.h"
#include "Curves/CurveLinearColor.h"

FastNoiseLite UNoise::SetupFastNoise(FNoiseSettings* settings)
{
	FastNoiseLite noise;

	FastNoiseLite::NoiseType NoiseType;
	if(settings->type == Perlin) { NoiseType = FastNoiseLite::NoiseType_Perlin;	}
	else { NoiseType = FastNoiseLite::NoiseType_OpenSimplex2; }

	noise.SetNoiseType(NoiseType);
	noise.SetFractalType(FastNoiseLite::FractalType_FBm);
	noise.SetSeed(settings->seed);
	noise.SetFrequency(settings->frequency);
	noise.SetFractalOctaves(settings->octaves);
	noise.SetFractalLacunarity(settings->lacunarity);
	noise.SetFractalGain(settings->persistence);

	noise.SetDomainWarpType(FastNoiseLite::DomainWarpType_OpenSimplex2);
	noise.SetDomainWarpAmp(settings->domainWarpingScale);

	return noise;
}

//1D Noise

float UNoise::Evaluate1D(float x, FNoiseSettings* NoiseSettings)
{
	FastNoiseLite noise = SetupFastNoise(NoiseSettings);
	float sample = (x + NoiseSettings->offset.X) * (1.0f / NoiseSettings->scale);
	return EvaluateFilter(noise.GetNoise(sample,NoiseSettings->seed),NoiseSettings->filter);	
}

//2D Noise

float UNoise::Evaluate2D(FVector pos, FNoiseSettings* NoiseSettings)
{
	FastNoiseLite noise = SetupFastNoise(NoiseSettings);
	FVector2D sample = (FVector2D(pos.X,pos.Y) + FVector2D(NoiseSettings->offset.X,NoiseSettings->offset.Y)) * (1.0f / NoiseSettings->scale);
	if(NoiseSettings->domainWarping){ noise.DomainWarp(sample.X,sample.Y); }
	return EvaluateFilter(noise.GetNoise(sample.X,sample.Y),NoiseSettings->filter);
}

float UNoise::EvaluateHeight(FVector pos, FNoiseSettings* NoiseSettings, int heightMultiplier)
{
	return Evaluate2D(pos,NoiseSettings) * heightMultiplier;
}

//does this work?
float UNoise::EvaluateHeightFalloff(FVector pos, FNoiseSettings* NoiseSettings, int heightMultiplier,int worldSize)
{
	float falloff = FMath::Max(FMath::Abs(pos.X/worldSize*2-1),FMath::Abs(pos.Y/worldSize*2-1));
	float smoothFalloff = FMath::Pow(falloff,3) / (FMath::Pow(falloff,3) + FMath::Pow(2.2f - 2.2f*falloff,3));
	
	return FMath::Clamp(Evaluate2D(pos,NoiseSettings) - smoothFalloff,-1,1) * heightMultiplier;
}

float UNoise::EvaluateSlope(FVector pos,FNoiseSettings* NoiseSettings, int heightMultiplier)
{
	float height = pos.Z; 
	float heightX =  EvaluateHeight(pos+FVector(100,0,0),NoiseSettings,heightMultiplier);
	float heightY =  EvaluateHeight(pos+FVector(0,100,0),NoiseSettings,heightMultiplier);

	float slopeX = FMath::Abs(heightX - height) / 100;
	float slopeY = FMath::Abs(heightY - height) / 100;

	return slopeX > slopeY ? slopeX : slopeY;
}

//3D Noise

float UNoise::Evaluate3D(FVector pos, FNoiseSettings* NoiseSettings)
{
	FastNoiseLite noise = SetupFastNoise(NoiseSettings);
	FVector sample = (pos + NoiseSettings->offset) * (1.0f / NoiseSettings->scale);
	if(NoiseSettings->domainWarping){ noise.DomainWarp(sample.X,sample.Y,sample.Z); }
	return EvaluateFilter(noise.GetNoise(sample.X,sample.Y,sample.Z),NoiseSettings->filter);
}

float UNoise::EvaluateDensity(FVector pos, FNoiseSettings* NoiseSettings, float densityMultiplier)
{
	return Evaluate3D(pos,NoiseSettings) * densityMultiplier;
}

//Filters

float UNoise::EvaluateFilter(float noise, NoiseFilter filter)
{
	if(filter == Rigid){ return FilterRigid(noise); }
	return noise;		
}

float UNoise::FilterRigid(float noise)
{
	float v = 1 - FMath::Abs(noise);
	return v * v * 2 - 1;
}

float UNoise::Normalize(float noise, float min = .0f, float max = 1.0f)
{
	return (max - min) * ((noise + 1.0f) / 2.0f) + min;
}

//Maps

TMap<FVector2d, float> UNoise::GenerateMap2D(FVector pos, FVector2d mapSize, FNoiseSettings* NoiseSettings)
{
	TMap<FVector2d, float> map;
	for(int x = pos.X; x < pos.X+mapSize.X; x++) {
		for(int y = pos.Y; y < pos.Y+mapSize.Y; y++) {
			FVector2d index = FVector2d(x, y);
			map.Add(index,Evaluate2D(FVector(x,y,0),NoiseSettings));
		}
	}
	return map;
}

TMap<FVector2d, float> UNoise::GenerateMap2D(FVector pos, FVector2d mapSize, TArray<FLayeredNoiseSettings>* NoiseSettings)
{
	TMap<FVector2d, float> map = TMap<FVector2d, float>();
	TArray<FNoiseLayerData> layerData = TArray<FNoiseLayerData>();

	float gain = 0;
	for (int i = 0; i < NoiseSettings->Num(); i++){
		if (NoiseSettings->operator[](i).Gain <= 0){ continue; }
		FNoiseLayerData data = FNoiseLayerData();
		data.map = GenerateMap2D(pos,mapSize,&NoiseSettings->operator[](i).LayerSettings);
		data.gain = NoiseSettings->operator[](i).Gain;
		data.curve = NoiseSettings->operator[](i).LayerCurve;
		layerData.Add(data);
		gain += data.gain;
	}	
	for(int x = pos.X; x < pos.X+mapSize.X; x++) {
		for(int y = pos.Y; y < pos.Y+mapSize.Y; y++) {
			FVector2d index = FVector2d(x, y);
			float sample = 0;
			for (int i = 0; i < layerData.Num(); i++){
				if (layerData[i].gain <= 0){ continue; }
				if (layerData[i].curve == nullptr)	{
					sample += layerData[i].map[index] * layerData[i].gain;
				}else{
					sample += layerData[i].curve->GetFloatValue(layerData[i].map[index]) * layerData[i].gain;
				}	
			}	
			sample /= gain;
			map.Add(index,sample);
		}
	}

	return map;
}

UTexture2D* UNoise::GenerateTexture(FVector pos, FVector2d mapSize, FNoiseSettings* NoiseSettings, UCurveLinearColor* ColorCurve)
{
	TMap<FVector2d, float> map = GenerateMap2D(pos,mapSize,NoiseSettings);
	UTexture2D* texture = UTexture2D::CreateTransient(mapSize.X,mapSize.Y);

	TArray<FColor> colors;

	for(int x = pos.X; x < pos.X+mapSize.X; x++) {
		for(int y = pos.Y; y < pos.Y+mapSize.Y; y++) {
			FVector2d index = FVector2d(x, y);
			float normalized = Normalize(map[index],0,1);
			if (ColorCurve == nullptr) {
				uint8 color = static_cast<uint8>(normalized * 255);
				colors.Add(FColor(color,color,color,255));	
			} else {
				FLinearColor color = ColorCurve->GetLinearColorValue(normalized);
				colors.Add(color.ToFColor(true));
			}
		}
	}
	void* TextureData = texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, colors.GetData(), colors.Num() * sizeof(FColor));
	texture->GetPlatformData()->Mips[0].BulkData.Unlock();

	texture->UpdateResource();
	
	return texture;
}

UTexture2D* UNoise::GenerateTexture(FVector pos, FVector2d mapSize,TArray<FLayeredNoiseSettings>* NoiseSettings, UCurveLinearColor* ColorCurve)
{
	UTexture2D* texture = UTexture2D::CreateTransient(mapSize.X,mapSize.Y);
	TArray<FColor> colors;
	TArray<FNoiseLayerData> layerData = TArray<FNoiseLayerData>();

	float gain = 0;
	for (int i = 0; i < NoiseSettings->Num(); i++){
		if (NoiseSettings->operator[](i).Gain <= 0){ continue; }
		FNoiseLayerData data = FNoiseLayerData();
		data.map = GenerateMap2D(pos,mapSize,&NoiseSettings->operator[](i).LayerSettings);
		data.gain = NoiseSettings->operator[](i).Gain;
		data.curve = NoiseSettings->operator[](i).LayerCurve;
		layerData.Add(data);
		gain += data.gain;
	}	
	for(int x = pos.X; x < pos.X+mapSize.X; x++) {
		for(int y = pos.Y; y < pos.Y+mapSize.Y; y++) {
			FVector2d index = FVector2d(x, y);
			float sample = 0;
			for (int i = 0; i < layerData.Num(); i++){
				if (layerData[i].curve == nullptr)	{
					sample += layerData[i].map[index] * layerData[i].gain;
				}else{
					sample += layerData[i].curve->GetFloatValue(layerData[i].map[index]) * layerData[i].gain;
				}	
			}	
			sample /= gain;
			float normalized = Normalize(sample,0,1);
			if (ColorCurve == nullptr) {
				uint8 color = static_cast<uint8>(normalized * 255);
				colors.Add(FColor(color,color,color,255));	
			} else {
				FLinearColor color = ColorCurve->GetLinearColorValue(normalized);
				colors.Add(color.ToFColor(true));
			}
		}
	}
	void* TextureData = texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, colors.GetData(), colors.Num() * sizeof(FColor));
	texture->GetPlatformData()->Mips[0].BulkData.Unlock();

	texture->UpdateResource();
	
	return texture;
}

TMap<FVector, float> UNoise::GenerateMap3D(FVector pos, FVector mapSize, FNoiseSettings* NoiseSettings)
{
	TMap<FVector, float> map;
	for(int x = pos.X; x < pos.X+mapSize.X; x++) {
		for(int y = pos.Y; y < pos.Y+mapSize.Y; y++) {
			for(int z = pos.Z; z < pos.Z+mapSize.Z; z++) {
				FVector index = FVector(x, y, z);
				map.Add(index,Evaluate3D(FVector(x,y,z),NoiseSettings));
			}
		}
	}
	return map;
}

//Poisson

FVector2D UNoise::PoissonSample(const FVector2D& center, float minRadius, float maxRadius)
{
	return Poisson::GenerateRandomPointInAnnulus(center,minRadius,maxRadius);
}

TArray<FVector2D> UNoise::PoissonDiscSample(const FVector2D& topLeft, const FVector2D& bottomRight, float minDist, int newPointsCount)
{
	return Poisson::GeneratePoissonDiscSamples(topLeft,bottomRight,minDist,newPointsCount);
}
