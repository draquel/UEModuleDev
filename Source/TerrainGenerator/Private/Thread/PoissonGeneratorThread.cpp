#include "Thread/PoissonGeneratorThread.h"

#include "Noise.h"

bool FPoissonGeneratorThread::Init() {
	return true;
}

uint32 FPoissonGeneratorThread::Run() {
	Points = UNoise::PoissonDiscSample(TopLeft,BottomRight,MinDist,NewPointsCount);
	return 0;
}

void FPoissonGeneratorThread::Exit()
{
	AsyncTask(ENamedThreads::GameThread, [this] ()
	{
		for (auto Point : Points) {
			Data->FoliagePoints.Add(Point);
		}
		Chunk->InstanceFoliage(&Data->FoliageInstances,&Settings->Foliage,&Points,Settings->FoliageOffset,Settings->FoliageScale,Settings->ElevationMinMax,Settings->SlopeMinMax, Settings->EnableCollision);
		Data->hasFoliage = true;
	}); 
}

void FPoissonGeneratorThread::Stop() {
	bShutdown = true;
}
