// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ELSmartObjectBase.h"
#include "ELSmartObjectHelperComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELGAMEFRAMEWORK_API UELSmartObjectHelperComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UELSmartObjectHelperComponent();

	void SetCurrentSmartObjectToPause();

	inline AELSmartObjectBase* GetRecentlyPausedSmartObject(bool autoRemovePause = true) 
	{
		if (pauseSmartObjects.Num() > 0)
		{
			AELSmartObjectBase* result = pauseSmartObjects.Last();
			if (autoRemovePause)
			{
				pauseSmartObjects.RemoveAt(pauseSmartObjects.Num() - 1);
			}
			return result;
		}
		return nullptr;
	}

	UPROPERTY(BlueprintReadOnly, Category = "ELGameFramework | SmartObjectHelper")
	AELSmartObjectBase *currentSmartObject;

	UPROPERTY(BlueprintReadOnly, Category = "ELGameFramework | SmartObjectHelper")
	TArray<AELSmartObjectBase*> pauseSmartObjects;

		
};
