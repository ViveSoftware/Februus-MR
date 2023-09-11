// Copyright HTC Corporation. All Rights Reserved.

#include "ViveOpenXRWristTrackerFunctionLibrary.h"
#include "ViveOpenXRWristTracker.h"

FViveOpenXRWristTracker* GetWristTracker()
{
	TArray<FViveOpenXRWristTrackerModule*> InputDeviceModules = IModularFeatures::Get().GetModularFeatureImplementations<FViveOpenXRWristTrackerModule>(IInputDeviceModule::GetModularFeatureName());
	for (auto InputDeviceModule : InputDeviceModules)
	{
		if (InputDeviceModule->GetModuleKeyName().IsEqual(IViveOpenXRWristTrackerModule::ViveOpenXRWristTrackerModularKeyName))
			return InputDeviceModule->GetWristTracker();
	}

	return nullptr;
}

void UViveOpenXRWristTrackerFunctionLibrary::GetWristTrackerLocationAndRotation(bool isRight, FVector& location, FRotator& rotation, bool& valid)
{
	location = FVector::ZeroVector;
	rotation = FRotator::ZeroRotator;
	valid = false;

	auto WristTracker = GetWristTracker();
	if (WristTracker == nullptr) return;
	if (WristTracker->GetTrackerValid(!isRight) && WristTracker->GetTrackerTracked(!isRight))
	{
		valid = true;
		location = WristTracker->GetTrackerPosition(!isRight);
		rotation = WristTracker->GetTrackerRotation(!isRight).Rotator();
	}
}

bool UViveOpenXRWristTrackerFunctionLibrary::IsWrsitTrackerKeyDown(bool isRight, TrackerButton key)
{
	auto WristTracker = GetWristTracker();
	return WristTracker->GetTrackerKeyDown(!isRight, key);;
}
