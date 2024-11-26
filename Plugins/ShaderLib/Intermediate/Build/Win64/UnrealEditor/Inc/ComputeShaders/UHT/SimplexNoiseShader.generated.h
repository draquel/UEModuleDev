// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "SimplexNoiseShader/SimplexNoiseShader.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UTextureRenderTarget2D;
#ifdef COMPUTESHADERS_SimplexNoiseShader_generated_h
#error "SimplexNoiseShader.generated.h already included, missing '#pragma once' in SimplexNoiseShader.h"
#endif
#define COMPUTESHADERS_SimplexNoiseShader_generated_h

#define FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_SimplexNoiseShader_SimplexNoiseShader_h_68_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execExecuteRTComputeShader);


#define FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_SimplexNoiseShader_SimplexNoiseShader_h_68_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUSimplexNoiseShaderLibrary(); \
	friend struct Z_Construct_UClass_USimplexNoiseShaderLibrary_Statics; \
public: \
	DECLARE_CLASS(USimplexNoiseShaderLibrary, UObject, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/ComputeShaders"), NO_API) \
	DECLARE_SERIALIZER(USimplexNoiseShaderLibrary)


#define FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_SimplexNoiseShader_SimplexNoiseShader_h_68_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API USimplexNoiseShaderLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	USimplexNoiseShaderLibrary(USimplexNoiseShaderLibrary&&); \
	USimplexNoiseShaderLibrary(const USimplexNoiseShaderLibrary&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, USimplexNoiseShaderLibrary); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(USimplexNoiseShaderLibrary); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(USimplexNoiseShaderLibrary) \
	NO_API virtual ~USimplexNoiseShaderLibrary();


#define FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_SimplexNoiseShader_SimplexNoiseShader_h_65_PROLOG
#define FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_SimplexNoiseShader_SimplexNoiseShader_h_68_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_SimplexNoiseShader_SimplexNoiseShader_h_68_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_SimplexNoiseShader_SimplexNoiseShader_h_68_INCLASS_NO_PURE_DECLS \
	FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_SimplexNoiseShader_SimplexNoiseShader_h_68_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> COMPUTESHADERS_API UClass* StaticClass<class USimplexNoiseShaderLibrary>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_SimplexNoiseShader_SimplexNoiseShader_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
