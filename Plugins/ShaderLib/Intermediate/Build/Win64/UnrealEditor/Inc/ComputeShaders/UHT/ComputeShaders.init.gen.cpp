// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeComputeShaders_init() {}
	COMPUTESHADERS_API UFunction* Z_Construct_UDelegateFunction_ComputeShaders_OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted__DelegateSignature();
	static FPackageRegistrationInfo Z_Registration_Info_UPackage__Script_ComputeShaders;
	FORCENOINLINE UPackage* Z_Construct_UPackage__Script_ComputeShaders()
	{
		if (!Z_Registration_Info_UPackage__Script_ComputeShaders.OuterSingleton)
		{
			static UObject* (*const SingletonFuncArray[])() = {
				(UObject* (*)())Z_Construct_UDelegateFunction_ComputeShaders_OnMySimpleComputeShaderLibrary_AsyncExecutionCompleted__DelegateSignature,
			};
			static const UECodeGen_Private::FPackageParams PackageParams = {
				"/Script/ComputeShaders",
				SingletonFuncArray,
				UE_ARRAY_COUNT(SingletonFuncArray),
				PKG_CompiledIn | 0x00000000,
				0x5BE4233A,
				0xD1BF8B41,
				METADATA_PARAMS(0, nullptr)
			};
			UECodeGen_Private::ConstructUPackage(Z_Registration_Info_UPackage__Script_ComputeShaders.OuterSingleton, PackageParams);
		}
		return Z_Registration_Info_UPackage__Script_ComputeShaders.OuterSingleton;
	}
	static FRegisterCompiledInInfo Z_CompiledInDeferPackage_UPackage__Script_ComputeShaders(Z_Construct_UPackage__Script_ComputeShaders, TEXT("/Script/ComputeShaders"), Z_Registration_Info_UPackage__Script_ComputeShaders, CONSTRUCT_RELOAD_VERSION_INFO(FPackageReloadVersionInfo, 0x5BE4233A, 0xD1BF8B41));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
