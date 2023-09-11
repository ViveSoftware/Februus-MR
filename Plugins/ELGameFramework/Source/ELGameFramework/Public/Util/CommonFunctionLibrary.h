// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "Modules/ModuleManager.h"
#include "CommonFunctionLibrary.generated.h"


DECLARE_DYNAMIC_DELEGATE_ThreeParams(FCompareObjects, UObject*, ObjectA, UObject*, ObjectB, bool&, Result);

/**
 * 
 */
UCLASS()
class ELGAMEFRAMEWORK_API UCommonFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/** Set active (visibility, collision, and tick) */
	UFUNCTION(BlueprintCallable, Category = "ELCommonUtility | Actor")
	static void SetActive(AActor* actor, bool active);

	/** Set visibility, collision, or tick*/
	UFUNCTION(Blueprintcallable, category = "ELCommonUtility | Actor")
	static void SetActiveAdvanced(AActor* actor, bool visible, bool collisionEnable, bool tickEnable);

	/** Check actor is visible, collision enable, tick enable, and active or not*/
	UFUNCTION(Blueprintcallable, category = "ELCommonUtility | Actor")
	static void IsActive(AActor* actor, bool &active, bool &visible, bool &collisionEnable, bool &tickEnable);

	UFUNCTION(BlueprintPure, Category = "ELCommonUtility | Array")
	static TArray<FString> SortStringArrayCopy(const TArray<FString>& StringArray, const bool Reversed = false);
	
	UFUNCTION(BlueprintCallable, Category = "ELCommonUtility | Array")
	static void SortStringArrayDirectly(UPARAM(ref) TArray<FString>& StringArray, const bool Reversed = false);

	UFUNCTION(BlueprintPure, Category = "ELCommonUtility | Array")
	static TArray<int32> SortIntegerArrayCopy(const TArray<int32>& IntegerArray, const bool Reversed = false);

	UFUNCTION(BlueprintCallable, Category = "ELCommonUtility | Array")
	static void SortIntegerArrayDirectly(UPARAM(ref) TArray<int32>& IntegerArray, const bool Reversed = false);

	UFUNCTION(BlueprintPure, Category = "ELCommonUtility | Array")
	static TArray<float> SortFloatArrayCopy(const TArray<float>& FloatArray, const bool Reversed = false);

	UFUNCTION(BlueprintCallable, Category = "ELCommonUtility | Array")
	static void SortFloatArrayDirectly(UPARAM(ref) TArray<float>& FloatArray, const bool Reversed = false);

	/**
	* Sorts a copy of the given array.
	*
	* To create the Comparator, do this:
	*  - create a function that has 2 input parameters (Object and Object) and 1 output parameter (Boolean)
	*  - it is important that the parameters have the following names: ObjectA, ObjectB and Return
	*  - in that function, return true if ObjectA is smaller than ObjectB, return false otherwise
	*  - then, when using the Sort Object Array blueprint, use the Create Event blueprint and set its value as the function created earlier
	*/
	UFUNCTION(BlueprintPure, Category = "ELCommonUtility | Array")
	static TArray<UObject*> SortObjectArrayCopy(const TArray<UObject*>& ObjectArray, FCompareObjects Comparator, const bool Reversed = false);

	/**
	* Sorts the given array.
	*
	* To create the Comparator, do this:
	*  - create a function that has 2 input parameters (Object and Object) and 1 output parameter (Boolean)
	*  - it is important that the parameters have the following names: ObjectA, ObjectB and Return
	*  - in that function, return true if ObjectA is smaller than ObjectB, return false otherwise
	*  - then, when using the Sort Object Array blueprint, use the Create Event blueprint and set its value as the function created earlier
	*/
	UFUNCTION(BlueprintCallable, Category = "ELCommonUtility | Array")
	static void SortObjectArrayDirectly(UPARAM(ref) TArray<UObject*>& ObjectArray, FCompareObjects Comparator, const bool Reversed = false);

	UFUNCTION(BlueprintPure, Category = "ELCommonUtility | Mesh")
	static float GetClosestPointOnMesh(const UPrimitiveComponent* meshComponent, const FVector & Point, FVector & ClosestPointOnMesh);

	UFUNCTION(BlueprintCallable, Category = "ELCommonUtility | Component")
	static UActorComponent* AddActorComponentFromClass(AActor* Actor, UClass* ActorComponentClass);

	UFUNCTION(BlueprintCallable, Category = "ELCommonUtility | Component")
	static void ForceDestroyComponent(UActorComponent* ActorComponent);

	UFUNCTION(BlueprintCallable, Category = "ELCommonUtility | Module")
	static bool CheckModuleIsLoaded(FName moduleName);

	UFUNCTION(BlueprintCallable, Category = "ELCommonUtility | Camera")
	static void TakeHighResScreenshot(UCameraComponent* camera);

	UFUNCTION(BlueprintCallable, Category = "ELCommonUtility | Navigation")
	static void UpdateNavigationData(UActorComponent* ActorComponent);
};
