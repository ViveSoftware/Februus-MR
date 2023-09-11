// Copyright HTC Corporation. All Rights Reserved.

#include "ViveCustomHandGestureComponent.h"
#include "ViveCustomHandGesture.h"

//DEFINE_LOG_CATEGORY(LogViveCustomHandGesture);


FCustomHandGestureNative UViveCustumGestureComponent::OnCustomHandGestureNative_Right;
FCustomHandGestureNative UViveCustumGestureComponent::OnCustomHandGestureNative_Left;

FDualGestureNative UViveCustumGestureComponent::OnCustomHandGestureNative_Dual;

// Sets default values for this component's properties
UViveCustumGestureComponent::UViveCustumGestureComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UViveCustumGestureComponent::BeginPlay()
{
	Super::BeginPlay();

	UViveCustumGestureComponent::OnCustomHandGestureNative_Right.AddDynamic(this, &UViveCustumGestureComponent::OnCustomHandGestureHandling_Right);
	UViveCustumGestureComponent::OnCustomHandGestureNative_Left.AddDynamic(this, &UViveCustumGestureComponent::OnCustomHandGestureHandling_Left);

	UViveCustumGestureComponent::OnCustomHandGestureNative_Dual.AddDynamic(this, &UViveCustumGestureComponent::OnDualGestureHandling);
}

void UViveCustumGestureComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
}

// Called every frame
void UViveCustumGestureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

#pragma region
void UViveCustumGestureComponent::OnCustomHandGestureHandling_Right(FString gesture)
{
	UE_LOG(LogViveCustomHandGesture, Log, TEXT("OnCustomHandGestureHandling_Right() gesture: %s"), *gesture);
	OnCustomHandGestureChanged_Right.Broadcast(gesture);
}

void UViveCustumGestureComponent::OnCustomHandGestureHandling_Left(FString gesture)
{
	UE_LOG(LogViveCustomHandGesture, Log, TEXT("OnCustomHandGestureHandling_Left() gesture: %s"), *gesture);
	OnCustomHandGestureChanged_Left.Broadcast(gesture);
}

void UViveCustumGestureComponent::OnDualGestureHandling(FString gesture)
{
	UE_LOG(LogViveCustomHandGesture, Log, TEXT("OnDualGestureHandling() gesture: %s"), *gesture);
	OnCustomHandGestureChange_DualHand.Broadcast(gesture);
}
#pragma endregion Custom Gesture

