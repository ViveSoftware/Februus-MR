// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Components/SkeletalMeshComponent.h"
#include "ELSmartObjectAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class ELGAMEFRAMEWORK_API UELSmartObjectAnimNotify : public UAnimNotify
{
	GENERATED_BODY()
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ELGameFramework")
	FName actionName;

};
