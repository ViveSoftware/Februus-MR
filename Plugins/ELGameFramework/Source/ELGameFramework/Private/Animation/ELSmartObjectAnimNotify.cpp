// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/ELSmartObjectAnimNotify.h"
#include "AI/ELSmartObjectHelperComponent.h"
#include "UObject/Object.h"

void UELSmartObjectAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (IsValid(MeshComp))
	{
		AActor* actor = MeshComp->GetOwner();
		//if (actor->GetWorld()->WorldType != EWorldType::EditorPreview)
		if (IsValid(actor))
		{
			UActorComponent* comp = actor->GetComponentByClass(UELSmartObjectHelperComponent::StaticClass());
			if (IsValid(comp))
			{
				UELSmartObjectHelperComponent* soHelper = Cast<UELSmartObjectHelperComponent>(comp);
				AELSmartObjectBase* curSO = soHelper->currentSmartObject;
				if (IsValid(curSO))
				{
					curSO->OnActorRecieveAnimNotify(actor, actionName, MeshComp, Animation);
				}
			}
		}
	}
	Super::Notify(MeshComp, Animation, EventReference);
}
