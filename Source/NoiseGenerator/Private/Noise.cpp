#include "Noise.h"
#include "FastNoiseLite.h"
#include "NoiseSettings.h"
#include "Poisson.h"
#include "Curves/CurveLinearColor.h"
#include "NoiseGenerator/Public/NoiseComputeShader/NoiseComputeShader.h"
#include "NoiseGenerator/Public/NoiseTextureComputeShader/NoiseTextureComputeShader.h"

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
		TArray<FIntVector2> indicies;
		NoiseMap->Map.GetKeys(indicies);
		for (int i = 0; i < indicies.Num(); i++) {
			FIntVector2 index = indicies[i];
			NoiseMap->Map[index] = Normalize(normalizeMode,NoiseMap->Map[index],NoiseMap->MinMax,typeMinMax);	
		}
	}
}

void UNoise::Normalize(FNoiseMap3d* NoiseMap, NoiseNormalizeMode normalizeMode, NoiseType noiseType)
{
	FMinMax typeMinMax = GetTypeMinMax(noiseType);	
	
	if (normalizeMode != NoNormalization){
		TArray<FIntVector> indicies;
		NoiseMap->Map.GetKeys(indicies);
		for (int i = 0; i < indicies.Num(); i++) {
			FIntVector index = indicies[i];
			NoiseMap->Map[index] = Normalize(normalizeMode,NoiseMap->Map[index],NoiseMap->MinMax,typeMinMax);	
		}
	}
}

//2DMaps
FNoiseMap2d UNoise::GenerateMap2D(FIntVector pos, FIntVector2 mapSize, int stepSize, FNoiseSettings* NoiseSettings)
{
	FNoiseMap2d NoiseMap = FNoiseMap2d(pos,mapSize,stepSize);
	for(int x = 0; x < mapSize.X; x+=stepSize) {
		for(int y = 0; y < mapSize.Y; y+=stepSize) {
			FIntVector2 index = FIntVector2(pos.X + x, pos.Y + y);
			NoiseMap.Map.Add(index,Evaluate2D(FVector(index.X,index.Y,0),NoiseSettings));
			NoiseMap.MinMax.Add(NoiseMap.Map[index]);
		}
	}	

	Normalize(&NoiseMap,NoiseSettings->normalizeMode,NoiseSettings->type);
	
	return NoiseMap;
}

FNoiseMap2d UNoise::GenerateMap2D(FIntVector pos, FIntVector2 mapSize, int stepSize, TArray<FNoiseSettings>* NoiseSettings)
{
	FNoiseMap2d NoiseMap = FNoiseMap2d(pos,mapSize,stepSize);
	TArray<FNoiseLayer2DData> layerData = TArray<FNoiseLayer2DData>();

	for (int i = 0; i < NoiseSettings->Num(); i++){
		if (NoiseSettings->operator[](i).gain <= 0){ continue; }
		FNoiseLayer2DData data = FNoiseLayer2DData();
		data.noiseMap = GenerateMap2D(pos,mapSize,stepSize,&NoiseSettings->operator[](i));
		data.gain = NoiseSettings->operator[](i).gain;
		data.curve = NoiseSettings->operator[](i).curve;
		layerData.Add(data);
	}

	GenerateMap2D(NoiseMap,&layerData);
	
	return NoiseMap;
}

//Create FNoiseMap2D from a TArray of FNoiseLayerData to create Layered Noise
void UNoise::GenerateMap2D(FNoiseMap2d& NoiseMap, TArray<FNoiseLayer2DData>* layerData)
{
	float gain = 0.0f;
	for (int i = 0; i < layerData->Num(); i++) { gain += layerData->operator[](i).gain; }
	for(int x = 0; x < NoiseMap.Size.X; x+=NoiseMap.StepSize) {
		for(int y = 0; y < NoiseMap.Size.Y; y+=NoiseMap.StepSize) {
			FIntVector2 index = FIntVector2(NoiseMap.Position.X + x, NoiseMap.Position.Y + y);
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

void UNoise::GenerateMap2D(FIntVector pos, FIntVector2 mapSize, int stepSize, FNoiseSettings NoiseSettings, TFunction<void(FNoiseMap2d NoiseMap)> Callback)
{
	int32 cycles = mapSize.X * mapSize.Y/stepSize * NoiseSettings.octaves;
	double start = FPlatformTime::Seconds();

	if(NoiseSettings.source == CPU) {
		FNoiseMap2d NoiseMap = GenerateMap2D(pos,mapSize,stepSize, &NoiseSettings);
		double end = FPlatformTime::Seconds();
		UE_LOG(NoiseGenerator,Log,TEXT("UNoise::GenerateMap2D ==> %s-%s, Cycles: %u, RunTime: %fs"),*UEnum::GetValueAsString(NoiseSettings.source),*UEnum::GetValueAsString(NoiseSettings.type),cycles,end-start);
		Callback(NoiseMap);
		return;
	}

	//NoiseSettings.source == GPU
	TArray<FNoiseSettings> NoiseSettingsArray;
	NoiseSettingsArray.Add(NoiseSettings);	
	FNoiseComputeShaderDispatchParams Params = FNoiseComputeShaderInterface::BuildParams((FVector3f)pos, FVector3f(mapSize.X/stepSize,mapSize.Y/stepSize,1),stepSize, NoiseSettingsArray, D2, NoDensityFunction);
	FNoiseComputeShaderInterface::Dispatch(Params,[pos,mapSize,stepSize,NoiseSettings,Callback,cycles,start](TArray<float> OutputVals){
		FNoiseMap2d NoiseMap = FNoiseMap2d(pos,mapSize,stepSize,OutputVals);
		if (NoiseSettings.normalizeMode == Local || NoiseSettings.normalizeMode == LocalPositive){
			Normalize(&NoiseMap,NoiseSettings.normalizeMode,NoiseSettings.type);
		}
		double end = FPlatformTime::Seconds();
		UE_LOG(NoiseGenerator,Log,TEXT("UNoise::GenerateMap2D ==> %s-%s, Cycles: %u, RunTime: %fs"),*UEnum::GetValueAsString(NoiseSettings.source),*UEnum::GetValueAsString(NoiseSettings.type),cycles,end-start);
		Callback(NoiseMap);
	});
}

void UNoise::GenerateMap2D(FIntVector pos, FIntVector2 mapSize, int stepSize, TArray<FNoiseSettings> NoiseSettings, TFunction<void(FNoiseMap2d NoiseMap)> Callback)
{
	//Count of NoiseSettings with a positive gain in the array
	int32 count = Algo::Accumulate(NoiseSettings, 0, [](int32 Total, const FNoiseSettings& Item) { return Total + (Item.gain > 0 ? 1 : 0); });
	if (count == 0){
		UE_LOG(NoiseGenerator,Error,TEXT("UNoise::GenerateMap2D ==> No Settings Defined or Total Gain <= 0"));
		return;
	}
	if (count == 1) {
		for (int i = 0; i < NoiseSettings.Num(); i++) {
			if (NoiseSettings[i].gain > 0.0f) {
				GenerateMap2D(pos,mapSize,stepSize,NoiseSettings[i], Callback);
				break;
			}
		}
		return;
	}
	
	int32 cycles = 0;
	for (auto Layer : NoiseSettings){ if (Layer.gain > 0) { cycles += (mapSize.X * mapSize.Y/stepSize) * Layer.octaves; } }	
	double start = FPlatformTime::Seconds();

	//Prepare Results
	TMap<int,FNoiseMap2d> CPUMaps;
	TArray<FNoiseSettings> GPUSettings;
	for (int i = 0; i < NoiseSettings.Num(); i++){
		if (NoiseSettings[i].gain <= 0.0f) { continue; }
		if (NoiseSettings[i].source == GPU){
			GPUSettings.Add(NoiseSettings[i]);	
		}
		if (NoiseSettings[i].source == CPU) {
			CPUMaps.Add(i,GenerateMap2D(pos,mapSize,stepSize,&NoiseSettings[i]));
		}
	}

	if (GPUSettings.Num() == 0)	{
		FNoiseMap2d NoiseMap = FNoiseMap2d(pos,mapSize,stepSize);
		TArray<FNoiseLayer2DData> ResultData;
		for (int i = 0; i < NoiseSettings.Num(); i++) {
			if (NoiseSettings[i].source == CPU && CPUMaps.Contains(i)) {
				ResultData.Add(FNoiseLayer2DData(CPUMaps[i],NoiseSettings[i].gain,NoiseSettings[i].curve));	
			}
		}
		GenerateMap2D(NoiseMap,&ResultData);
		double end = FPlatformTime::Seconds();
		UE_LOG(NoiseGenerator,Log,TEXT("UNoise::GenerateMap2D ==> %s [%d], Cycles: %u, RunTime: %f"),TEXT("Layered - CPU"),ResultData.Num(),cycles,end-start);
		Callback(NoiseMap);
		return;
	}
	
	FNoiseComputeShaderDispatchParams Params = FNoiseComputeShaderInterface::BuildParams((FVector3f)pos, FVector3f(mapSize.X,mapSize.Y,stepSize)/stepSize,stepSize,GPUSettings,D2,NoDensityFunction);
	FNoiseComputeShaderInterface::Dispatch(Params,[pos,mapSize,stepSize,CPUMaps,NoiseSettings,Callback,cycles,start](TArray<float> OutputVals)	{
		float cpuGain = 0;
		float gpuGain = 0;
		TArray<FNoiseLayer2DData> ResultData;
		FNoiseMap2d NoiseMap = FNoiseMap2d(pos,mapSize,stepSize);
		
		for (int i = 0; i < NoiseSettings.Num(); i++) {
			if (NoiseSettings[i].source == CPU && CPUMaps.Contains(i)) {
				cpuGain += NoiseSettings[i].gain;
				ResultData.Add(FNoiseLayer2DData(CPUMaps[i],NoiseSettings[i].gain,NoiseSettings[i].curve));	
			}
			if (NoiseSettings[i].source == GPU && NoiseSettings[i].gain > 0.0f)	{
				gpuGain += NoiseSettings[i].gain;
			}
		}
		if (OutputVals.Num() > 0) {
			FNoiseMap2d GPUNoiseMap = FNoiseMap2d(pos,mapSize,stepSize,OutputVals);
			ResultData.Add(FNoiseLayer2DData(FNoiseMap2d(pos,mapSize,stepSize,OutputVals),gpuGain,nullptr));
		}

		GenerateMap2D(NoiseMap,&ResultData);
		double end = FPlatformTime::Seconds();
		UE_LOG(NoiseGenerator,Log,TEXT("UNoise::GenerateMap2D ==> %s [%d], Cycles: %u, RunTime: %f"),CPUMaps.Num()>0?TEXT("Layered CPU/GPU"):TEXT("Layered GPU"),ResultData.Num(),cycles,end-start);
		Callback(NoiseMap);
	});
}

UTexture2D* UNoise::GenerateTexture(FIntVector pos, FIntVector2 mapSize, FNoiseSettings* NoiseSettings, UCurveLinearColor* ColorCurve)
{
	FNoiseMap2d NoiseMap = GenerateMap2D(pos,mapSize,1, NoiseSettings);
	return GenerateTexture(&NoiseMap,ColorCurve);
}

UTexture2D* UNoise::GenerateTexture(FIntVector pos, FIntVector2 mapSize,TArray<FNoiseSettings>* NoiseSettings, UCurveLinearColor* ColorCurve)
{
	FNoiseMap2d NoiseMap = GenerateMap2D(pos,mapSize,1, NoiseSettings);
	return GenerateTexture(&NoiseMap,ColorCurve);
}

UTexture2D* UNoise::GenerateTexture(FNoiseMap2d* NoiseMap, UCurveLinearColor* ColorCurve)
{
	UTexture2D* texture = UTexture2D::CreateTransient(NoiseMap->Size.X,NoiseMap->Size.Y);
	TArray<FColor> colors;

	for(int x = 0; x < NoiseMap->Size.X; x++) {
		for(int y = 0; y < NoiseMap->Size.Y; y++) {
			FIntVector2 index = FIntVector2(NoiseMap->Position.X + x, NoiseMap->Position.Y + y);
			if (!NoiseMap->Map.Contains(index)) { colors.Add(FColor(FColor::Black)); continue; }
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

void UNoise::GenerateTexture(UTextureRenderTarget2D* RenderTarget, FIntVector pos, FIntVector2 Size, int stepSize, TArray<FNoiseSettings>* NoiseSettings, TFunction<void()> Callback)
{
	double start = FPlatformTime::Seconds();
	FNoiseTextureComputeShaderInterface::Dispatch(FNoiseTextureComputeShaderInterface::BuildParams(RenderTarget,(FVector3f)pos,FVector3f(Size.X,Size.Y,1),stepSize,*NoiseSettings),[RenderTarget,Size,stepSize,start,Callback]()	{
		double end = FPlatformTime::Seconds();
		UE_LOG(NoiseGenerator,Log,TEXT("UNoise::GenerateTexture ==> Size:%s, RunTime: %fs"),*(Size/stepSize).ToString(),end-start);
		Callback();
	});
}

//3DMaps
FNoiseMap3d UNoise::GenerateMap3D(FIntVector pos, FIntVector mapSize, int stepSize, FNoiseSettings* NoiseSettings, NoiseDensityFunction DensityFunction)
{
	FNoiseMap3d NoiseMap = FNoiseMap3d(pos,mapSize);
	for(int x = 0; x < mapSize.X; x += stepSize) {
		for(int y = 0; y < mapSize.Y; y+= stepSize) {
			for(int z = 0; z < mapSize.Z; z+= stepSize) {
				FIntVector index = pos + FIntVector(x,y,z);
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
				// UE_LOG(NoiseGenerator,Warning,TEXT("UNoise::GenerateMap3D => new map index %s"),*index.ToString());
				NoiseMap.Map.Add(index,value);
				NoiseMap.MinMax.Add(NoiseMap.Map[index]);
			}
		}
	}

	Normalize(&NoiseMap,NoiseSettings->normalizeMode,NoiseSettings->type);
	
	return NoiseMap;
}
//Create FNoiseMap2D from a TArray of FNoiseLayerData to create Layered Noise
void UNoise::GenerateMap3D(FNoiseMap3d& NoiseMap, TArray<FNoiseLayer3DData>* layerData)
{
	float gain = 0.0f;
	for (int i = 0; i < layerData->Num(); i++) { gain += layerData->operator[](i).gain; }
	for(int x = 0; x < NoiseMap.Size.X; x+=NoiseMap.StepSize) {
		for(int y = 0; y < NoiseMap.Size.Y; y+=NoiseMap.StepSize) {
			for(int z = 0; z < NoiseMap.Size.Z; z+=NoiseMap.StepSize) {
				FIntVector index = FIntVector(NoiseMap.Position.X + x, NoiseMap.Position.Y + y, NoiseMap.Position.Z + z);
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
}

void UNoise::GenerateMap3D(FIntVector pos, FIntVector mapSize, int stepSize, FNoiseSettings NoiseSettings, NoiseDensityFunction DensityFunction, TFunction<void(FNoiseMap3d NoiseMap)> Callback)
{
	int32 cycles = ((mapSize.X * mapSize.Y * mapSize.Z) / stepSize) * NoiseSettings.octaves;
	double start = FPlatformTime::Seconds();
	
	//Alignment error
	if (FMath::Modulo(mapSize.X,stepSize) != 0 || FMath::Modulo(mapSize.Y, stepSize) != 0 || FMath::Modulo(mapSize.Z, stepSize) != 0){
		UE_LOG(NoiseGenerator,Error,TEXT("NoiseGenerator::GenerateMap3D ==> The MapSize[(%d,%d,%d)] must be evenly divisible by the StepSize[%d]."),mapSize.X,mapSize.Y,mapSize.Z,stepSize);
		return;
	}
	
	if(NoiseSettings.source == CPU) {
		FNoiseMap3d NoiseMap = GenerateMap3D(pos,mapSize,stepSize,&NoiseSettings,DensityFunction);
		double end = FPlatformTime::Seconds();
		UE_LOG(NoiseGenerator,Log,TEXT("UNoise::GenerateMap3D ==> %s-%s, Cycles: %u, RunTime: %fs"),*UEnum::GetValueAsString(NoiseSettings.source),*UEnum::GetValueAsString(NoiseSettings.type),cycles,end-start);
		Callback(NoiseMap);
		return;
	}

	//NoiseSettings.source == GPU
	TArray<FNoiseSettings> NoiseSettingsArray;
	NoiseSettingsArray.Add(NoiseSettings);
	FNoiseComputeShaderDispatchParams Params = FNoiseComputeShaderInterface::BuildParams((FVector3f)pos, FVector3f(mapSize.X,mapSize.Y,mapSize.Z)/stepSize,stepSize,NoiseSettingsArray,D3,DensityFunction);
	FNoiseComputeShaderInterface::Dispatch(Params,[pos,mapSize,stepSize,NoiseSettings,Callback,cycles,start](TArray<float> OutputVals){
		FNoiseMap3d NoiseMap = FNoiseMap3d(pos,mapSize,stepSize,OutputVals);
		if (NoiseSettings.normalizeMode == Local || NoiseSettings.normalizeMode == LocalPositive){
			Normalize(&NoiseMap,NoiseSettings.normalizeMode,NoiseSettings.type);
		}
		double end = FPlatformTime::Seconds();
		UE_LOG(NoiseGenerator,Log,TEXT("UNoise::GenerateMap3D ==> %s-%s, Cycles: %u, RunTime: %fs"),*UEnum::GetValueAsString(NoiseSettings.source),*UEnum::GetValueAsString(NoiseSettings.type),cycles,end-start);
		Callback(NoiseMap);
	});
}

void UNoise::GenerateMap3D(FIntVector pos, FIntVector mapSize, int stepSize, TArray<FNoiseSettings> NoiseSettings, NoiseDensityFunction DensityFunction, TFunction<void(FNoiseMap3d NoiseMap)> Callback)
{
	//Count of NoiseSettings with a positive gain in the array
	int32 count = Algo::Accumulate(NoiseSettings, 0, [](int32 Total, const FNoiseSettings& Item) { return Total + (Item.gain > 0 ? 1 : 0); });
	if (count == 0){
		UE_LOG(NoiseGenerator,Error,TEXT("UNoise::GenerateMap3D ==> No Settings Defined or Total Gain <= 0"));
		return;
	}
	if (count == 1) {
		for (int i = 0; i < NoiseSettings.Num(); i++) {
			if (NoiseSettings[i].gain > 0.0f) {
				GenerateMap3D(pos,mapSize,stepSize,NoiseSettings[i],DensityFunction,Callback);
				break;
			}
		}
		return;
	}
	
	int32 cycles = 0;
	for (auto Layer : NoiseSettings){ if (Layer.gain > 0) { cycles += ((mapSize.X * mapSize.Y * mapSize.Z)/stepSize) * Layer.octaves; } }	
	double start = FPlatformTime::Seconds();

	//Prepare Results
	TMap<int,FNoiseMap3d> CPUMaps;
	TArray<FNoiseSettings> GPUSettings;
	for (int i = 0; i < NoiseSettings.Num(); i++){
		if (NoiseSettings[i].gain <= 0.0f) { continue; }
		if (NoiseSettings[i].source == GPU){
			GPUSettings.Add(NoiseSettings[i]);
		}
		if (NoiseSettings[i].source == CPU) {
			CPUMaps.Add(i,GenerateMap3D(pos,mapSize,stepSize,&NoiseSettings[i],DensityFunction));
		}
	}
	
	FNoiseComputeShaderDispatchParams Params = FNoiseComputeShaderInterface::BuildParams((FVector3f)pos, FVector3f(mapSize.X,mapSize.Y,mapSize.Z)/stepSize,stepSize,GPUSettings,D3,DensityFunction);
	FNoiseComputeShaderInterface::Dispatch(Params,[pos,mapSize,stepSize,CPUMaps,NoiseSettings,Callback,cycles,start](TArray<float> OutputVals)	{
		float cpuGain = 0;
		float gpuGain = 0;
		TArray<FNoiseLayer3DData> ResultData;
		FNoiseMap3d NoiseMap = FNoiseMap3d(pos,mapSize,stepSize);
		
		for (int i = 0; i < NoiseSettings.Num(); i++) {
			if (NoiseSettings[i].source == CPU && CPUMaps.Contains(i)) {
				cpuGain += NoiseSettings[i].gain;
				ResultData.Add(FNoiseLayer3DData(CPUMaps[i],NoiseSettings[i].gain,NoiseSettings[i].curve));	
			}
			if (NoiseSettings[i].source == GPU && NoiseSettings[i].gain > 0.0f)	{
				gpuGain += NoiseSettings[i].gain;
			}
		}
		if (OutputVals.Num() > 0) {
			FNoiseMap3d GPUNoiseMap = FNoiseMap3d(pos,mapSize,stepSize,OutputVals);
			ResultData.Add(FNoiseLayer3DData(FNoiseMap3d(pos,mapSize,stepSize,OutputVals),gpuGain,nullptr));
		}
		
		GenerateMap3D(NoiseMap,&ResultData);
		double end = FPlatformTime::Seconds();
		UE_LOG(NoiseGenerator,Log,TEXT("UNoise::GenerateMap3D ==> %s [%d], Cycles: %u, RunTime: %f"),TEXT("Layered"),ResultData.Num(),cycles,end-start);
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


UTextureRenderTarget2D* UNoise::CreateRenderTarget(FIntVector2 Size, ETextureRenderTargetFormat Format)
{
	// Create the render target
	UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
    
	// Set the render target properties
	RenderTarget->RenderTargetFormat = Format;
	RenderTarget->InitAutoFormat(Size.X, Size.Y);
	RenderTarget->ClearColor = FLinearColor::Black;
	RenderTarget->bAutoGenerateMips = false;
	RenderTarget->UpdateResourceImmediate(true);

	return RenderTarget;
}