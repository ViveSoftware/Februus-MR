// Copyright HTC Corporation. All Rights Reserved.

#pragma once

#include "IOpenXRExtensionPlugin.h"

#include "Modules/ModuleInterface.h"
#include "OpenXRCore.h"

//DECLARE_LOG_CATEGORY_EXTERN(LogViveCustomHandGesture, Log, All);

class FViveCustomHandGestureModule : public IModuleInterface, public IOpenXRExtensionPlugin
{
	/************************************************************************/
	/* IOpenXRExtensionPlugin                                               */
	/************************************************************************/
	virtual FString GetDisplayName() override
	{
		return FString(TEXT("ViveCustomHandGesture"));
	}
};
