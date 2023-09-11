// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/ELScenarioComponentBase.h"
#include "Net/UnrealNetwork.h"

//static void GetLifetimeBlueprintActorComponentReplicationList(const UActorComponent* ThisActorComponent, const UBlueprintGeneratedClass* MyClass, TArray< FLifetimeProperty > & OutLifetimeProps)
//{
//	if (MyClass == NULL)
//	{
//		return;
//	}
//	uint32 PropertiesLeft = MyClass->NumReplicatedProperties;
//
//	for (TFieldIterator<UProperty> It(MyClass, EFieldIteratorFlags::ExcludeSuper); It && PropertiesLeft > 0; ++It)
//	{
//		UProperty * Prop = *It;
//		if (Prop != NULL && Prop->GetPropertyFlags() & CPF_Net)
//		{
//			PropertiesLeft--;
//			OutLifetimeProps.Add(FLifetimeProperty(Prop->RepIndex));
//		}
//	}
//
//	return GetLifetimeBlueprintActorComponentReplicationList(ThisActorComponent, Cast< UBlueprintGeneratedClass >(MyClass->GetSuperStruct()), OutLifetimeProps);
//}

// Sets default values for this component's properties
UELScenarioComponentBase::UELScenarioComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
	// ...
}

void UELScenarioComponentBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UELScenarioComponentBase, isPausing_);
	DOREPLIFETIME(UELScenarioComponentBase, isRunning_);
	//GetLifetimeBlueprintActorComponentReplicationList(this, Cast< UBlueprintGeneratedClass >(GetClass()), OutLifetimeProps);
}

bool UELScenarioComponentBase::Setup_Implementation(UELScenarioComponentBase* referenceScenario)
{
	return false;
}

bool UELScenarioComponentBase::Start_Implementation()
{
	return false;
}

bool UELScenarioComponentBase::Pause_Implementation()
{
	return false;
}

bool UELScenarioComponentBase::Resume_Implementation()
{
	return false;
}

bool UELScenarioComponentBase::Stop_Implementation()
{
	return false;
}

void UELScenarioComponentBase::FinishScenario()
{
	if (isRunning_ == true && isPreStop_ == false)
	{
		OnScenarioPreFinished.Broadcast(this->GetOwner(), this);
		OnScenarioPreFinished.Clear();
		OnScenarioFinished.Broadcast(this->GetOwner(), this);
		OnScenarioFinished.Clear();
		isRunning_ = false;
		isPausing_ = false;
	}
}

void UELScenarioComponentBase::PostStart()
{
	isRunning_ = true;
	isPreStop_ = false;
}

void UELScenarioComponentBase::PreStop()
{
	isPreStop_ = true;
}

void UELScenarioComponentBase::PostStop()
{
	isRunning_ = false;
	isPausing_ = false;
	isPreStop_ = false;
}

void UELScenarioComponentBase::PostPause()
{
	isPausing_ = true;
}

void UELScenarioComponentBase::PostResume()
{
	isPausing_ = false;
}

bool UELScenarioComponentBase::IsPause()
{
	return isPausing_;
}

bool UELScenarioComponentBase::IsRunning()
{
	return isRunning_;
}

FString UELScenarioComponentBase::GetScenarioName_Implementation()
{
	return (name.IsEmpty() ? this->GetName() : name);
}


