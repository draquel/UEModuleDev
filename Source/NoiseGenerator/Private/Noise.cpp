#include "Noise.h"
#include "FastNoiseLite.h"
#include "NoiseGenerator.h"
#include "NoiseSettings.h"
#include "Poisson.h"
#include "Curves/CurveLinearColor.h"
#include "DSP/Chorus.h"
#include "NoiseGenerator/Public/NoiseComputeShader/NoiseComputeShader.h"

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

FMinMax UNoise::GetTypeMinMax(NoiseType type)
{
	FMinMax minmax = FMinMax();
	switch(type){
		default:
		case Perlin:
			minmax.min = -0.45f; minmax.max = 0.45f;
			break;
		case Simplex:
			minmax.min = -0.8f; minmax.max = 0.8f;
			break;
	}
	return minmax;
}

float UNoise::Normalize(float noise, float min = .0f, float max = 1.0f)
{
	return (max - min) * ((noise + 1.0f) / 2.0f) + min;
}

float UNoise::Normalize(NoiseNormalizeMode normalizeMode, float noise, FMinMax localMinMax, FMinMax typeMinMax)
{
	switch (normalizeMode){
		case Local:
			noise = (1-(-1))/(localMinMax.max-localMinMax.min)*(noise-localMinMax.min)+(-1);
			break;
		case LocalPositive:
			noise = (noise-localMinMax.min)/(localMinMax.max-localMinMax.min);
			break;
		case Global:
			noise = (1-(-1))/(typeMinMax.max-typeMinMax.min)*(noise-typeMinMax.min)+(-1);
			break;
		case GlobalPositive:
			noise = (noise - (typeMinMax.min)) / (typeMinMax.max - (typeMinMax.min));
			break;
		default:
			break;
	}
	
	return noise;
}

void UNoise::Normalize(FNoiseMap2d* NoiseMap, NoiseNormalizeMode normalizeMode, NoiseType noiseType)
{
	FMinMax typeMinMax = GetTypeMinMax(noiseType);

	if (normalizeMode != NoNormalization){
		for(int x = 0; x < NoiseMap->Size.X; x++) {
			for(int y = 0; y < NoiseMap->Size.Y; y++) {
				FIntVector2 index =	FIntVector2(NoiseMap->Position.X + x, NoiseMap->Position.Y + y);
				NoiseMap->Map[index] = Normalize(normalizeMode,NoiseMap->Map[index],NoiseMap->MinMax,typeMinMax);
			}
		}
	}
}

void UNoise::Normalize(FNoiseMap3d* NoiseMap, NoiseNormalizeMode normalizeMode, NoiseType noiseType)
{
	FMinMax typeMinMax = GetTypeMinMax(noiseType);	
	
	if (normalizeMode != NoNormalization){
		for(int x = 0; x < NoiseMap->Size.X; x++) {
			for(int y = 0; y < NoiseMap->Size.Y; y++) {
				for(int z = 0; z < NoiseMap->Size.Z; z++) {
					FIntVector index = FIntVector(NoiseMap->Position.X + x, NoiseMap->Position.Y + y, NoiseMap->Position.Z + z);
					NoiseMap->Map[index] = Normalize(normalizeMode,NoiseMap->Map[index],NoiseMap->MinMax,typeMinMax);
				}
			}
		}
	}
}

//Maps
FNoiseMap2d UNoise::GenerateMap2D(FIntVector pos, FIntVector2 mapSize, FNoiseSettings* NoiseSettings)
{
	FNoiseMap2d NoiseMap = FNoiseMap2d(pos,mapSize);
	for(int x = 0; x < mapSize.X; x++) {
		for(int y = 0; y < mapSize.Y; y++) {
			FIntVector2 index = FIntVector2(pos.X + x, pos.Y + y);
			NoiseMap.Map.Add(index,Evaluate2D(FVector(index.X,index.Y,0),NoiseSettings));
			NoiseMap.MinMax.Add(NoiseMap.Map[index]);
		}
	}	

	Normalize(&NoiseMap,NoiseSettings->normalizeMode,NoiseSettings->type);
	
	return NoiseMap;
}

FNoiseMap2d UNoise::GenerateMap2D(FIntVector pos, FIntVector2 mapSize, TArray<FNoiseSettings>* NoiseSettings)
{
	FNoiseMap2d NoiseMap = FNoiseMap2d(pos,mapSize);
	TArray<FNoiseLayerData> layerData = TArray<FNoiseLayerData>();

	for (int i = 0; i < NoiseSettings->Num(); i++){
		if (NoiseSettings->operator[](i).gain <= 0){ continue; }
		FNoiseLayerData data = FNoiseLayerData();
		data.noiseMap = GenerateMap2D(pos,mapSize,&NoiseSettings->operator[](i));
		data.gain = NoiseSettings->operator[](i).gain;
		data.curve = NoiseSettings->operator[](i).curve;
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


void UNoise::GenerateMap2D(FIntVector pos, FIntVector2 mapSize, FNoiseSettings NoiseSettings, TFunction<void(FNoiseMap2d NoiseMap)> Callback)
{
	int cycles = mapSize.X * mapSize.Y * NoiseSettings.octaves;
	double start = FPlatformTime::Seconds();

	if(NoiseSettings.source == CPU) {
		FNoiseMap2d NoiseMap = GenerateMap2D(pos,mapSize,&NoiseSettings);
		double end = FPlatformTime::Seconds();
		UE_LOG(NoiseGenerator,Log,TEXT("UNoise::GenerateMap2D() ==> %s-%s, Cycles: %d, RunTime: %fs"),*UEnum::GetValueAsString(NoiseSettings.source),*UEnum::GetValueAsString(NoiseSettings.type),cycles,end-start);
		Callback(NoiseMap);
		return;
	}

	//NoiseSettings.source == GPU
	FNoiseComputeShaderDispatchParams Params = FNoiseComputeShaderInterface::BuildParams((FVector3f)pos, FVector3f(mapSize.X,mapSize.Y,1.0f), NoiseSettings);
	FNoiseComputeShaderInterface::Dispatch(Params,[pos,mapSize,NoiseSettings,Callback,cycles,start](TArray<float> OutputVals){
		FNoiseMap2d NoiseMap = FNoiseMap2d(pos,FIntVector2(mapSize.X,mapSize.Y),OutputVals);
		if (NoiseSettings.normalizeMode == Local || NoiseSettings.normalizeMode == LocalPositive){
			Normalize(&NoiseMap,NoiseSettings.normalizeMode,NoiseSettings.type);
		}
		double end = FPlatformTime::Seconds();
		UE_LOG(NoiseGenerator,Log,TEXT("UNoise::GenerateMap2D() ==> %s-%s, Cycles: %d, RunTime: %fs"),*UEnum::GetValueAsString(NoiseSettings.source),*UEnum::GetValueAsString(NoiseSettings.type),cycles,end-start);
		Callback(NoiseMap);
	});
}

void UNoise::GenerateMap2D(FIntVector pos, FIntVector2 mapSize, TArray<FNoiseSettings> NoiseSettings, TFunction<void(FNoiseMap2d NoiseMap)> Callback)
{
	//Count of NoiseSettings with a positive gain in the array
	int32 count = Algo::Accumulate(NoiseSettings, 0, [](int32 Total, const FNoiseSettings& Item) { return Total + (Item.gain > 0 ? 1 : 0); });
	if (count == 0){
		UE_LOG(NoiseGenerator,Error,TEXT("UNoise::GenerateMap2D() ==> No Settings Defined or Total Gain <= 0"));
		return;
	}
	if (count == 1) {
		for (int i = 0; i < NoiseSettings.Num(); i++) {
			if (NoiseSettings[i].gain > 0.0f) {
				GenerateMap2D(pos,mapSize,NoiseSettings[i],Callback);
				break;
			}
		}
		return;
	}
	
	int cycles = 0;
	for (auto Layer : NoiseSettings){ if (Layer.gain > 0) { cycles += (mapSize.X * mapSize.Y) * Layer.octaves; } }	
	double start = FPlatformTime::Seconds();

	//Prepare Results
	TArray<FNoiseComputeShaderDispatchParams> Params;
	TMap<int,FNoiseMap2d> CPUMaps;
	for (int i = 0; i < NoiseSettings.Num(); i++){
		if (NoiseSettings[i].gain <= 0.0f) { continue; }
		if (NoiseSettings[i].source == GPU){
			Params.Add(FNoiseComputeShaderInterface::BuildParams((FVector3f)pos, FVector3f(mapSize.X,mapSize.Y,1.0f),NoiseSettings[i]));
		}
		if (NoiseSettings[i].source == CPU) {
			CPUMaps.Add(i,GenerateMap2D(pos,mapSize,&NoiseSettings[i]));
		}
	}

	FNoiseComputeShaderInterface::LayeredDispatch(Params,[pos,mapSize,CPUMaps,NoiseSettings,Callback,cycles,start](TArray<TArray<float>> OutputVals)	{
		//Collect Results
		int available = OutputVals.Num(); // available buffers
		int used = 0; // buffers checked / converted
		TArray<FNoiseLayerData> ResultData;
		for (int i = 0; i < NoiseSettings.Num(); i++) {
			if (NoiseSettings[i].source == CPU && CPUMaps.Contains(i)) {
				ResultData.Add(FNoiseLayerData(CPUMaps[i],NoiseSettings[i].gain,NoiseSettings[i].curve));
			}
			if (NoiseSettings[i].source == GPU) {
				if (used >= available) { continue; } //Checked all buffers
			 	if (OutputVals[used].Num() == 0) { used++; continue; } //Buffer has no data
				ResultData.Add(FNoiseLayerData(FNoiseMap2d(pos,mapSize,OutputVals[used]),NoiseSettings[i].gain,NoiseSettings[i].curve));
				if (NoiseSettings[i].normalizeMode == Local || NoiseSettings[i].normalizeMode == LocalPositive){
					Normalize(&ResultData[i].noiseMap,NoiseSettings[i].normalizeMode, NoiseSettings[i].type);
				}
				used++;
			}
		}

		//Combine Results
		FNoiseMap2d NoiseMap = FNoiseMap2d(pos,mapSize);
		GenerateMap2D(NoiseMap,&ResultData);
		double end = FPlatformTime::Seconds();
		UE_LOG(NoiseGenerator,Log,TEXT("UNoise::GenerateMap2D() ==> %s [%d], Cycles: %d, RunTime: %f"),TEXT("Layered"),ResultData.Num(),cycles,end-start);
		Callback(NoiseMap);
	});
}

UTexture2D* UNoise::GenerateTexture(FIntVector pos, FIntVector2 mapSize, FNoiseSettings* NoiseSettings, UCurveLinearColor* ColorCurve)
{
	FNoiseMap2d NoiseMap = GenerateMap2D(pos,mapSize,NoiseSettings);
	return GenerateTexture(&NoiseMap,ColorCurve);
}

UTexture2D* UNoise::GenerateTexture(FIntVector pos, FIntVector2 mapSize,TArray<FNoiseSettings>* NoiseSettings, UCurveLinearColor* ColorCurve)
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

FNoiseMap3d UNoise::GenerateMap3D(FIntVector pos, FIntVector mapSize, FNoiseSettings* NoiseSettings, NoiseDensityFunction DensityFunction)
{
	FNoiseMap3d NoiseMap = FNoiseMap3d(pos,mapSize);
	for(int x = 0; x < mapSize.X; x++) {
		for(int y = 0; y < mapSize.Y; y++) {
			for(int z = 0; z < mapSize.Z; z++) {
				FIntVector index = FIntVector(pos.X + x, pos.Y + y, pos.Z + z);
				float value;
				switch(DensityFunction)	{
					case Floor:
						value = index.Z + (Evaluate3D(FVector(index.X,index.Y,index.Z),NoiseSettings)*mapSize.Z);
						break;
					case Sphere:
						value = 500 - index.Size() + Evaluate3D(FVector(index.X,index.Y,index.Z),NoiseSettings);
						break;
					case NoDensityFunction:
					default:
						value = Evaluate3D(FVector(index.X,index.Y,index.Z),NoiseSettings);
						break;
				}
				NoiseMap.Map.Add(index,value);
				NoiseMap.MinMax.Add(NoiseMap.Map[index]);
			}
		}
	}

	Normalize(&NoiseMap,NoiseSettings->normalizeMode,NoiseSettings->type);
	
	return NoiseMap;
}

void UNoise::GenerateMap3D(FIntVector pos, FIntVector mapSize, FNoiseSettings NoiseSettings, NoiseDensityFunction DensityFunction, TFunction<void(FNoiseMap3d NoiseMap)> Callback)
{
	int cycles = mapSize.X * mapSize.Y * NoiseSettings.octaves;
	double start = FPlatformTime::Seconds();

	if(NoiseSettings.source == CPU) {
		FNoiseMap3d NoiseMap = GenerateMap3D(pos,mapSize,&NoiseSettings,DensityFunction);
		double end = FPlatformTime::Seconds();
		UE_LOG(NoiseGenerator,Log,TEXT("UNoise::GenerateMap2D() ==> %s-%s, Cycles: %d, RunTime: %fs"),*UEnum::GetValueAsString(NoiseSettings.source),*UEnum::GetValueAsString(NoiseSettings.type),cycles,end-start);
		Callback(NoiseMap);
		return;
	}

	//NoiseSettings.source == GPU
	FNoiseComputeShaderDispatchParams Params = FNoiseComputeShaderInterface::BuildParams((FVector3f)pos, FVector3f(mapSize.X,mapSize.Y,mapSize.Z), NoiseSettings, D3,DensityFunction);
	FNoiseComputeShaderInterface::Dispatch(Params,[pos,mapSize,NoiseSettings,Callback,cycles,start](TArray<float> OutputVals){
		UE_LOG(LogTemp,Display,TEXT("Generating Map: [%d]"),OutputVals.Num());
		FNoiseMap3d NoiseMap = FNoiseMap3d(pos,mapSize,OutputVals);
		if (NoiseSettings.normalizeMode == Local || NoiseSettings.normalizeMode == LocalPositive){
			Normalize(&NoiseMap,NoiseSettings.normalizeMode,NoiseSettings.type);
		}
		double end = FPlatformTime::Seconds();
		UE_LOG(NoiseGenerator,Log,TEXT("UNoise::GenerateMap2D() ==> %s-%s, Cycles: %d, RunTime: %fs"),*UEnum::GetValueAsString(NoiseSettings.source),*UEnum::GetValueAsString(NoiseSettings.type),cycles,end-start);
		Callback(NoiseMap);
	});
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
