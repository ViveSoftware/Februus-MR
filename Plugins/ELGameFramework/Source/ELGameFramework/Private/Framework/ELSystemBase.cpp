// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/ELSystemBase.h"
#include "Net/UnrealNetwork.h"

//DEFINE_LOG_CATEGORY_STATIC(LogELGameFramework, Log, All)

// Sets default values
AELSystemBase::AELSystemBase()
{
	bReplicates = true;
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AELSystemBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AELSystemBase, name);
	DOREPLIFETIME(AELSystemBase, enable);
	DOREPLIFETIME(AELSystemBase, myWorld);
	DOREPLIFETIME(AELSystemBase, acceptableActorClass);
	DOREPLIFETIME(AELSystemBase, acceptableComponentClass);
	DOREPLIFETIME(AELSystemBase, actors);
	DOREPLIFETIME(AELSystemBase, checkType);
	DOREPLIFETIME(AELSystemBase, actorCheckMethod);
	DOREPLIFETIME(AELSystemBase, componentCheckMethod);
}

bool AELSystemBase::SetEnable_Validate(bool value)
{
	return enable != value;
}

void AELSystemBase::SetEnable_Implementation(bool value)
{
	enable = value;
	if (enable == true)
	{
		OnEnable();
	}
	else
	{
		OnDisable();
	}
}

bool AELSystemBase::GetEnable()
{
	return enable;
}

bool AELSystemBase::AddActor_Validate(AActor* actor)
{
	return actor != nullptr;
}

PRAGMA_DISABLE_OPTIMIZATION
void AELSystemBase::AddActor_Implementation(AActor* actor)
{
	checkf(actor != nullptr, TEXT("[%s][AddActor] actor is null"), *this->GetName());

	if (actors.Contains(actor) == false)
	{
		switch (checkType)
		{
		case EELFrameworkCheckType::VE_None:
			DoAddActor(actor);
			break;
		case EELFrameworkCheckType::VE_Actor:
			if (CheckActor(actor)) DoAddActor(actor);
			break;
		case EELFrameworkCheckType::VE_Component:
			if (CheckComponent(actor)) DoAddActor(actor);
			break;
		case EELFrameworkCheckType::VE_Both:
			if (CheckActor(actor) && CheckComponent(actor)) DoAddActor(actor);
			break;
		case EELFrameworkCheckType::VE_Any:
			if (CheckActor(actor) || CheckComponent(actor)) DoAddActor(actor);
			break;
		default:
			break;
		}
		//switch (actorCheckMethod)
		//{
		//case EELFrameworkCheckActor::VE_Absolute:
		//	if (CheckActorIsAcceptableAbsolute(actor) == true) DoAddActor(actor);
		//	break;
		//case EELFrameworkCheckActor::VE_Derived:
		//	if (CheckActorIsAcceptableDerived(actor) == true) DoAddActor(actor);
		//	break;
		//case EELFrameworkCheckActor::VE_None:
		//	DoAddActor(actor);
		//	break;
		//default:
		//	break;
		//}
	}
}
PRAGMA_ENABLE_OPTIMIZATION

bool AELSystemBase::RemoveActor_Validate(AActor* actor)
{
	return actor != nullptr;
}

void AELSystemBase::RemoveActor_Implementation(AActor* actor)
{
	checkf(actor != nullptr, TEXT("[%s][RemoveActor] actor is null"), *this->GetName());

	if (actors.Contains(actor))
	{
		DoRemoveActor(actor);
	}
}

bool AELSystemBase::ActorIsInSystem(AActor* actor)
{
	checkf(actor != nullptr, TEXT("[%s][ActorIsInSystem] actor is null"), *this->GetName());

	return actors.Contains(actor);
}

PRAGMA_DISABLE_OPTIMIZATION
void AELSystemBase::CheckRemoveActor(AActor* actor)
{

	checkf(actor != nullptr, TEXT("[%s][CheckRemoveActor] actor is null"), *this->GetName());
	if (actors.Contains(actor) == true)
	{
		switch (checkType)
		{
		case EELFrameworkCheckType::VE_None:
			DoRemoveActor(actor);
			break;
		case EELFrameworkCheckType::VE_Actor:
			if (CheckActor(actor) == false) DoRemoveActor(actor);
			break;
		case EELFrameworkCheckType::VE_Component:
			if (CheckComponent(actor) == false) DoRemoveActor(actor);
			break;
		case EELFrameworkCheckType::VE_Both:
			if (CheckActor(actor) == false && CheckComponent(actor) == false) DoRemoveActor(actor);
			break;
		case EELFrameworkCheckType::VE_Any:
			if (CheckActor(actor) == false || CheckComponent(actor) == false) DoRemoveActor(actor);
			break;
		default:
			break;
		}
	}
}
PRAGMA_ENABLE_OPTIMIZATION

void AELSystemBase::OnEnable_Implementation()
{
	OnEnableNative();
}

void AELSystemBase::OnDisable_Implementation()
{
	OnDisableNative();
}

// Called when the game starts or when spawned
void AELSystemBase::BeginPlay()
{
	if (name.IsEmpty())
	{
		UE_LOG(LogELGameFramework, Error, TEXT("[%s] ELSystem's name is empty."), *this->GetName());
	}

	for (AActor* dependentOnActor : dependentOnActors)
	{
		if (dependentOnActor != nullptr)
		{
			if (!dependentOnActor->HasActorBegunPlay())
			{
				dependentOnActor->DispatchBeginPlay();
			}
		}
	}

	Super::BeginPlay();
}

void AELSystemBase::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);
	if (name.IsEmpty())
	{
		name = GetClass()->GetName();
	}
}

void AELSystemBase::OnEnableNative()
{

}

void AELSystemBase::OnDisableNative()
{

}

// Called every frame
void AELSystemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AELSystemBase::CheckActor(AActor* actor)
{
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
	return false;
}

bool AELSystemBase::CheckComponent(AActor * actor)
{
	switch (componentCheckMethod)
	{
	case EELFrameworkCheckComponent::VE_Any:
		return CheckAnyComponentIsAcceptable(actor);
		break;
	case EELFrameworkCheckComponent::VE_All:
		return CheckAllComponentIsAcceptable(actor);
		break;
	default:
		break;
	}
	return false;
}

bool AELSystemBase::CheckActorIsAcceptableAbsolute(AActor* actor)
{
	return acceptableActorClass.Contains(actor->GetClass());
}

bool AELSystemBase::CheckActorIsAcceptableDerived(AActor* actor)
{
	for (auto classType : acceptableActorClass)
	{
		if (actor->IsA(classType))
		{
			return true;
		}
	}
	return false;
}

bool AELSystemBase::CheckAllComponentIsAcceptable(AActor* actor)
{
	for (auto compType : acceptableComponentClass)
	{
		auto comp = actor->GetComponentByClass(compType);
		//if (comp == nullptr)
		if (IsValid(comp) == false || comp->IsBeingDestroyed())
		{
			return false;
		}
	}
	return true;
}

bool AELSystemBase::CheckAnyComponentIsAcceptable(AActor* actor)
{
	for (auto compType : acceptableComponentClass)
	{
		auto comp = actor->GetComponentByClass(compType);
		//if (comp != nullptr)
		if (IsValid(comp) && comp->IsBeingDestroyed() == false)
		{
			return true;
		}
	}
	return false;
}

void AELSystemBase::DoAddActor(AActor* actor)
{
	actors.Add(actor);
	actor->OnDestroyed.AddDynamic(this, &AELSystemBase::DoOnActorDestroyed);
	UE_LOG(LogELGameFramework, Log, TEXT("[%s] %s join system."), *this->GetName(), *actor->GetName());
	OnActorJoin(actor);
}

void AELSystemBase::DoRemoveActor(AActor* actor)
{
	actors.Remove(actor);
	actor->OnDestroyed.RemoveDynamic(this, &AELSystemBase::DoOnActorDestroyed);
	UE_LOG(LogELGameFramework, Log, TEXT("[%s] %s leave system."), *this->GetName(), *actor->GetName());
	OnActorLeave(actor);
}

void AELSystemBase::DoOnActorDestroyed(AActor* actor)
{
	OnActorDestoryed(actor);
	RemoveActor(actor);
}



