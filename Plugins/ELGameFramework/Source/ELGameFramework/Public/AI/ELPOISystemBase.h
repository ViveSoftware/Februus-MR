// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/ELSystemBase.h"
#include "ELSmartObjectBase.h"
#include "ELSmartObjectHelperComponent.h"
#include "ELPOISystemBase.generated.h"

class ELGAMEFRAMEWORK_API ELSmartObjectSorter
{
public:
	ELSmartObjectSorter() {};
	~ELSmartObjectSorter() {} ;

	static void GetSortedSmartObjects(AActor* actor, TArray<AELSmartObjectBase*>& smartObject) {
		smartObject.Sort([actor](AELSmartObjectBase& A, AELSmartObjectBase& B)
		{
			return A.GetPriority(actor) > B.GetPriority(actor);
		});
	}
};


/**
 * 
 */
UCLASS()
class ELGAMEFRAMEWORK_API AELPOISystemBase : public AELSystemBase
{
	GENERATED_BODY()

public:
	AELPOISystemBase();

	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELPOISystem | ActorOperation")
	AELSmartObjectBase* FindSmartObject(AActor* actor);

	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELPOISystem | ActorOperation")
	bool JoinSmartObject(AActor* actor, AELSmartObjectBase* smartObject);

	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELPOISystem | ActorOperation")
	bool RequestSmartObject(AActor* actor, AELSmartObjectBase* smartObject, bool canInterruptCheck, bool priorityCheck, EELSmartObjectInterruptHandleRequestByOther requestInterruptHandle);

	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELPOISystem | ActorOperation")
	AELSmartObjectBase* RequestSmartObjectByClass(AActor* actor, TSubclassOf<AELSmartObjectBase> smartObjectClass, bool canInterruptCheck, bool priorityCheck, EELSmartObjectInterruptHandleRequestByOther requestInterruptHandle);

	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELPOISystem | ActorOperation")
	void ForceLeaveAllSmartObjects(AActor* actor);

protected:
	UFUNCTION()
	void OnFinishSmartObject(AActor* actor, AELSmartObjectBase* smartObject);

private:
	virtual void DoAddActor(AActor* actor) override;
	virtual void DoRemoveActor(AActor* actor) override;

	inline UELSmartObjectHelperComponent* GetSmartObjectHelper(AActor* actor);

private:
	TArray<AELSmartObjectBase*> smartObjects_;
	TArray<AELSmartObjectBase*> smartObjectsCanBeFound_;
};
