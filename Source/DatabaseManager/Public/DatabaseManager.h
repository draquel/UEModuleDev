#pragma once

#include "CoreMinimal.h"

class FDatabaseManagerModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};