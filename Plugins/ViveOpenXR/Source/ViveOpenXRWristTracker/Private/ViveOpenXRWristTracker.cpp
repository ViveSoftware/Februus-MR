// Copyright HTC Corporation. All Rights Reserved.

#include "ViveOpenXRWristTracker.h"
#include "IXRTrackingSystem.h"
#include "OpenXRCore.h"
#include "UObject/UObjectIterator.h"
#include "IOpenXRExtensionPlugin.h"
#include "Modules/ModuleManager.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Misc/ConfigCacheIni.h"

#if WITH_EDITOR
#include "Editor/EditorEngine.h"
#include "Editor.h"
#endif

DEFINE_LOG_CATEGORY(LogViveOpenXRWristTracker);

static TAutoConsoleVariable<int32> CVarEnableOpenXRWristTrackerDebug(TEXT("OpenXR.debug.EnableWristTrackerDebug"), 1, TEXT("0 - WristTracker debug visualizations are disabled. 1 - WristTracker debug visualizations are enabled."));

/************************************************************************/
/* FViveOpenXRWristTracker                                              */
/************************************************************************/

FViveOpenXRWristTracker* FViveOpenXRWristTracker::m_Instance = nullptr;

FViveOpenXRWristTracker::FViveOpenXRWristTracker(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
	: MessageHandler(InMessageHandler)
{
	RegisterOpenXRExtensionModularFeature();

	m_Instance = this;
	UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("FViveOpenXRWristTracker() register extension feature WristTracker %p."), m_Instance);
}

FViveOpenXRWristTracker::~FViveOpenXRWristTracker()
{
	Destroy();
}

void FViveOpenXRWristTracker::Destroy()
{
	UnregisterOpenXRExtensionModularFeature();
}

bool FViveOpenXRWristTracker::GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions)
{
	if (m_bEnableWristTracker)
	{
		UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("GetRequiredExtensions() XR_HTC_vive_wrist_tracker_interaction."));
		OutExtensions.Add("XR_HTC_vive_wrist_tracker_interaction");
	}
	return true;
}

bool FViveOpenXRWristTracker::GetInteractionProfile(XrInstance InInstance, FString& OutKeyPrefix, XrPath& OutPath, bool& OutHasHaptics)
{
	if (!m_bEnableWristTracker) return false; 
	UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("GetInteractionProfile() /interaction_profiles/htc/vive_wrist_tracker."));
	OutKeyPrefix = "WristTracker";
	OutHasHaptics = false;
	return xrStringToPath(InInstance, "/interaction_profiles/htc/vive_wrist_tracker", &OutPath) == XR_SUCCESS;
}

void FViveOpenXRWristTracker::AttachActionSets(TSet<XrActionSet>& OutActionSets)
{
	if (!m_bEnableWristTracker) return;

	check(Instance != XR_NULL_HANDLE);

	XrPath RightHand = XR_NULL_PATH;
	XR_ENSURE(xrStringToPath(Instance, "/user/wrist_htc/right", &RightHand));

	XrPath LeftHand = XR_NULL_PATH;
	XR_ENSURE(xrStringToPath(Instance, "/user/wrist_htc/left", &LeftHand));

	SubactionPathRight.RemoveAll([](const int& num) {
		return true;
		});
	SubactionPathLeft.RemoveAll([](const int& num) {
		return true;
		});
	SubactionPathRight.Emplace(RightHand);
	SubactionPathLeft.Emplace(LeftHand);

	//Create ActionSet
	if (WristTrackerActionSet != XR_NULL_HANDLE)
	{
		xrDestroyActionSet(WristTrackerActionSet);
		WristTrackerActionSet = XR_NULL_HANDLE;
	}
	{
		XrActionSetCreateInfo Info;
		Info.type = XR_TYPE_ACTION_SET_CREATE_INFO;
		Info.next = nullptr;
		FCStringAnsi::Strcpy(Info.actionSetName, XR_MAX_ACTION_SET_NAME_SIZE, "viveopenxrwristtrackeractionset");
		FCStringAnsi::Strcpy(Info.localizedActionSetName, XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE, "VIVE OpenXR Wrist Tracker Action Set");
		Info.priority = 0;
		XR_ENSURE(xrCreateActionSet(Instance, &Info, &WristTrackerActionSet));
	}
	//Create Actions lambda
	auto AddAction = [&](XrActionSet& InActionSet, XrAction& OutAction, XrActionType InActionType, const FName& InName, const TArray<XrPath>& InSubactionPaths)
	{
		check(InActionSet != XR_NULL_HANDLE);
		if(OutAction != XR_NULL_HANDLE) {
			xrDestroyAction(OutAction);
			OutAction = XR_NULL_HANDLE;
		}

		char ActionName[NAME_SIZE];
		InName.GetPlainANSIString(ActionName);

		XrActionCreateInfo Info;
		Info.type = XR_TYPE_ACTION_CREATE_INFO;
		Info.next = nullptr;
		Info.actionType = InActionType;
		Info.countSubactionPaths = InSubactionPaths.Num();
		Info.subactionPaths = InSubactionPaths.GetData();
		FCStringAnsi::Strcpy(Info.actionName, XR_MAX_ACTION_NAME_SIZE, ActionName);
		FCStringAnsi::Strcpy(Info.localizedActionName, XR_MAX_LOCALIZED_ACTION_NAME_SIZE, ActionName);
		XR_ENSURE(xrCreateAction(InActionSet, &Info, &OutAction));
	};

	// Create suggested bindings
	XrPath WristTrackerProfilePath = XR_NULL_PATH;
	XR_ENSURE(xrStringToPath(Instance, "/interaction_profiles/htc/vive_wrist_tracker", &WristTrackerProfilePath));

	//TArray<XrPath> subactionPaths;
	XrActionSuggestedBinding bindings[kActionCount];

	// 0. left entity pose
	int action_index = 0;
	if (action_index < kActionCount)
	{
		UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("AttachActionSets() %d. /user/wrist_htc/left/input/entity_htc/pose."), action_index);
		AddAction(WristTrackerActionSet, LeftEntityPoseAction, XR_ACTION_TYPE_POSE_INPUT, "left_entity_pose", SubactionPathLeft);
		if (LeftEntityPoseAction == XR_NULL_HANDLE) { return; }
		XrPath LeftEntityPosePath = XR_NULL_PATH;
		XR_ENSURE(xrStringToPath(Instance, "/user/wrist_htc/left/input/entity_htc/pose", &LeftEntityPosePath));
		bindings[action_index].action = LeftEntityPoseAction;
		bindings[action_index].binding = LeftEntityPosePath;
	}

	// 1. right entity pose
	action_index++;
	if (action_index < kActionCount)
	{
		UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("AttachActionSets() %d. /user/wrist_htc/right/input/entity_htc/pose."), action_index);
		AddAction(WristTrackerActionSet, RightEntityPoseAction, XR_ACTION_TYPE_POSE_INPUT, "right_entity_pose", SubactionPathRight);
		if (RightEntityPoseAction == XR_NULL_HANDLE) { return; }
		XrPath RightEntityPosePath = XR_NULL_PATH;
		XR_ENSURE(xrStringToPath(Instance, "/user/wrist_htc/right/input/entity_htc/pose", &RightEntityPosePath));
		bindings[action_index].action = RightEntityPoseAction;
		bindings[action_index].binding = RightEntityPosePath;
	}

	// 2. left button x click
	action_index++;
	if (action_index < kActionCount)
	{
		UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("AttachActionSets() %d. /user/wrist_htc/left/input/x/click."), action_index);
		AddAction(WristTrackerActionSet, LeftXClickAction, XR_ACTION_TYPE_BOOLEAN_INPUT, "left_x_click", SubactionPathLeft);
		if (LeftXClickAction == XR_NULL_HANDLE) { return; }
		XrPath LeftXClickPath = XR_NULL_PATH;
		XR_ENSURE(xrStringToPath(Instance, "/user/wrist_htc/left/input/x/click", &LeftXClickPath));
		bindings[action_index].action = LeftXClickAction;
		bindings[action_index].binding = LeftXClickPath;
	}

	// 3. left button menu click
	action_index++;
	if (action_index < kActionCount)
	{
		UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("AttachActionSets() %d. /user/wrist_htc/left/input/menu/click."), action_index);
		AddAction(WristTrackerActionSet, LeftMenuClickAction, XR_ACTION_TYPE_BOOLEAN_INPUT, "left_menu_click", SubactionPathLeft);
		if (LeftMenuClickAction == XR_NULL_HANDLE) { return; }
		XrPath LeftMenuClickPath = XR_NULL_PATH;
		XR_ENSURE(xrStringToPath(Instance, "/user/wrist_htc/left/input/menu/click", &LeftMenuClickPath));
		bindings[action_index].action = LeftMenuClickAction;
		bindings[action_index].binding = LeftMenuClickPath;
	}

	// 4. right button a click
	action_index++;
	if (action_index < kActionCount)
	{
		UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("AttachActionSets() %d. /user/wrist_htc/right/input/a/click."), action_index);
		AddAction(WristTrackerActionSet, RightAClickAction, XR_ACTION_TYPE_BOOLEAN_INPUT, "right_a_click", SubactionPathRight);
		if (RightAClickAction == XR_NULL_HANDLE) { return; }
		XrPath RightAClickPath = XR_NULL_PATH;
		XR_ENSURE(xrStringToPath(Instance, "/user/wrist_htc/right/input/a/click", &RightAClickPath));
		bindings[action_index].action = RightAClickAction;
		bindings[action_index].binding = RightAClickPath;
	}

	// 5. right button system click
	action_index++;
	if (action_index < kActionCount)
	{
		UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("AttachActionSets() %d. /user/wrist_htc/right/input/system/click."), action_index);
		AddAction(WristTrackerActionSet, RightSystemClickAction, XR_ACTION_TYPE_BOOLEAN_INPUT, "right_system_click", SubactionPathRight);
		if (RightSystemClickAction == XR_NULL_HANDLE) { return; }
		XrPath RightSystemClickPath = XR_NULL_PATH;
		XR_ENSURE(xrStringToPath(Instance, "/user/wrist_htc/right/input/system/click", &RightSystemClickPath));
		bindings[action_index].action = RightSystemClickAction;
		bindings[action_index].binding = RightSystemClickPath;
	}

	XrInteractionProfileSuggestedBinding SuggestedBindings{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
	SuggestedBindings.interactionProfile = WristTrackerProfilePath;
	SuggestedBindings.suggestedBindings = bindings;
	SuggestedBindings.countSuggestedBindings = kActionCount;
	XR_ENSURE(xrSuggestInteractionProfileBindings(Instance, &SuggestedBindings));

	OutActionSets.Add(WristTrackerActionSet);
}

void FViveOpenXRWristTracker::GetActiveActionSetsForSync(TArray<XrActiveActionSet>& OutActiveSets)
{
	if (!m_bEnableWristTracker) return;

	check(WristTrackerActionSet != XR_NULL_HANDLE);
	OutActiveSets.Add(XrActiveActionSet{WristTrackerActionSet, XR_NULL_PATH});
}

void FViveOpenXRWristTracker::PostCreateInstance(XrInstance InInstance)
{
	Instance = InInstance;
}

void FViveOpenXRWristTracker::OnDestroySession(XrSession InSession)
{
	if (LeftEntityPoseActionSpace)
	{
		XR_ENSURE(xrDestroySpace(LeftEntityPoseActionSpace));
		LeftEntityPoseActionSpace = XR_NULL_HANDLE;
	}
	if (RightEntityPoseActionSpace)
	{
		XR_ENSURE(xrDestroySpace(RightEntityPoseActionSpace));
		RightEntityPoseActionSpace = XR_NULL_HANDLE;
	}
}

const void* FViveOpenXRWristTracker::OnBeginSession(XrSession InSession, const void* InNext)
{
	if (m_bEnableWristTracker)
	{
		static FName SystemName(TEXT("OpenXR"));
		if (GEngine->XRSystem.IsValid() && (GEngine->XRSystem->GetSystemName() == SystemName))
		{
			XRTrackingSystem = GEngine->XRSystem.Get();
		}

		// 0. left entity pose, only XR_ACTION_TYPE_POSE_INPUT needs the ActionSpace.
		UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("OnBeginSession() xrCreateActionSpace /user/wrist_htc/left/input/entity_htc/pose."));
		XrActionSpaceCreateInfo LeftEntityPoseActionSpaceInfo{ XR_TYPE_ACTION_SPACE_CREATE_INFO };
		LeftEntityPoseActionSpaceInfo.action = LeftEntityPoseAction;
		LeftEntityPoseActionSpaceInfo.poseInActionSpace = ToXrPose(FTransform::Identity);
		XR_ENSURE(xrCreateActionSpace(InSession, &LeftEntityPoseActionSpaceInfo, &LeftEntityPoseActionSpace));

		// 1. right entity pose, only XR_ACTION_TYPE_POSE_INPUT needs the ActionSpace.
		UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("OnBeginSession() xrCreateActionSpace /user/wrist_htc/right/input/entity_htc/pose."));
		XrActionSpaceCreateInfo RightEntityPoseActionSpaceInfo{ XR_TYPE_ACTION_SPACE_CREATE_INFO };
		RightEntityPoseActionSpaceInfo.action = RightEntityPoseAction;
		RightEntityPoseActionSpaceInfo.poseInActionSpace = ToXrPose(FTransform::Identity);
		XR_ENSURE(xrCreateActionSpace(InSession, &RightEntityPoseActionSpaceInfo, &RightEntityPoseActionSpace));

		SyncInfo.countActiveActionSets = 0;
		SyncInfo.activeActionSets = XR_NULL_HANDLE;

		bSessionStarted = true;
	}

	return InNext;
}

void FViveOpenXRWristTracker::PostSyncActions(XrSession InSession)
{
	if (!m_bEnableWristTracker) return;

	XRTrackingSystem->GetWorldToMetersScale();

	// left entity pose
	XrActionStateGetInfo LeftEntityPoseActionStateInfo{ XR_TYPE_ACTION_STATE_GET_INFO };
	LeftEntityPoseActionStateInfo.action = LeftEntityPoseAction;
	//LeftEntityPoseActionStateInfo.subactionPath = SubactionPathLeft[0];
	XR_ENSURE(xrGetActionStatePose(InSession, &LeftEntityPoseActionStateInfo, &LeftEntityPoseActionState));

	// right entity pose
	XrActionStateGetInfo RightEntityPoseActionStateInfo{ XR_TYPE_ACTION_STATE_GET_INFO };
	RightEntityPoseActionStateInfo.action = RightEntityPoseAction;
	//RightEntityPoseActionStateInfo.subactionPath = SubactionPathRight[0];
	XR_ENSURE(xrGetActionStatePose(InSession, &RightEntityPoseActionStateInfo, &RightEntityPoseActionState));

	// left button x click
	XrActionStateGetInfo LeftXClickActionStateInfo{ XR_TYPE_ACTION_STATE_GET_INFO };
	LeftXClickActionStateInfo.action = LeftXClickAction;
	//LeftXClickActionStateInfo.subactionPath = SubactionPathLeft[0];
	XR_ENSURE(xrGetActionStateBoolean(InSession, &LeftXClickActionStateInfo, &LeftXClickActionState));
	
	// left button menu click
	XrActionStateGetInfo LeftMenuActionStateInfo{ XR_TYPE_ACTION_STATE_GET_INFO };
	LeftMenuActionStateInfo.action = LeftMenuClickAction;
	//LeftMenuActionStateInfo.subactionPath = SubactionPathLeft[0];
	XR_ENSURE(xrGetActionStateBoolean(InSession, &LeftMenuActionStateInfo, &LeftMenuClickActionState));

	// right button a click
	XrActionStateGetInfo RightAActionStateInfo{ XR_TYPE_ACTION_STATE_GET_INFO };
	RightAActionStateInfo.action = RightAClickAction;
	RightAActionStateInfo.subactionPath = SubactionPathRight[0];
	XR_ENSURE(xrGetActionStateBoolean(InSession, &RightAActionStateInfo, &RightAClickActionState));
	
	// right button system click
	XrActionStateGetInfo RightSystemActionStateInfo{ XR_TYPE_ACTION_STATE_GET_INFO };
	RightSystemActionStateInfo.action = RightSystemClickAction;
	//RightSystemActionStateInfo.subactionPath = SubactionPathRight[0];
	XR_ENSURE(xrGetActionStateBoolean(InSession, &RightSystemActionStateInfo, &RightSystemClickActionState));
}

void FViveOpenXRWristTracker::UpdateDeviceLocations(XrSession InSession, XrTime DisplayTime, XrSpace TrackingSpace)
{
	if (!m_bEnableWristTracker) return;

	HmdOrigin = (uint64_t)TrackingSpace;
	WorldToMetersScale = XRTrackingSystem->GetWorldToMetersScale();

	if (GetTrackerActive(true)) {
		XR_ENSURE(xrLocateSpace(LeftEntityPoseActionSpace, TrackingSpace, DisplayTime, &LeftEntityPoseSpaceLocation));
		if (GetTrackerTracked(true))
			TrackerTransformLeft = ToFTransform(LeftEntityPoseSpaceLocation.pose, WorldToMetersScale);
	}
	if (GetTrackerActive(false)) {
		XR_ENSURE(xrLocateSpace(RightEntityPoseActionSpace, TrackingSpace, DisplayTime, &RightEntityPoseSpaceLocation));
		if (GetTrackerTracked(false))
			TrackerTransformRight = ToFTransform(RightEntityPoseSpaceLocation.pose, WorldToMetersScale);
	}
}

namespace TrackerKeys
{
	/// ---- Left hand only ----
	// "/input/menu/click"
	const FKey Tracker_Left_Menu_Click("Tracker_Left_Menu_Click");
	// "/input/x/click"
	const FKey Tracker_Left_X_Click("Tracker_Left_X_Click");

	/// ---- Right hand only ----
	// "/input/system/click"
	const FKey Tracker_Right_System_Click("Tracker_Right_System_Click");
	// "/input/a/click"
	const FKey Tracker_Right_A_Click("Tracker_Right_A_Click");
}

#pragma region
void FViveOpenXRWristTracker::SendControllerEvents()
{
	if (!m_bEnableWristTracker) return;

	bool s_KeyStateLeft[kTrackerButtonCount] = { false, false };
	bool s_KeyStateRight[kTrackerButtonCount] = { false, false };

	uint8_t menu_id = (uint8_t)TrackerButton::Menu;
	uint8_t primary_id = (uint8_t)TrackerButton::Primary;

	IPlatformInputDeviceMapper& DeviceMapper = IPlatformInputDeviceMapper::Get();

	/// Left tracker keys
	// "/user/wrist_htc/left/input/menu/click"
	s_KeyStateLeft[menu_id] = GetTrackerKeyDown(true, TrackerButton::Menu);
	if (s_KeyStateLeftEx[menu_id] != s_KeyStateLeft[menu_id])
	{
		s_KeyStateLeftEx[menu_id] = s_KeyStateLeft[menu_id];

		FName keyName = TrackerKeys::Tracker_Left_Menu_Click.GetFName();
		UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("SendControllerEvents() %s = %d."),
			*keyName.ToString(), (uint8_t)s_KeyStateLeft[menu_id]);

		if (s_KeyStateLeftEx[menu_id])
			MessageHandler->OnControllerButtonPressed(keyName, DeviceMapper.GetPrimaryPlatformUser(), DeviceMapper.GetDefaultInputDevice(), false);
		else
			MessageHandler->OnControllerButtonReleased(keyName, DeviceMapper.GetPrimaryPlatformUser(), DeviceMapper.GetDefaultInputDevice(), false);
	}

	// "/user/wrist_htc/left/input/x/click"
	s_KeyStateLeft[primary_id] = GetTrackerKeyDown(true, TrackerButton::Primary);
	if (s_KeyStateLeftEx[primary_id] != s_KeyStateLeft[primary_id])
	{
		s_KeyStateLeftEx[primary_id] = s_KeyStateLeft[primary_id];

		FName keyName = TrackerKeys::Tracker_Left_X_Click.GetFName();
		UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("SendControllerEvents() %s = %d."),
			*keyName.ToString(), (uint8_t)s_KeyStateLeft[primary_id]);

		if (s_KeyStateLeftEx[primary_id])
			MessageHandler->OnControllerButtonPressed(keyName, DeviceMapper.GetPrimaryPlatformUser(), DeviceMapper.GetDefaultInputDevice(), false);
		else
			MessageHandler->OnControllerButtonReleased(keyName, DeviceMapper.GetPrimaryPlatformUser(), DeviceMapper.GetDefaultInputDevice(), false);
	}

	/// Right tracker keys
	// "/user/wrist_htc/right/input/system/click"
	s_KeyStateRight[menu_id] = GetTrackerKeyDown(false, TrackerButton::Menu);
	if (s_KeyStateRightEx[menu_id] != s_KeyStateRight[menu_id])
	{
		s_KeyStateRightEx[menu_id] = s_KeyStateRight[menu_id];

		FName keyName = TrackerKeys::Tracker_Right_System_Click.GetFName();
		UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("SendControllerEvents() %s = %d."),
			*keyName.ToString(), (uint8_t)s_KeyStateRight[menu_id]);

		if (s_KeyStateRightEx[menu_id])
			MessageHandler->OnControllerButtonPressed(keyName, DeviceMapper.GetPrimaryPlatformUser(), DeviceMapper.GetDefaultInputDevice(), false);
		else
			MessageHandler->OnControllerButtonReleased(keyName, DeviceMapper.GetPrimaryPlatformUser(), DeviceMapper.GetDefaultInputDevice(), false);
	}

	// "/user/wrist_htc/right/input/a/click"
	s_KeyStateRight[primary_id] = GetTrackerKeyDown(false, TrackerButton::Primary);
	if (s_KeyStateRightEx[primary_id] != s_KeyStateRight[primary_id])
	{
		s_KeyStateRightEx[primary_id] = s_KeyStateRight[primary_id];

		FName keyName = TrackerKeys::Tracker_Right_A_Click.GetFName();
		UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("SendControllerEvents() %s = %d."),
			*keyName.ToString(), (uint8_t)s_KeyStateRight[primary_id]);

		if (s_KeyStateRightEx[primary_id])
			MessageHandler->OnControllerButtonPressed(keyName, DeviceMapper.GetPrimaryPlatformUser(), DeviceMapper.GetDefaultInputDevice(), false);
		else
			MessageHandler->OnControllerButtonReleased(keyName, DeviceMapper.GetPrimaryPlatformUser(), DeviceMapper.GetDefaultInputDevice(), false);
	}
}
#pragma endregion IInputDevice overrides

/************************************************************************/
/* FViveOpenXRWristTrackerModule                                        */
/************************************************************************/

#define LOCTEXT_NAMESPACE "FViveOpenXRWristTrackerModule"

FName IViveOpenXRWristTrackerModule::ViveOpenXRWristTrackerModularKeyName = FName(TEXT("ViveOpenXRWristTracker"));

FViveOpenXRWristTrackerModule::FViveOpenXRWristTrackerModule()
{
}

void FViveOpenXRWristTrackerModule::StartupModule()
{
	UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("StartupModule()"));
	IViveOpenXRWristTrackerModule::StartupModule();

	TSharedPtr<FGenericApplicationMessageHandler> DummyMessageHandler(new FGenericApplicationMessageHandler());
	CreateInputDevice(DummyMessageHandler.ToSharedRef());

	EKeys::AddMenuCategoryDisplayInfo("Wrist Tracker", LOCTEXT("WristTrackerSubCategory", "HTC Wrist Tracker"), TEXT("GraphEditor.PadEvent_16x"));

	/// ---- Left hand only ----
	// "/input/menu/click"
	EKeys::AddKey(FKeyDetails(TrackerKeys::Tracker_Left_Menu_Click, LOCTEXT("Tracker_Left_Menu_Click", "Wrist Tracker (L) Menu"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Wrist Tracker"));
	// "/input/x/click"
	EKeys::AddKey(FKeyDetails(TrackerKeys::Tracker_Left_X_Click, LOCTEXT("Tracker_Left_X_Click", "Wrist Tracker (L) X Press"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Wrist Tracker"));

	/// ---- Right hand only ----
	// "/input/system/click"
	EKeys::AddKey(FKeyDetails(TrackerKeys::Tracker_Right_System_Click, LOCTEXT("Tracker_Right_System_Click", "Wrist Tracker (R) System"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Wrist Tracker"));
	// "/input/a/click"
	EKeys::AddKey(FKeyDetails(TrackerKeys::Tracker_Right_A_Click, LOCTEXT("Tracker_Right_A_Click", "Wrist Tracker (R) A Press"), FKeyDetails::GamepadKey | FKeyDetails::NotBlueprintBindableKey, "Wrist Tracker"));

	check(GConfig && GConfig->IsReadyForUse());
	FString modeName;
	if (GConfig->GetString(TEXT("/Script/ViveOpenXRRuntimeSettings.ViveOpenXRRuntimeSettings"), TEXT("bEnableWristTracker"), modeName, GEngineIni))
	{
		if (modeName.Equals("False"))
		{
			GetWristTracker()->m_bEnableWristTracker = false;
		}
		else if (modeName.Equals("True"))
		{
			GetWristTracker()->m_bEnableWristTracker = true;
		}
	}

	if (GetWristTracker()->m_bEnableWristTracker)
	{
		UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("Enable Wrist controller."));
	}
	else
	{
		UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("Disable Wrist controller."));
	}
}

void FViveOpenXRWristTrackerModule::ShutdownModule()
{
	UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("ShutdownModule()"));
}

TSharedPtr<class IInputDevice> FViveOpenXRWristTrackerModule::CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
	if (!WristTracker.IsValid())
	{
		auto InputDevice = new FViveOpenXRWristTracker(InMessageHandler);
		WristTracker = TSharedPtr<FViveOpenXRWristTracker>(InputDevice);
		UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("CreateInputDevice() new WristTracker %p"), WristTracker.Get());

		return WristTracker;
	}
	else
	{
		WristTracker.Get()->SetMessageHandler(InMessageHandler);
		UE_LOG(LogViveOpenXRWristTracker, Log, TEXT("CreateInputDevice() update WristTracker %p"), WristTracker.Get());

		return WristTracker;
	}
	return nullptr;
}

FViveOpenXRWristTracker* FViveOpenXRWristTrackerModule::GetWristTracker()
{
	return FViveOpenXRWristTracker::GetInstance();
}

IMPLEMENT_MODULE(FViveOpenXRWristTrackerModule, ViveOpenXRWristTracker);
