// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "UObject/Object.h"
#include "TextureUtils.generated.h"

/**
 * 
 */
UCLASS()
class TOOLSLIB_API UTextureUtils : public UObject
{
	GENERATED_BODY()

public:
	static UTextureRenderTarget2D* CreateRenderTarget(FIntVector2 Size, int StepSize, ETextureRenderTargetFormat Format)
	{
		// Create the render target
		UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
    
		// Set the render target properties
		RenderTarget->RenderTargetFormat = Format;
		RenderTarget->InitAutoFormat((Size.X+StepSize)/StepSize, (Size.Y+StepSize)/StepSize);
		RenderTarget->ClearColor = FLinearColor::Black;
		RenderTarget->bAutoGenerateMips = false;
		RenderTarget->UpdateResourceImmediate(true);

		return RenderTarget;
	}

	static float SampleTextureR16f(UTexture2D* Texture, FIntVector2 Coordinates)
	{
		if (!Texture)
		{
			UE_LOG(LogTemp, Error, TEXT("SampleTextureR16f: Texture is NULL"));
			return 0.0f;
		}

		// Access the texture's platform data
		FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];

		int32 Width = Mip.SizeX;
		int32 Height = Mip.SizeY;

		// UE_LOG(LogTemp, Log, TEXT("Texture Width: %d, Height: %d"), Width, Height);

		// Ensure the coordinates are within bounds
		if (Coordinates.X < 0 || Coordinates.X >= Width || Coordinates.Y < 0 || Coordinates.Y >= Height)
		{
			UE_LOG(LogTemp, Error, TEXT("SampleTextureR16f: Coordinates out of bounds (%d, %d)"), Coordinates.X, Coordinates.Y);
			return 0.0f;
		}

		// Lock the mip data for reading
		const void* Data = Mip.BulkData.LockReadOnly();
		if (!Data)
		{
			UE_LOG(LogTemp, Error, TEXT("SampleTextureR16f: Failed to lock texture data"));
			return 0.0f;
		}

		// Calculate the pixel index
		int32 PixelIndex = Coordinates.Y * Width + Coordinates.X;

		// Access the 16-bit float value and convert to 32-bit float
		const uint16* RawData = static_cast<const uint16*>(Data);
		uint16 RawValue = RawData[PixelIndex];

		FFloat16 HalfValue;
		HalfValue.Encoded = RawValue;
		float PixelValue = HalfValue.GetFloat();

		// UE_LOG(LogTemp, Log, TEXT("Sampled Value - 16-bit: %u, Float: %f"), RawValue, PixelValue);

		// Unlock the mip data
		Mip.BulkData.Unlock();

		return PixelValue;
	}
};
