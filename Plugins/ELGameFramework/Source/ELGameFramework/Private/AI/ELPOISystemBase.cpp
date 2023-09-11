// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/ELPOISystemBase.h"

//DEFINE_LOG_CATEGORY(LogELGameFrameworkAI);

AELPOISystemBase::AELPOISystemBase()
	:Super()
{
	bOnlyRelevantToOwner = true;

	name = "ELPOISystem";
	acceptableActorClass.Add(AELSmartObjectBase::StaticClass());
}

AELSmartObjectBase* AELPOISystemBase::FindSmartObject(AActor* actor)
{
	checkf(actor != nullptr, TEXT("[%s][FindSmartObject] actor is null"), *this->GetName());

	UELSmartObjectHelperComponent* soHelper = GetSmartObjectHelper(actor);

	if (soHelper->currentSmartObject)
	{
		// Actor is in another smart object, find failed.
		//UE_LOG(LogELGameFrameworkAI, Warning, TEXT("[%s] %s find smart object failed (already in other smart objects)."), *this->GetName(), *actor->GetName());
		return nullptr;
	}

	// get candidate smart objects sorted in priority
	TArray<AELSmartObjectBase*> candidateSOs;
	for (auto so : smartObjectsCanBeFound_)
	{
		if (so->CanActorJoin(actor))
		{
			candidateSOs.Add(so);
		}
	}
	ELSmartObjectSorter::GetSortedSmartObjects(actor, candidateSOs);

#if WITH_EDITOR
	UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] %s find smart object in priority: "), *this->GetName(), *actor->GetName());
	for (auto so : candidateSOs)
	{
		UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] \t%s"), *this->GetName(), *so->GetName());
	}
#endif

	// try to join in priority order
	for (auto so : candidateSOs)
	{
		if (JoinSmartObject(actor, so))
		{
			return so;
		}
	}

	return nullptr;
}

bool AELPOISystemBase::JoinSmartObject(AActor* actor, AELSmartObjectBase* smartObject)
{
	checkf(actor != nullptr, TEXT("[%s][JoinSmartObject] actor is null"), *this->GetName());
	checkf(smartObject != nullptr, TEXT("[%s][JoinSmartObject] smartObject is null"), *this->GetName());

	if (smartObjects_.Contains(smartObject) == false)
	{
		UE_LOG(LogELGameFrameworkAI, Warning, TEXT("[%s] %s join smart object failed (%s is not in system or world)."), *this->GetName(), *actor->GetName(), *smartObject->GetName());
		return false;
	}

	UELSmartObjectHelperComponent* soHelper = GetSmartObjectHelper(actor);

	// leave current but not broadcast PreFinish to resume
	if (soHelper->currentSmartObject)
	{
		soHelper->currentSmartObject->DoFinishSmartObject(actor);
		soHelper->currentSmartObject->PostFinishSmartObject(actor);
		soHelper->currentSmartObject = nullptr;
	}

	// set current & join
	soHelper->currentSmartObject = smartObject;
	smartObject->AddActor(actor);
	if (!smartObject->ActorIsInSystem(actor))
	{
		// leave on join or can join test fail
		//UE_LOG(LogELGameFrameworkAI, Warning, TEXT("[%s] %s join %s failed (leave on join or can join return false)."), *this->GetName(), *actor->GetName(), *smartObject->GetName());
		soHelper->currentSmartObject = nullptr;
		return false;
	}

	return true;
}

bool AELPOISystemBase::RequestSmartObject(AActor* actor, AELSmartObjectBase* smartObject, bool canInterruptCheck, bool priorityCheck, EELSmartObjectInterruptHandleRequestByOther requestInterruptHandle)
{
	checkf(actor != nullptr, TEXT("[%s][RequestSmartObject] actor is null"), *this->GetName());
	checkf(smartObject != nullptr, TEXT("[%s][RequestSmartObject] smartObject is null"), *this->GetName());

	if (smartObjects_.Contains(smartObject) == false)
	{
		UE_LOG(LogELGameFrameworkAI, Warning, TEXT("[%s] %s request smart object failed (%s is not in system or world)."), *this->GetName(), *actor->GetName(), *smartObject->GetName());
		return false;
	}

	UELSmartObjectHelperComponent* soHelper = GetSmartObjectHelper(actor);
	AELSmartObjectBase *currentSmartObject = soHelper->currentSmartObject;

	if (currentSmartObject)
	{
		if (currentSmartObject == smartObject)
		{
			// the same, already in
			//UE_LOG(LogELGameFrameworkAI, Warning, TEXT("[%s] %s is already in %s and request failed."), *this->GetName(), *actor->GetName(), *smartObject->GetName());
			return false;
		}

		if (smartObject->CanActorJoin(actor) == false)
		{
			// can not join
			UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] %s request %s failed (CanActorJoin return false)."), *this->GetName(), *actor->GetName(), *smartObject->GetName());
			return false;
		}

		if (canInterruptCheck)
		{
			// check can interrupt
			if (currentSmartObject->CanActorInterrupted(actor) == false)
			{
				// can not interrupted
				UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] %s request %s failed (CanActorInterrupted return false)."), *this->GetName(), *actor->GetName(), *smartObject->GetName());
				return false;
			}
		}

		if (priorityCheck)
		{
			// check priority
			if (smartObject->GetPriority(actor) < currentSmartObject->GetPriority(actor))
			{
				// priority less than current
				UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] %s request %s failed (priority is less than current smart object %s)."), *this->GetName(), *actor->GetName(), *smartObject->GetName(), *currentSmartObject->GetName());
				return false;
			}
		}

		// pause or leave current
		if (requestInterruptHandle == EELSmartObjectInterruptHandleRequestByOther::VE_Pause && currentSmartObject->GetInterruptHandle() <= EELSmartObjectInterruptHandleSelfSetting::VE_Pause)
		{
			soHelper->SetCurrentSmartObjectToPause();
			currentSmartObject->PauseActor(actor);
		}
		//else	// leave will handle in Join
		//{
		//	currentSmartObject->DoFinishSmartObject(actor);
		//	currentSmartObject->PostFinishSmartObject(actor);
		//	currentSmartObject = nullptr;
		//}
	}

	return JoinSmartObject(actor, smartObject);
}

AELSmartObjectBase* AELPOISystemBase::RequestSmartObjectByClass(AActor* actor, TSubclassOf<AELSmartObjectBase> smartObjectClass, bool canInterruptCheck, bool priorityCheck, EELSmartObjectInterruptHandleRequestByOther requestInterruptHandle)
{
	checkf(actor != nullptr, TEXT("[%s][RequestSmartObjectByClass] actor is null"), *this->GetName());

	UELSmartObjectHelperComponent* soHelper = GetSmartObjectHelper(actor);

	if (soHelper->currentSmartObject && soHelper->currentSmartObject->IsA(smartObjectClass))
	{
		// already in
		//UE_LOG(LogELGameFrameworkAI, Warning, TEXT("[%s] %s is already in %s and is the requested class."), *this->GetName(), *actor->GetName(), *soHelper->currentSmartObject->GetName());
		return soHelper->currentSmartObject;
	}

	for (auto so : smartObjects_)
	{
		if (so->IsA(smartObjectClass))
		{
			if (RequestSmartObject(actor, so, canInterruptCheck, priorityCheck, requestInterruptHandle) == true)
			{
				return so;
			}
		}
	}

	return nullptr;
}

void AELPOISystemBase::ForceLeaveAllSmartObjects(AActor* actor)
{
	checkf(actor != nullptr, TEXT("[%s][ForceLeaveAllSmartObjects] actor is null"), *this->GetName());

	UELSmartObjectHelperComponent* soHelper = GetSmartObjectHelper(actor);

	if (soHelper->currentSmartObject)
	{
		soHelper->currentSmartObject->DoFinishSmartObject(actor);
		soHelper->currentSmartObject->PostFinishSmartObject(actor);
		soHelper->currentSmartObject = nullptr;
	}

	auto pauseSO = soHelper->GetRecentlyPausedSmartObject();
	while (pauseSO)
	{
		pauseSO->DoFinishSmartObject(actor);
		pauseSO->PostFinishSmartObject(actor);
		pauseSO = soHelper->GetRecentlyPausedSmartObject();
	}
}

void AELPOISystemBase::OnFinishSmartObject(AActor* actor, AELSmartObjectBase* smartObject)
{
	UELSmartObjectHelperComponent* soHelper = GetSmartObjectHelper(actor);

	if (soHelper->pauseSmartObjects.Contains(smartObject))
	{
		soHelper->pauseSmartObjects.Remove(smartObject);
		UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] %s finish %s in pause."), *this->GetName(), *actor->GetName(), *smartObject->GetName());
		return;
	}

	if (soHelper->currentSmartObject == nullptr)
	{
		UE_LOG(LogELGameFrameworkAI, Warning, TEXT("[%s] %s finish %s, but is not current smart object."), *this->GetName(), *actor->GetName(), *smartObject->GetName());
	}

	soHelper->currentSmartObject = nullptr;
	auto recentlyPausedSO = soHelper->GetRecentlyPausedSmartObject();
	if (recentlyPausedSO)
	{
		soHelper->currentSmartObject = recentlyPausedSO;
		recentlyPausedSO->ResumeActor(actor);
	}
}

void AELPOISystemBase::DoAddActor(AActor* actor)
{
	Super::DoAddActor(actor);
	AELSmartObjectBase* smartObject = Cast<AELSmartObjectBase>(actor);
	check(smartObject);
	smartObjects_.Add(smartObject);
	smartObject->OnSmartObjectPreFinished.AddDynamic(this, &AELPOISystemBase::OnFinishSmartObject);
	if (smartObject->GetCanBeFound()) smartObjectsCanBeFound_.Add(smartObject);
}

void AELPOISystemBase::DoRemoveActor(AActor* actor)
{
	Super::DoRemoveActor(actor);
	AELSmartObjectBase* smartObject = Cast<AELSmartObjectBase>(actor);
	check(smartObject);
	smartObjects_.Remove(smartObject);
	smartObject->OnSmartObjectPreFinished.RemoveDynamic(this, &AELPOISystemBase::OnFinishSmartObject);
	if (smartObject->GetCanBeFound()) smartObjectsCanBeFound_.Remove(smartObject);
}

UELSmartObjectHelperComponent* AELPOISystemBase::GetSmartObjectHelper(AActor* actor)
{
	auto soHelperComp = actor->GetComponentByClass(UELSmartObjectHelperComponent::StaticClass());
	UELSmartObjectHelperComponent* soHelper = Cast<UELSmartObjectHelperComponent>(soHelperComp);
	if (soHelper == nullptr)
	{
		soHelper = NewObject<UELSmartObjectHelperComponent>(actor);
		soHelper->RegisterComponent();
	}
	return soHelper;
}

