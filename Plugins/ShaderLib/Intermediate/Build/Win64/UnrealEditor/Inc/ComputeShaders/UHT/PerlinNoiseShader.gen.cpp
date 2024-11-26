// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "ComputeShaders/Public/PerlinNoiseShader/PerlinNoiseShader.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodePerlinNoiseShader() {}

// Begin Cross Module References
COMPUTESHADERS_API UClass* Z_Construct_UClass_UPerlinNoiseShaderLibrary();
COMPUTESHADERS_API UClass* Z_Construct_UClass_UPerlinNoiseShaderLibrary_NoRegister();
COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector3f();
ENGINE_API UClass* Z_Construct_UClass_UTextureRenderTarget2D_NoRegister();
UPackage* Z_Construct_UPackage__Script_ComputeShaders();
// End Cross Module References

// Begin Class UPerlinNoiseShaderLibrary Function ExecuteRTComputeShader
struct Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics
{
	struct PerlinNoiseShaderLibrary_eventExecuteRTComputeShader_Parms
	{
		UTextureRenderTarget2D* RT;
		FVector3f Position;
		FVector3f Offset;
		int32 octaves;
		float lacunarity;
		float persistence;
		float scale;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/PerlinNoiseShader/PerlinNoiseShader.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_RT;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Position;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Offset;
	static const UECodeGen_Private::FIntPropertyParams NewProp_octaves;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_lacunarity;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_persistence;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_scale;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::NewProp_RT = { "RT", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(PerlinNoiseShaderLibrary_eventExecuteRTComputeShader_Parms, RT), Z_Construct_UClass_UTextureRenderTarget2D_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::NewProp_Position = { "Position", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(PerlinNoiseShaderLibrary_eventExecuteRTComputeShader_Parms, Position), Z_Construct_UScriptStruct_FVector3f, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::NewProp_Offset = { "Offset", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(PerlinNoiseShaderLibrary_eventExecuteRTComputeShader_Parms, Offset), Z_Construct_UScriptStruct_FVector3f, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::NewProp_octaves = { "octaves", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(PerlinNoiseShaderLibrary_eventExecuteRTComputeShader_Parms, octaves), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::NewProp_lacunarity = { "lacunarity", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(PerlinNoiseShaderLibrary_eventExecuteRTComputeShader_Parms, lacunarity), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::NewProp_persistence = { "persistence", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(PerlinNoiseShaderLibrary_eventExecuteRTComputeShader_Parms, persistence), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::NewProp_scale = { "scale", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(PerlinNoiseShaderLibrary_eventExecuteRTComputeShader_Parms, scale), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::NewProp_RT,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::NewProp_Position,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::NewProp_Offset,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::NewProp_octaves,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::NewProp_lacunarity,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::NewProp_persistence,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::NewProp_scale,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UPerlinNoiseShaderLibrary, nullptr, "ExecuteRTComputeShader", nullptr, nullptr, Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::PropPointers), sizeof(Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::PerlinNoiseShaderLibrary_eventExecuteRTComputeShader_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04822401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::Function_MetaDataParams), Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::PerlinNoiseShaderLibrary_eventExecuteRTComputeShader_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UPerlinNoiseShaderLibrary::execExecuteRTComputeShader)
{
	P_GET_OBJECT(UTextureRenderTarget2D,Z_Param_RT);
	P_GET_STRUCT(FVector3f,Z_Param_Position);
	P_GET_STRUCT(FVector3f,Z_Param_Offset);
	P_GET_PROPERTY(FIntProperty,Z_Param_octaves);
	P_GET_PROPERTY(FFloatProperty,Z_Param_lacunarity);
	P_GET_PROPERTY(FFloatProperty,Z_Param_persistence);
	P_GET_PROPERTY(FFloatProperty,Z_Param_scale);
	P_FINISH;
	P_NATIVE_BEGIN;
	UPerlinNoiseShaderLibrary::ExecuteRTComputeShader(Z_Param_RT,Z_Param_Position,Z_Param_Offset,Z_Param_octaves,Z_Param_lacunarity,Z_Param_persistence,Z_Param_scale);
	P_NATIVE_END;
}
// End Class UPerlinNoiseShaderLibrary Function ExecuteRTComputeShader

// Begin Class UPerlinNoiseShaderLibrary
void UPerlinNoiseShaderLibrary::StaticRegisterNativesUPerlinNoiseShaderLibrary()
{
	UClass* Class = UPerlinNoiseShaderLibrary::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "ExecuteRTComputeShader", &UPerlinNoiseShaderLibrary::execExecuteRTComputeShader },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UPerlinNoiseShaderLibrary);
UClass* Z_Construct_UClass_UPerlinNoiseShaderLibrary_NoRegister()
{
	return UPerlinNoiseShaderLibrary::StaticClass();
}
struct Z_Construct_UClass_UPerlinNoiseShaderLibrary_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// This is a static blueprint library that can be used to invoke our compute shader from blueprints.\n" },
#endif
		{ "IncludePath", "PerlinNoiseShader/PerlinNoiseShader.h" },
		{ "ModuleRelativePath", "Public/PerlinNoiseShader/PerlinNoiseShader.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "This is a static blueprint library that can be used to invoke our compute shader from blueprints." },
#endif
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UPerlinNoiseShaderLibrary_ExecuteRTComputeShader, "ExecuteRTComputeShader" }, // 3328350806
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UPerlinNoiseShaderLibrary>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UPerlinNoiseShaderLibrary_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UObject,
	(UObject* (*)())Z_Construct_UPackage__Script_ComputeShaders,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UPerlinNoiseShaderLibrary_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UPerlinNoiseShaderLibrary_Statics::ClassParams = {
	&UPerlinNoiseShaderLibrary::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UPerlinNoiseShaderLibrary_Statics::Class_MetaDataParams), Z_Construct_UClass_UPerlinNoiseShaderLibrary_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UPerlinNoiseShaderLibrary()
{
	if (!Z_Registration_Info_UClass_UPerlinNoiseShaderLibrary.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UPerlinNoiseShaderLibrary.OuterSingleton, Z_Construct_UClass_UPerlinNoiseShaderLibrary_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UPerlinNoiseShaderLibrary.OuterSingleton;
}
template<> COMPUTESHADERS_API UClass* StaticClass<UPerlinNoiseShaderLibrary>()
{
	return UPerlinNoiseShaderLibrary::StaticClass();
}
UPerlinNoiseShaderLibrary::UPerlinNoiseShaderLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UPerlinNoiseShaderLibrary);
UPerlinNoiseShaderLibrary::~UPerlinNoiseShaderLibrary() {}
// End Class UPerlinNoiseShaderLibrary

// Begin Registration
struct Z_CompiledInDeferFile_FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_PerlinNoiseShader_PerlinNoiseShader_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UPerlinNoiseShaderLibrary, UPerlinNoiseShaderLibrary::StaticClass, TEXT("UPerlinNoiseShaderLibrary"), &Z_Registration_Info_UClass_UPerlinNoiseShaderLibrary, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UPerlinNoiseShaderLibrary), 1171929585U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_PerlinNoiseShader_PerlinNoiseShader_h_2525434369(TEXT("/Script/ComputeShaders"),
	Z_CompiledInDeferFile_FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_PerlinNoiseShader_PerlinNoiseShader_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_PerlinNoiseShader_PerlinNoiseShader_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
