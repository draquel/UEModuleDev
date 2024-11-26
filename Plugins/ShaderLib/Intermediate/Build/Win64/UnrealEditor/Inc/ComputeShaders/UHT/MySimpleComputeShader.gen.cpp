// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "ComputeShaders/Public/MySimpleComputeShader/MySimpleComputeShader.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeMySimpleComputeShader() {}

// Begin Cross Module References
COMPUTESHADERS_API UClass* Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution();
COMPUTESHADERS_API UClass* Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution_NoRegister();
COMPUTESHADERS_API UFunction* Z_Construct_UDelegateFunction_ComputeShaders_OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted__DelegateSignature();
COREUOBJECT_API UClass* Z_Construct_UClass_UObject_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UBlueprintAsyncActionBase();
UPackage* Z_Construct_UPackage__Script_ComputeShaders();
// End Cross Module References

// Begin Delegate FOnMySimpleComputeShaderLibrary_AsyncExecutionCompleted
struct Z_Construct_UDelegateFunction_ComputeShaders_OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted__DelegateSignature_Statics
{
	struct _Script_ComputeShaders_eventOnMySimpleComputeShaderLibrary_AsyncExecutionCompleted_Parms
	{
		int32 Value;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/MySimpleComputeShader/MySimpleComputeShader.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Value_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FIntPropertyParams NewProp_Value;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FIntPropertyParams Z_Construct_UDelegateFunction_ComputeShaders_OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted__DelegateSignature_Statics::NewProp_Value = { "Value", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(_Script_ComputeShaders_eventOnMySimpleComputeShaderLibrary_AsyncExecutionCompleted_Parms, Value), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Value_MetaData), NewProp_Value_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UDelegateFunction_ComputeShaders_OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted__DelegateSignature_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UDelegateFunction_ComputeShaders_OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted__DelegateSignature_Statics::NewProp_Value,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UDelegateFunction_ComputeShaders_OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted__DelegateSignature_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UDelegateFunction_ComputeShaders_OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted__DelegateSignature_Statics::FuncParams = { (UObject*(*)())Z_Construct_UPackage__Script_ComputeShaders, nullptr, "OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted__DelegateSignature", nullptr, nullptr, Z_Construct_UDelegateFunction_ComputeShaders_OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted__DelegateSignature_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UDelegateFunction_ComputeShaders_OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted__DelegateSignature_Statics::PropPointers), sizeof(Z_Construct_UDelegateFunction_ComputeShaders_OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted__DelegateSignature_Statics::_Script_ComputeShaders_eventOnMySimpleComputeShaderLibrary_AsyncExecutionCompleted_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00130000, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UDelegateFunction_ComputeShaders_OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted__DelegateSignature_Statics::Function_MetaDataParams), Z_Construct_UDelegateFunction_ComputeShaders_OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted__DelegateSignature_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UDelegateFunction_ComputeShaders_OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted__DelegateSignature_Statics::_Script_ComputeShaders_eventOnMySimpleComputeShaderLibrary_AsyncExecutionCompleted_Parms) < MAX_uint16);
UFunction* Z_Construct_UDelegateFunction_ComputeShaders_OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted__DelegateSignature()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UDelegateFunction_ComputeShaders_OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted__DelegateSignature_Statics::FuncParams);
	}
	return ReturnFunction;
}
void FOnMySimpleComputeShaderLibrary_AsyncExecutionCompleted_DelegateWrapper(const FMulticastScriptDelegate& OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted, const int32 Value)
{
	struct _Script_ComputeShaders_eventOnMySimpleComputeShaderLibrary_AsyncExecutionCompleted_Parms
	{
		int32 Value;
	};
	_Script_ComputeShaders_eventOnMySimpleComputeShaderLibrary_AsyncExecutionCompleted_Parms Parms;
	Parms.Value=Value;
	OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted.ProcessMulticastDelegate<UObject>(&Parms);
}
// End Delegate FOnMySimpleComputeShaderLibrary_AsyncExecutionCompleted

// Begin Class UMySimpleComputeShaderLibrary_AsyncExecution Function ExecuteBaseComputeShader
struct Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader_Statics
{
	struct MySimpleComputeShaderLibrary_AsyncExecution_eventExecuteBaseComputeShader_Parms
	{
		UObject* WorldContextObject;
		int32 Arg1;
		int32 Arg2;
		UMySimpleComputeShaderLibrary_AsyncExecution* ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "BlueprintInternalUseOnly", "true" },
		{ "Category", "ComputeShader" },
		{ "ModuleRelativePath", "Public/MySimpleComputeShader/MySimpleComputeShader.h" },
		{ "WorldContext", "WorldContextObject" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_WorldContextObject;
	static const UECodeGen_Private::FIntPropertyParams NewProp_Arg1;
	static const UECodeGen_Private::FIntPropertyParams NewProp_Arg2;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader_Statics::NewProp_WorldContextObject = { "WorldContextObject", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(MySimpleComputeShaderLibrary_AsyncExecution_eventExecuteBaseComputeShader_Parms, WorldContextObject), Z_Construct_UClass_UObject_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader_Statics::NewProp_Arg1 = { "Arg1", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(MySimpleComputeShaderLibrary_AsyncExecution_eventExecuteBaseComputeShader_Parms, Arg1), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader_Statics::NewProp_Arg2 = { "Arg2", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(MySimpleComputeShaderLibrary_AsyncExecution_eventExecuteBaseComputeShader_Parms, Arg2), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(MySimpleComputeShaderLibrary_AsyncExecution_eventExecuteBaseComputeShader_Parms, ReturnValue), Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader_Statics::NewProp_WorldContextObject,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader_Statics::NewProp_Arg1,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader_Statics::NewProp_Arg2,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution, nullptr, "ExecuteBaseComputeShader", nullptr, nullptr, Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader_Statics::PropPointers), sizeof(Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader_Statics::MySimpleComputeShaderLibrary_AsyncExecution_eventExecuteBaseComputeShader_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader_Statics::Function_MetaDataParams), Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader_Statics::MySimpleComputeShaderLibrary_AsyncExecution_eventExecuteBaseComputeShader_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UMySimpleComputeShaderLibrary_AsyncExecution::execExecuteBaseComputeShader)
{
	P_GET_OBJECT(UObject,Z_Param_WorldContextObject);
	P_GET_PROPERTY(FIntProperty,Z_Param_Arg1);
	P_GET_PROPERTY(FIntProperty,Z_Param_Arg2);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(UMySimpleComputeShaderLibrary_AsyncExecution**)Z_Param__Result=UMySimpleComputeShaderLibrary_AsyncExecution::ExecuteBaseComputeShader(Z_Param_WorldContextObject,Z_Param_Arg1,Z_Param_Arg2);
	P_NATIVE_END;
}
// End Class UMySimpleComputeShaderLibrary_AsyncExecution Function ExecuteBaseComputeShader

// Begin Class UMySimpleComputeShaderLibrary_AsyncExecution
void UMySimpleComputeShaderLibrary_AsyncExecution::StaticRegisterNativesUMySimpleComputeShaderLibrary_AsyncExecution()
{
	UClass* Class = UMySimpleComputeShaderLibrary_AsyncExecution::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "ExecuteBaseComputeShader", &UMySimpleComputeShaderLibrary_AsyncExecution::execExecuteBaseComputeShader },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UMySimpleComputeShaderLibrary_AsyncExecution);
UClass* Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution_NoRegister()
{
	return UMySimpleComputeShaderLibrary_AsyncExecution::StaticClass();
}
struct Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "MySimpleComputeShader/MySimpleComputeShader.h" },
		{ "ModuleRelativePath", "Public/MySimpleComputeShader/MySimpleComputeShader.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Completed_MetaData[] = {
		{ "ModuleRelativePath", "Public/MySimpleComputeShader/MySimpleComputeShader.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FMulticastDelegatePropertyParams NewProp_Completed;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UMySimpleComputeShaderLibrary_AsyncExecution_ExecuteBaseComputeShader, "ExecuteBaseComputeShader" }, // 4257522438
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UMySimpleComputeShaderLibrary_AsyncExecution>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FMulticastDelegatePropertyParams Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution_Statics::NewProp_Completed = { "Completed", nullptr, (EPropertyFlags)0x0010000010080000, UECodeGen_Private::EPropertyGenFlags::InlineMulticastDelegate, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UMySimpleComputeShaderLibrary_AsyncExecution, Completed), Z_Construct_UDelegateFunction_ComputeShaders_OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted__DelegateSignature, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Completed_MetaData), NewProp_Completed_MetaData) }; // 2503454488
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution_Statics::NewProp_Completed,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UBlueprintAsyncActionBase,
	(UObject* (*)())Z_Construct_UPackage__Script_ComputeShaders,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution_Statics::ClassParams = {
	&UMySimpleComputeShaderLibrary_AsyncExecution::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution_Statics::PropPointers),
	0,
	0x009000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution_Statics::Class_MetaDataParams), Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution()
{
	if (!Z_Registration_Info_UClass_UMySimpleComputeShaderLibrary_AsyncExecution.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UMySimpleComputeShaderLibrary_AsyncExecution.OuterSingleton, Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UMySimpleComputeShaderLibrary_AsyncExecution.OuterSingleton;
}
template<> COMPUTESHADERS_API UClass* StaticClass<UMySimpleComputeShaderLibrary_AsyncExecution>()
{
	return UMySimpleComputeShaderLibrary_AsyncExecution::StaticClass();
}
UMySimpleComputeShaderLibrary_AsyncExecution::UMySimpleComputeShaderLibrary_AsyncExecution(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UMySimpleComputeShaderLibrary_AsyncExecution);
UMySimpleComputeShaderLibrary_AsyncExecution::~UMySimpleComputeShaderLibrary_AsyncExecution() {}
// End Class UMySimpleComputeShaderLibrary_AsyncExecution

// Begin Registration
struct Z_CompiledInDeferFile_FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_MySimpleComputeShader_MySimpleComputeShader_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UMySimpleComputeShaderLibrary_AsyncExecution, UMySimpleComputeShaderLibrary_AsyncExecution::StaticClass, TEXT("UMySimpleComputeShaderLibrary_AsyncExecution"), &Z_Registration_Info_UClass_UMySimpleComputeShaderLibrary_AsyncExecution, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UMySimpleComputeShaderLibrary_AsyncExecution), 2512824870U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_MySimpleComputeShader_MySimpleComputeShader_h_3603496545(TEXT("/Script/ComputeShaders"),
	Z_CompiledInDeferFile_FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_MySimpleComputeShader_MySimpleComputeShader_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Unreal_Projects_ModuleDev_5_5_Plugins_ShaderLib_Source_ComputeShaders_Public_MySimpleComputeShader_MySimpleComputeShader_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
