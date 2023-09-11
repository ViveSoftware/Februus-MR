// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"


//ELGameFramework Log
//DECLARE_LOG_CATEGORY_EXTERN(LogELGameFramework, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogELGameFramework, Log, All);
//DEFINE_LOG_CATEGORY_STATIC(LogELGameFrameworkAI, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogELGameFrameworkAI, Log, All);


class FELGameFrameworkModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
