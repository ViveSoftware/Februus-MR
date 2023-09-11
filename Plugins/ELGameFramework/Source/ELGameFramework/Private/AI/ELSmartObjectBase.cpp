// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/ELSmartObjectBase.h"
#include "AI/ELScenarioComponentBase.h"

//DEFINE_LOG_CATEGORY_STATIC(LogELGameFrameworkAI, Log, All);
DEFINE_LOG_CATEGORY(LogELGameFrameworkAI);

AELSmartObjectBase::AELSmartObjectBase()
	:Super()
{
	rangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RangeSphere"));
	//rangeSphere->RelativeLocation = FVector::ZeroVector;
	//rangeSphere->RelativeRotation = FRotator::ZeroRotator;
	//rangeSphere->RelativeScale3D = FVector::OneVector;
	rangeSphere->SetSphereRadius(100);
	//rangeSphere->bGenerateOverlapEvents = true;
	rangeSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//rangeSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	RootComponent = rangeSphere;
}

void AELSmartObjectBase::DoFinishSmartObject(AActor* actor)
{
	if (actorToRunnedScenario.Contains(actor))
	{
		bool success;
		UELScenarioComponentBase* scn;
		StopScenario(actor, success, scn);
	}
	RemoveActor(actor);
	UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] %s finish %s."), *this->GetName(), *actor->GetName(), *this->GetName());
}

void AELSmartObjectBase::PostFinishSmartObject(AActor* actor)
{
	OnSmartObjectFinished.Broadcast(actor, this);
}

bool AELSmartObjectBase::CanActorJoin_Implementation(AActor* actor)
{
	if (actors.Contains(actor) == false)
	{
		if (rangeCheck)
		{
			if (rangeSphere->IsOverlappingActor(actor) == false)
			{
				return false;
			}
		}

		if (coolDownForEachActor > 0.0f && actorToCoolDown.Contains(actor))
		{
			float lastTime = actorToCoolDown[actor];
			float curTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
			if ((curTime - lastTime) < coolDownForEachActor)
			{
				return false;
			}
			//UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] %s is in CD."), *this->GetName(), *actor->GetName());
		}

		switch (actorCheckMethod)
		{
		case EELFrameworkCheckActor::VE_Absolute:
			return CheckActorIsAcceptableAbsolute(actor);
			break;
		case EELFrameworkCheckActor::VE_Derived:
			return CheckActorIsAcceptableDerived(actor);
			break;
		default:
			break;
		}
	}
	return false;
}

bool AELSmartObjectBase::CanActorInterrupted_Implementation(AActor* actor)
{
	switch (interruptHandle)
	{
	case EELSmartObjectInterruptHandleSelfSetting::VE_Default:
	case EELSmartObjectInterruptHandleSelfSetting::VE_Pause:
	case EELSmartObjectInterruptHandleSelfSetting::VE_Leave:
		return true;
	case EELSmartObjectInterruptHandleSelfSetting::VE_NoInterrupt:
		return false;
	default:
		return false;
		break;
	}
}

int AELSmartObjectBase::GetPriority_Implementation(AActor* actor)
{
	return priority;
}

void AELSmartObjectBase::ResetCoolDown(AActor* actor)
{
	checkf(actor != nullptr, TEXT("[%s][ResetCoolDown] actor is null"), *this->GetName());
	actorToCoolDown.Remove(actor);
}

void AELSmartObjectBase::PauseActor(AActor* actor)
{
	checkf(actor != nullptr, TEXT("[%s][PuaseActor] actor is null"), *this->GetName());
	if (actors.Contains(actor))
	{
		UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] %s pause %s."), *this->GetName(), *actor->GetName(), *this->GetName());
		pauseActors.Add(actor);
		OnActorPuase(actor);
	}

}

void AELSmartObjectBase::ResumeActor(AActor* actor)
{
	checkf(actor != nullptr, TEXT("[%s][ResumeActor] actor is null"), *this->GetName());
	if (actors.Contains(actor) && pauseActors.Contains(actor))
	{
		UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] %s resume %s."), *this->GetName(), *actor->GetName(), *this->GetName());
		pauseActors.Remove(actor);
		OnActorResume(actor);
	}
}

void AELSmartObjectBase::OnActorRecieveAnimNotify_Implementation(AActor* actor, FName notifyName, USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	checkf(actor != nullptr, TEXT("[%s][OnActorRecieveAnimNotify] actor is null"), *this->GetName());
	UELScenarioComponentBase* scenario = GetActorCurrentScenario(actor);
	if (IsValid(scenario))
	{
		scenario->RecieveAnimNotify(notifyName, MeshComp, Animation);
	}
}

void AELSmartObjectBase::FinishSmartObject(AActor* actor)
{
	checkf(actor != nullptr, TEXT("[%s][FinishSmartObject] actor is null"), *this->GetName());

	if (actors.Contains(actor))
	{
		DoFinishSmartObject(actor);
		OnSmartObjectPreFinished.Broadcast(actor, this);
		PostFinishSmartObject(actor);
	}
}

void AELSmartObjectBase::RunScenario(AActor* actor, UELScenarioComponentBase* scenarioTemplate, bool& success, UELScenarioComponentBase*& runningScenario)
{
	checkf(actor != nullptr, TEXT("[%s][RunScenario] actor is null"), *this->GetName());
	checkf(scenarioTemplate != nullptr, TEXT("[%s][RunScenario] scenario is null"), *this->GetName());

	runningScenario = nullptr;
	success = false;

	// check actor is in smart object
	if (!actors.Contains(actor))
	{
		UE_LOG(LogELGameFrameworkAI, Warning, TEXT("[%s] does not have Actor-%s, and fail to run scenario (%s)."), *this->GetName(), *actor->GetName(), *scenarioTemplate->GetName());
		return;
	}

	// check actor is running scenario
	if (actorToRunnedScenario.Contains(actor))
	{
		runningScenario = actorToRunnedScenario[actor];
		UE_LOG(LogELGameFrameworkAI, Warning, TEXT("[%s] %s is running scenario-%s, and fail to run scenario (%s)."), *this->GetName(), *actor->GetName(), *runningScenario->GetScenarioName(), *scenarioTemplate->GetName());
		return;
	}

	// check actor has requested scenario and is usable
	//TArray<UActorComponent*> comps = actor->GetComponentsByClass(scenarioTemplate->GetClass());
	TArray<UELScenarioComponentBase*> comps;
	actor->GetComponents<UELScenarioComponentBase>(comps);
	for (auto comp : comps)
	{
		//UELScenarioComponentBase* tmp = Cast<UELScenarioComponentBase>(comp);
		if (comp->GetClass() != scenarioTemplate->GetClass()) continue;
		if (comp->IsPause() == false && comp->IsRunning() == false)
		{
			runningScenario = comp;
			break;
		}
	}

	// add scenario comp if actor does not have one
	if (runningScenario == nullptr)
	{
		//FName snName = *scenarioTemplate->GetName();
		//runningScenario = actor->CreateDefaultSubobject<UELScenarioComponentBase>(snName);
		//auto tmp = actor->CreateComponentFromTemplate(scenarioTemplate);
		UELScenarioComponentBase* tmp = NewObject<UELScenarioComponentBase>(actor, scenarioTemplate->GetClass());
		tmp->RegisterComponent();
		//tmp->SetNetAddressable(); // Make DSO components net addressable
		//tmp->SetIsReplicated(true); // Enable replication by default
		runningScenario = Cast<UELScenarioComponentBase>(tmp);
	}

	// setup and start scenario
	if (runningScenario->Setup(scenarioTemplate))
	{
		success = runningScenario->Start();
		if (success)
		{
			runningScenario->PostStart();
			runningScenario->OnScenarioPreFinished.AddDynamic(this, &AELSmartObjectBase::OnSenarioPreFinished);
			actorToRunnedScenario.Add(actor, runningScenario);
			UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] %s start scenario (%s)."), *this->GetName(), *actor->GetName(), *runningScenario->GetScenarioName());
		}
		else
		{
			UE_LOG(LogELGameFrameworkAI, Warning, TEXT("[%s] %s start scenario failed (%s)."), *this->GetName(), *actor->GetName(), *runningScenario->GetScenarioName());
		}
	}
	else
	{
		UE_LOG(LogELGameFrameworkAI, Warning, TEXT("[%s] %s setup scenario failed (%s)."), *this->GetName(), *actor->GetName(), *runningScenario->GetScenarioName());
	}
}

void AELSmartObjectBase::PauseScenario(AActor* actor, bool& success, UELScenarioComponentBase*& pausedScenario)
{
	checkf(actor != nullptr, TEXT("[%s][PauseScenario] actor is null"), *this->GetName());

	if (actorToRunnedScenario.Contains(actor))
	{
		auto runScn = actorToRunnedScenario[actor];
		success = runScn->Pause();
		if (success)
		{
			if (runScn->IsRunning())	// false if calling finish in Pause()
			{
				actorToPausedScenario.Add(actor, runScn);
				UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] %s pause scenario (%s)."), *this->GetName(), *actor->GetName(), *runScn->GetScenarioName());
			}
		}
		else
		{
			UE_LOG(LogELGameFrameworkAI, Warning, TEXT("[%s] %s pause scenario failed (%s)."), *this->GetName(), *actor->GetName(), *runScn->GetScenarioName());
		}
	}
	else
	{
		UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] %s does not run any scenario. Failed to pause."), *this->GetName(), *actor->GetName());

	}
}

void AELSmartObjectBase::ResumeScenario(AActor* actor, bool& success, UELScenarioComponentBase*& resumedScenario)
{
	checkf(actor != nullptr, TEXT("[%s][ResumeScenario] actor is null"), *this->GetName());

	if (actorToPausedScenario.Contains(actor))
	{
		auto pauseScn = actorToPausedScenario[actor];
		success = pauseScn->Resume();
		if (success)
		{
			if (pauseScn->IsRunning())	// false if calling finish in Resume()
			{
				actorToPausedScenario.Remove(actor);
				UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] %s resume scenario (%s)."), *this->GetName(), *actor->GetName(), *pauseScn->GetScenarioName());
			}
		}
		else
		{
			UE_LOG(LogELGameFrameworkAI, Warning, TEXT("[%s] %s resume scenario failed (%s)."), *this->GetName(), *actor->GetName(), *pauseScn->GetScenarioName());
		}
	}
	else
	{
		UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] %s does not pause any scenario. Failed to resume."), *this->GetName(), *actor->GetName());
	}
}

void AELSmartObjectBase::StopScenario(AActor* actor, bool& success, UELScenarioComponentBase*& stoppedScenario)
{
	checkf(actor != nullptr, TEXT("[%s][StopScenario] actor is null"), *this->GetName());

	if (actorToRunnedScenario.Contains(actor))
	{
		auto runScn = actorToRunnedScenario[actor];
		runScn->PreStop();
		success = runScn->Stop();
		if (success)
		{
			if (runScn->IsRunning())	// If finish when call Stop(), this will be false.
			{
				runScn->OnScenarioPreFinished.RemoveDynamic(this, &AELSmartObjectBase::OnSenarioPreFinished);
				actorToRunnedScenario.Remove(actor);
				if (actorToPausedScenario.Contains(actor))
				{
					actorToPausedScenario.Remove(actor);
				}
				UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] %s stop scenario (%s)."), *this->GetName(), *actor->GetName(), *runScn->GetScenarioName());
				runScn->PostStop();
			}
		}
		else
		{
			UE_LOG(LogELGameFrameworkAI, Warning, TEXT("[%s] %s stop scenario failed (%s)."), *this->GetName(), *actor->GetName(), *runScn->GetScenarioName());

		}
	}
	else
	{
		UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] %s does not run any scenario."), *this->GetName(), *actor->GetName());

	}
}

UELScenarioComponentBase* AELSmartObjectBase::GetActorCurrentScenario(AActor* actor)
{
	checkf(actor != nullptr, TEXT("[%s][GetActorRunningScenario] actor is null"), *this->GetName());
	UELScenarioComponentBase* result = nullptr;
	if (actorToRunnedScenario.Contains(actor))
	{
		result = actorToRunnedScenario[actor];
	}
	return result;
}

void AELSmartObjectBase::OnSenarioPreFinished(AActor* actor, UELScenarioComponentBase* scenario)
{
	if (actorToRunnedScenario.Contains(actor))
	{
		auto runScn = actorToRunnedScenario[actor];
		if (runScn != scenario)
		{
			UE_LOG(LogELGameFrameworkAI, Error, TEXT("[%s] %s's running scenario doesn't match. (running: %s) (call finish: %s)"), *this->GetName(), *actor->GetName(), *runScn->GetScenarioName(), *scenario->GetScenarioName());
		}
		runScn->OnScenarioPreFinished.RemoveDynamic(this, &AELSmartObjectBase::OnSenarioPreFinished);
		actorToRunnedScenario.Remove(actor);
		if (actorToPausedScenario.Contains(actor))
		{
			UE_LOG(LogELGameFrameworkAI, Warning, TEXT("[%s] %s is pause but call finish. (scenario is run by %s)"), *this->GetName(), *scenario->GetScenarioName(), *actor->GetName());
			actorToPausedScenario.Remove(actor);
		}
		UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] %s finish scenario (%s)."), *this->GetName(), *actor->GetName(), *scenario->GetScenarioName());
	}
	else
	{
		UE_LOG(LogELGameFrameworkAI, Error, TEXT("[%s] %s does not run any scenario. (call finish: %s)"), *this->GetName(), *actor->GetName(), *scenario->GetScenarioName());
	}
}

void AELSmartObjectBase::DoAddActor(AActor* actor)
{
	if (rangeCheck)
	{
		if (rangeSphere->IsOverlappingActor(actor) == false)
		{
			UE_LOG(LogELGameFrameworkAI, Warning, TEXT("[%s] %s is not in range."), *this->GetName(), *actor->GetName());
			return;
		}
	}
	if (CanActorJoin(actor))
	{
		UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] %s join."), *this->GetName(), *actor->GetName());
		Super::DoAddActor(actor);
	}
}

void AELSmartObjectBase::DoRemoveActor(AActor* actor)
{
	if (actors.Contains(actor))
	{
		pauseActors.Remove(actor);
		if (coolDownForEachActor > 0.0f)
		{
			actorToCoolDown.Add(actor, UGameplayStatics::GetRealTimeSeconds(GetWorld()));
		}
		Super::DoRemoveActor(actor);
		UE_LOG(LogELGameFrameworkAI, Log, TEXT("[%s] %s leave."), *this->GetName(), *actor->GetName());
	}
}
