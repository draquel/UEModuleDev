// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "ComputeShaders/Public/SimplexNoiseShader/SimplexNoiseShader.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSimplexNoiseShader() {}

// Begin Cross Module References
COMPUTESHADERS_API UClass* Z_Construct_UClass_USimplexNoiseShaderLibrary();
COMPUTESHADERS_API UClass* Z_Construct_UClass_USimplexNoiseShaderLibrary_NoRegister();
COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
ENGINE_API UClass* Z_Construct_UClass_UTextureRenderTarget2D_NoRegister();
UPackage* Z_Construct_UPackage__Script_ComputeShaders();
// End Cross Module References

// Begin Class USimplexNoiseShaderLibrary Function ExecuteRTComputeShader
struct Z_Construct_UFunction_USimplexNoiseShaderLibrary_ExecuteRTComputeShader_Statics
{
	struct SimplexNoiseShaderLibrary_eventExecuteRTComputeShader_Parms
	{
		UTextureRenderTarget2D* RT;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/SimplexNoiseShader/SimplexNoiseShader.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_RT;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_USimplexNoiseShaderLibrary_ExecuteRTComputeShader_Statics::NewProp_RT = { "RT", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SimplexNoiseShaderLibrary_eventExecuteRTComputeShader_Parms, RT), Z_Construct_UClass_UTextureRenderTarget2D_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USimplexNoiseShaderLibrary_ExecuteRTComputeShader_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USimplexNoiseShaderLibrary_ExecuteRTComputeShader_Statics::NewProp_RT,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USimplexNoiseShaderLibrary_ExecuteRTComputeShader_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USimplexNoiseShaderLibrary_ExecuteRTComputeShader_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_USimplexNoiseShaderLibrary, nullptr, "ExecuteRTComputeShader", nullptr, nullptr, Z_Construct_UFunction_USimplexNoiseShaderLibrary_ExecuteRTComputeShader_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_USimplexNoiseShaderLibrary_ExecuteRTComputeShader_Statics::PropPointers), sizeof(Z_Construct_UFunction_USimplexNoiseShaderLibrary_ExecuteRTComputeShader_Statics::SimplexNoiseShaderLibrary_eventExecuteRTComputeShader_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USimplexNoiseShaderLibrary_ExecuteRTComputeShader_Statics::Function_MetaDataParams), Z_Construct_UFunction_USimplexNoiseShaderLibrary_ExecuteRTComputeShader_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_USimplexNoiseShaderLibrary_ExecuteRTComputeShader_Statics::SimplexNoiseShaderLibrary_eventExecuteRTComputeShader_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USimplexNoiseShaderLibrary_ExecuteRTComputeShader()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USimplexNoiseShaderLibrary_ExecuteRTComputeShader_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USimplexNoiseShaderLibrary::execExecuteRTComputeShader)
{
	P_GET_OBJECT(UTextureRenderTarget2D,Z_Param_RT);
	P_FINISH;
	P_NATIVE_BEGIN;
	USimplexNoiseShaderLibrary::ExecuteRTComputeShader(Z_Param_RT);
	P_NATIVE_END;
}
// End Class USimplexNoiseShaderLibrary Function ExecuteRTComputeShader

// Begin Class USimplexNoiseShaderLibrary
void USimplexNoiseShaderLibrary::StaticRegisterNativesUSimplexNoiseShaderLibrary()
{
	UClass* Class = USimplexNoiseShaderLibrary::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "ExecuteRTComputeShader", &USimplexNoiseShaderLibrary::execExecuteRTComputeShader },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(USimplexNoiseShaderLibrary);
UClass* Z_Construct_UClass_USimplexNoiseShaderLibrary_NoRegister()
{
	return USimplexNoiseShaderLibrary::StaticClass();
}
struct Z_Construct_UClass_USimplexNoiseShaderLibrary_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// This is a static blueprint library that can be used to invoke our compute shader from blueprints.\n" },
#endif
		{ "IncludePath", "SimplexNoiseShader/SimplexNoiseShader.h" },
		{ "ModuleRelativePath", "Public/SimplexNoiseShader/SimplexNoiseShader.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "This is a static blueprint library that can be used to invoke our compute shader from blueprints." },
#endif
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_USimplexNoiseShaderLibrary_ExecuteRTComputeShader, "ExecuteRTComputeShader" }, // 2412527235
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<USimplexNoiseShaderLibrary>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_USimplexNoiseShaderLibrary_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UObject,
	(UObject* (*)())Z_Construct_UPackage__Script_ComputeShaders,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USimplexNoiseShaderLibrary_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_USimplexNoiseShaderLibrary_Statics::ClassParams = {
	&USimplexNoiseShaderLibrary::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	0,
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_USimplexNoiseShaderLibrary_Statics::Class_MetaDataParams), Z_Construct_UClass_USimplexNoiseShaderLibrary_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_USimplexNoiseShaderLibrary()
{
	if (!Z_Registration_Info_UClass_USimplexNoiseShaderLibrary.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_USimplexNoiseShaderLibrary.OuterSingleton, Z_Construct_UClass_USimplexNoiseShaderLibrary_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_USimplexNoiseShaderLibrary.OuterSingleton;
}
template<> COMPUTESHADERS_API UClass* StaticClass<USimplexNoiseShaderLibrary>()
{
	return USimplexNoiseShaderLibrary::StaticClass();
}
USimplexNoiseShaderLibrary::USimplexNoiseShaderLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(USimplexNoiseShaderLibrary);
USimplexNoiseShaderLibrary::~USimplexNoiseShaderLibrary() {}
// End Class USimplexNoiseShaderLibrary

// Begin Registration
struct Z_CompiledInDeferFile_FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_SimplexNoiseShader_SimplexNoiseShader_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_USimplexNoiseShaderLibrary, USimplexNoiseShaderLibrary::StaticClass, TEXT("USimplexNoiseShaderLibrary"), &Z_Registration_Info_UClass_USimplexNoiseShaderLibrary, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(USimplexNoiseShaderLibrary), 1004813088U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_SimplexNoiseShader_SimplexNoiseShader_h_2575926944(TEXT("/Script/ComputeShaders"),
	Z_CompiledInDeferFile_FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_SimplexNoiseShader_SimplexNoiseShader_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_SimplexNoiseShader_SimplexNoiseShader_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
