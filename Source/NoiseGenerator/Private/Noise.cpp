#include "Noise.h"
#include "FastNoiseLite.h"
#include "NoiseSettings.h"
#include "Poisson.h"
#include "Curves/CurveLinearColor.h"

FastNoiseLite UNoise::SetupFastNoise(FNoiseSettings* settings)
{
	FastNoiseLite noise;

	FastNoiseLite::NoiseType NoiseType;
	switch (settings->type)	{
		default:
		case Perlin:
			NoiseType = FastNoiseLite::NoiseType_Perlin;
			break;
		case Simplex:
			NoiseType = FastNoiseLite::NoiseType_OpenSimplex2;
			break;
	}

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

float UNoise::EvaluateSlope(FVector pos,FNoiseSettings* NoiseSettings, int heightMultiplier)
{
	float height = pos.Z; 
	float heightX =  EvaluateHeight(pos+FVector(100,0,0),NoiseSettings,heightMultiplier);
	float heightY =  EvaluateHeight(pos+FVector(0,100,0),NoiseSettings,heightMultiplier);

	float slopeX = FMath::Abs(heightX - height) / 100;
	float slopeY = FMath::Abs(heightY - height) / 100;

	return slopeX > slopeY ? slopeX : slopeY;
}

//does this work?
float UNoise::EvaluateHeightFalloff(FVector pos, FNoiseSettings* NoiseSettings, int heightMultiplier,int worldSize)
{
	float falloff = FMath::Max(FMath::Abs(pos.X/worldSize*2-1),FMath::Abs(pos.Y/worldSize*2-1));
	float smoothFalloff = FMath::Pow(falloff,3) / (FMath::Pow(falloff,3) + FMath::Pow(2.2f - 2.2f*falloff,3));
	
	return FMath::Clamp(Evaluate2D(pos,NoiseSettings) - smoothFalloff,-1,1) * heightMultiplier;
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
	//return (noise - min) / (max - min);
}

void UNoise::Normalize(FNoiseMap2d& NoiseMap, NoiseNormalizeMode normalizeMode, NoiseType noiseType)
{
	float min;
	float max;
	switch(noiseType){
		default:
		case Perlin:
			min = -0.45f; max = 0.45f;
		break;
		case Simplex:
			min = -0.8f; max = 0.8f;
		break;
	}

	if (normalizeMode != NoNormalization){
		for(int x = 0; x < NoiseMap.Size.X; x++) {
			for(int y = 0; y < NoiseMap.Size.Y; y++) {
				FIntVector2 index = FIntVector2(FMath::Floor(NoiseMap.Position.X) + x, FMath::Floor(NoiseMap.Position.Y) + y);
				switch (normalizeMode){
				case Local:
					NoiseMap.Map[index] = (1-(-1))/(NoiseMap.MinMax.max-NoiseMap.MinMax.min)*(NoiseMap.Map[index]-NoiseMap.MinMax.min)+(-1);
					break;
				case LocalPositive:
					NoiseMap.Map[index] = (NoiseMap.Map[index]-NoiseMap.MinMax.min)/(NoiseMap.MinMax.max-NoiseMap.MinMax.min);
					break;
				case Global:
					NoiseMap.Map[index] = (1-(-1))/(max-min)*(NoiseMap.Map[index]-min)+(-1);
					break;
				case GlobalPositive:
					NoiseMap.Map[index] = (NoiseMap.Map[index] - (min)) / (max - (min));
					break;
				default:
					break;
				}
			}
		}
	}
	// return NoiseMap;
}



//Maps
FNoiseMap2d UNoise::GenerateMap2D(FIntVector pos, FIntVector2 mapSize, FNoiseSettings* NoiseSettings)
{
	FNoiseMap2d NoiseMap = FNoiseMap2d(pos,mapSize);
	for(int x = 0; x < mapSize.X; x++) {
		for(int y = 0; y < mapSize.Y; y++) {
			FIntVector2 index = FIntVector2(FMath::Floor(pos.X) + x, FMath::Floor(pos.Y) + y);
			NoiseMap.Map.Add(index,Evaluate2D(FVector(index.X,index.Y,0),NoiseSettings));
			NoiseMap.MinMax.Add(NoiseMap.Map[index]);
		}
	}	

	Normalize(NoiseMap,NoiseSettings->normalizeMode,NoiseSettings->type);
	
	return NoiseMap;
}

FNoiseMap2d UNoise::GenerateMap2D(FIntVector pos, FIntVector2 mapSize, TArray<FLayeredNoiseSettings>* NoiseSettings)
{
	FNoiseMap2d NoiseMap = FNoiseMap2d(pos,mapSize);
	TArray<FNoiseLayerData> layerData = TArray<FNoiseLayerData>();

	for (int i = 0; i < NoiseSettings->Num(); i++){
		if (NoiseSettings->operator[](i).Gain <= 0){ continue; }
		FNoiseLayerData data = FNoiseLayerData();
		data.noiseMap = GenerateMap2D(pos,mapSize,&NoiseSettings->operator[](i).LayerSettings);
		data.gain = NoiseSettings->operator[](i).Gain;
		data.curve = NoiseSettings->operator[](i).LayerCurve;
		layerData.Add(data);
	}

	GenerateMap2D(NoiseMap,&layerData);
	
	return NoiseMap;
}

//Create FNoiseMap2D from a TArray of FNoiseLayerData to create Layered Noise
void UNoise::GenerateMap2D(FNoiseMap2d& NoiseMap, TArray<FNoiseLayerData>* layerData)
{
	float gain = 0.0f;
	for (int i = 0; i < layerData->Num(); i++) { gain += layerData->operator[](i).gain; }
	for(int x = NoiseMap.Position.X; x < NoiseMap.Position.X+NoiseMap.Size.X; x++) {
		for(int y = NoiseMap.Position.Y; y < NoiseMap.Position.Y+NoiseMap.Size.Y; y++) {
			FIntVector2 index = FIntVector2(x, y);
			float sample = 0;
			for (int i = 0; i < layerData->Num(); i++){
				if (layerData->operator[](i).gain <= 0){ continue; }
				if (layerData->operator[](i).curve == nullptr)	{
					sample += layerData->operator[](i).noiseMap.Map[index] * layerData->operator[](i).gain;
				}else{
					sample += layerData->operator[](i).curve->GetFloatValue(layerData->operator[](i).noiseMap.Map[index]) * layerData->operator[](i).gain;
				}	
			}	
			sample /= gain;
			NoiseMap.Map.Add(index,sample);
			NoiseMap.MinMax.Add(NoiseMap.Map[index]);
		}
	}	
}

UTexture2D* UNoise::GenerateTexture(FIntVector pos, FIntVector2 mapSize, FNoiseSettings* NoiseSettings, UCurveLinearColor* ColorCurve)
{
	FNoiseMap2d NoiseMap = GenerateMap2D(pos,mapSize,NoiseSettings);
	return GenerateTexture(&NoiseMap,ColorCurve);
}

UTexture2D* UNoise::GenerateTexture(FIntVector pos, FIntVector2 mapSize,TArray<FLayeredNoiseSettings>* NoiseSettings, UCurveLinearColor* ColorCurve)
{
	FNoiseMap2d NoiseMap = GenerateMap2D(pos,mapSize,NoiseSettings);
	return GenerateTexture(&NoiseMap,ColorCurve);
}

UTexture2D* UNoise::GenerateTexture(FNoiseMap2d* NoiseMap, UCurveLinearColor* ColorCurve)
{
	UTexture2D* texture = UTexture2D::CreateTransient(NoiseMap->Size.X,NoiseMap->Size.Y);
	TArray<FColor> colors;

	for(int x = 0; x < NoiseMap->Size.X; x++) {
		for(int y = 0; y < NoiseMap->Size.Y; y++) {
			FIntVector2 index = FIntVector2(NoiseMap->Position.X + x, NoiseMap->Position.Y + y);
			if (!NoiseMap->Map.Contains(index)){
				UE_LOG(LogTemp, Error, TEXT("Noise map does not contain this element %s"),*index.ToString());
				continue;
			}
			if (ColorCurve == nullptr) {
				uint8 color = static_cast<uint8>(NoiseMap->Map[index] * 255);
				colors.Add(FColor(color,color,color,255));	
			} else {
				FLinearColor color = ColorCurve->GetLinearColorValue(NoiseMap->Map[index]);
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

TMap<FIntVector, float> UNoise::GenerateMap3D(FIntVector pos, FIntVector mapSize, FNoiseSettings* NoiseSettings)
{
	TMap<FIntVector, float> map;
	FMinMax MinMax;
	for(int x = pos.X; x < pos.X+mapSize.X; x++) {
		for(int y = pos.Y; y < pos.Y+mapSize.Y; y++) {
			for(int z = pos.Z; z < pos.Z+mapSize.Z; z++) {
				FIntVector index = FIntVector(x, y, z);
				map.Add(index,Evaluate3D(FVector(x,y,z),NoiseSettings));
				MinMax.Add(map[index]);
			}
		}
	}
	if (NoiseSettings->normalizeMode == NoiseNormalizeMode::Local){
		for(int x = pos.X; x < pos.X+mapSize.X; x++) {
			for(int y = pos.Y; y < pos.Y+mapSize.Y; y++) {
				for(int z = pos.Z; z < pos.Z+mapSize.Z; z++) {
					FIntVector index = FIntVector(x, y, z);
					map[index] = Normalize(map[index],MinMax.min,MinMax.max);
				}
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
