// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "MySimpleComputeShader/MySimpleComputeShader.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UMySimpleComputeShaderLibrary_AsyncExecution;
class UObject;
#ifdef COMPUTESHADERS_MySimpleComputeShader_generated_h
#error "MySimpleComputeShader.generated.h already included, missing '#pragma once' in MySimpleComputeShader.h"
#endif
#define COMPUTESHADERS_MySimpleComputeShader_generated_h

#define FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_MySimpleComputeShader_MySimpleComputeShader_h_67_DELEGATE \
COMPUTESHADERS_API void FOnMySimpleComputeShaderLibrary_AsyncExecutionCompleted_DelegateWrapper(const FMulticastScriptDelegate& OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted, const int32 Value);


#define FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_MySimpleComputeShader_MySimpleComputeShader_h_73_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execExecuteBaseComputeShader);


#define FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_MySimpleComputeShader_MySimpleComputeShader_h_73_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUMySimpleComputeShaderLibrary_AsyncExecution(); \
	friend struct Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution_Statics; \
public: \
	DECLARE_CLASS(UMySimpleComputeShaderLibrary_AsyncExecution, UBlueprintAsyncActionBase, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/ComputeShaders"), NO_API) \
	DECLARE_SERIALIZER(UMySimpleComputeShaderLibrary_AsyncExecution)


#define FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_MySimpleComputeShader_MySimpleComputeShader_h_73_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UMySimpleComputeShaderLibrary_AsyncExecution(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	UMySimpleComputeShaderLibrary_AsyncExecution(UMySimpleComputeShaderLibrary_AsyncExecution&&); \
	UMySimpleComputeShaderLibrary_AsyncExecution(const UMySimpleComputeShaderLibrary_AsyncExecution&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UMySimpleComputeShaderLibrary_AsyncExecution); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UMySimpleComputeShaderLibrary_AsyncExecution); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UMySimpleComputeShaderLibrary_AsyncExecution) \
	NO_API virtual ~UMySimpleComputeShaderLibrary_AsyncExecution();


#define FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_MySimpleComputeShader_MySimpleComputeShader_h_70_PROLOG
#define FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_MySimpleComputeShader_MySimpleComputeShader_h_73_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_MySimpleComputeShader_MySimpleComputeShader_h_73_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_MySimpleComputeShader_MySimpleComputeShader_h_73_INCLASS_NO_PURE_DECLS \
	FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_MySimpleComputeShader_MySimpleComputeShader_h_73_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> COMPUTESHADERS_API UClass* StaticClass<class UMySimpleComputeShaderLibrary_AsyncExecution>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_MySimpleComputeShader_MySimpleComputeShader_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
