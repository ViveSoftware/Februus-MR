// Copyright HTC Corporation. All Rights Reserved.

#pragma once
#include "IViveOpenXRWristTrackerModule.h"
#include "ViveOpenXRWristTrackerEnums.h"

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Containers/Ticker.h"
#include "IOpenXRExtensionPlugin.h"
#include "IInputDevice.h"

#include "OpenXRCore.h"

DECLARE_LOG_CATEGORY_EXTERN(LogViveOpenXRWristTracker, Log, All);

namespace Side
{
	const int RIGHT = 0;
	const int LEFT = 1;
	const int COUNT = 2;
}

class FViveOpenXRWristTracker : public IOpenXRExtensionPlugin, public IInputDevice
{
public:
	FViveOpenXRWristTracker(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler);
	virtual ~FViveOpenXRWristTracker();
	void Destroy();

	/************************************************************************/
	/* IOpenXRExtensionPlugin                                               */
	/************************************************************************/

	virtual FString GetDisplayName() override
	{
		return FString(TEXT("ViveOpenXRWristTracker"));
	}
	virtual bool GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
	virtual bool GetInteractionProfile(XrInstance InInstance, FString& OutKeyPrefix, XrPath& OutPath, bool& OutHasHaptics) override;
	virtual void AttachActionSets(TSet<XrActionSet>& OutActionSets) override;
	virtual void GetActiveActionSetsForSync(TArray<XrActiveActionSet>& OutActiveSets) override;
	virtual void PostCreateInstance(XrInstance InInstance) override;
	virtual const void* OnBeginSession(XrSession InSession, const void* InNext) override;
	virtual void OnDestroySession(XrSession InSession) override;
	virtual void PostSyncActions(XrSession InSession) override;
	virtual void UpdateDeviceLocations(XrSession InSession, XrTime DisplayTime, XrSpace TrackingSpace) override;

	static inline FViveOpenXRWristTracker* GetInstance() {
		return m_Instance;
	}

	bool m_bEnableWristTracker = false;
protected:
	static FViveOpenXRWristTracker* m_Instance;

private:
	class IXRTrackingSystem* XRTrackingSystem = nullptr;
	float WorldToMetersScale = 100;

	bool bSessionStarted = false;
	XrActionsSyncInfo SyncInfo{ XR_TYPE_ACTIONS_SYNC_INFO };
	static const int kActionCount = 6;

	XrInstance Instance = XR_NULL_HANDLE;
	XrActionSet WristTrackerActionSet = XR_NULL_HANDLE;
	// Pose
	XrAction LeftEntityPoseAction = XR_NULL_HANDLE, RightEntityPoseAction = XR_NULL_HANDLE;
	XrSpace LeftEntityPoseActionSpace = XR_NULL_HANDLE, RightEntityPoseActionSpace = XR_NULL_HANDLE;
	XrActionStatePose LeftEntityPoseActionState{ XR_TYPE_ACTION_STATE_POSE };
	XrActionStatePose RightEntityPoseActionState{ XR_TYPE_ACTION_STATE_POSE };
	// Left button
	XrAction LeftXClickAction = XR_NULL_HANDLE, LeftMenuClickAction = XR_NULL_HANDLE;
	// Right buttons
	XrAction RightAClickAction = XR_NULL_HANDLE, RightSystemClickAction = XR_NULL_HANDLE;

	TArray<XrPath> SubactionPathRight;
	TArray<XrPath> SubactionPathLeft;
	// Pose cached data
	XrSpaceLocation LeftEntityPoseSpaceLocation{ XR_TYPE_SPACE_LOCATION };
	XrSpaceLocation RightEntityPoseSpaceLocation{ XR_TYPE_SPACE_LOCATION };
	FTransform TrackerTransformLeft = FTransform::Identity;
	FTransform TrackerTransformRight = FTransform::Identity;
	// Left button cached data
	XrActionStateBoolean LeftXClickActionState{ XR_TYPE_ACTION_STATE_BOOLEAN };
	XrActionStateBoolean LeftMenuClickActionState{ XR_TYPE_ACTION_STATE_BOOLEAN };
	// Right button cached data
	XrActionStateBoolean RightAClickActionState{ XR_TYPE_ACTION_STATE_BOOLEAN };
	XrActionStateBoolean RightSystemClickActionState{ XR_TYPE_ACTION_STATE_BOOLEAN };

	uint64_t HmdOrigin = 0;

// Public functions for ViveOpenXRWristTrackerBP
public:
	uint64_t GetHeadOrigin() { return HmdOrigin; }
	FQuat GetTrackerRotation(bool isLeft)
	{
		auto rotation = isLeft ? TrackerTransformLeft.GetRotation() : TrackerTransformRight.GetRotation();
		return rotation;
	}
	FVector GetTrackerPosition(bool isLeft)
	{
		auto location = isLeft ? TrackerTransformLeft.GetLocation() : TrackerTransformRight.GetLocation();
		return location;
	}
	bool GetTrackerActive(bool isLeft)
	{
		return isLeft ? LeftEntityPoseActionState.isActive > 0 : RightEntityPoseActionState.isActive > 0;
	}
	bool GetTrackerValid(bool isLeft)
	{
		if (!GetTrackerActive(isLeft)) { return false; }

		auto locationFlags = isLeft ? LeftEntityPoseSpaceLocation.locationFlags : RightEntityPoseSpaceLocation.locationFlags;
		return locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT &&
			locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT;
	}
	bool GetTrackerTracked(bool isLeft)
	{
		if (!GetTrackerActive(isLeft)) { return false; }

		auto locationFlags = isLeft ? LeftEntityPoseSpaceLocation.locationFlags : RightEntityPoseSpaceLocation.locationFlags;
		return locationFlags & XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT &&
			locationFlags & XR_SPACE_LOCATION_POSITION_TRACKED_BIT;
	}
	bool GetTrackerKeyDown(bool isLeft, TrackerButton key)
	{
		switch (key)
		{
		case TrackerButton::Menu:
			return isLeft ? (LeftMenuClickActionState.currentState > 0) : (RightSystemClickActionState.currentState > 0);
		case TrackerButton::Primary:
			return isLeft ? (LeftXClickActionState.currentState > 0) : (RightAClickActionState.currentState > 0);
		default:
			break;
		}
		return false;
	}

public:

	/************************************************************************/
	/* IInputDevice                                                         */
	/************************************************************************/

	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override { return false; }
	virtual void SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) override
	{
		MessageHandler = InMessageHandler;
	}
	virtual void SendControllerEvents() override;

	virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override {}
	virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues& Values) override {}
	virtual void Tick(float DeltaTime) override {}
private:
	/** The recipient of motion controller input events */
	TSharedPtr< FGenericApplicationMessageHandler > MessageHandler;
	bool s_KeyStateLeftEx[kTrackerButtonCount] = { false, false };
	bool s_KeyStateRightEx[kTrackerButtonCount] = { false, false };
};

class FViveOpenXRWristTrackerModule : public IViveOpenXRWristTrackerModule
{
public:
	FViveOpenXRWristTrackerModule();
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual TSharedPtr<class IInputDevice> CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;

	FViveOpenXRWristTracker* GetWristTracker();

	
private:
	TSharedPtr<FViveOpenXRWristTracker> WristTracker;
};
