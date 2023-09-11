// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ELWorldBase.h"
#include "ELSystemBase.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EELFrameworkCheckType : uint8
{
	VE_None 			UMETA(DisplayName = "None"),
	VE_Actor 			UMETA(DisplayName = "Actor"),
	VE_Component 		UMETA(DisplayName = "Component"),
	VE_Both				UMETA(DisplayName = "Both"),
	VE_Any				UMETA(DisplayName = "Any")
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EELFrameworkCheckActor : uint8
{
	VE_Derived 		UMETA(DisplayName = "Derived"),
	VE_Absolute		UMETA(DisplayName = "Absolute")
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EELFrameworkCheckComponent : uint8
{
	VE_Any 			UMETA(DisplayName = "Any"),
	VE_All 			UMETA(DisplayName = "All")
};

UCLASS(Blueprintable, BlueprintType)
class ELGAMEFRAMEWORK_API AELSystemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	friend class AELWorldBase;

	// Sets default values for this actor's properties
	AELSystemBase();

	/** Set system enable. */
	UFUNCTION(Server, Reliable, BlueprintCallable, WithValidation, Category = "ELGameFramework | ELSystem")
	void SetEnable(bool value);

	/** Get system is enable. */
	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELSystem")
	bool GetEnable();

	/** Add actor to system. In normal, no need to call this. Add actor to world will automatically add to corresponding system. Call if you want to handle by yourself. */
	UFUNCTION(Server, Reliable, BlueprintCallable, WithValidation, Category = "ELGameFramework | ELSystem")
	virtual void AddActor(AActor* actor);

	/** Remove actor from system. In normal, no need to call this. Remove actor from world will automatically remove from corresponding system. Call if you want to handle by yourself. */
	UFUNCTION(Server, Reliable, BlueprintCallable, WithValidation, Category = "ELGameFramework | ELSystem")
	virtual void RemoveActor(AActor* actor);

	/** Check actor is in system. */
	UFUNCTION(BlueprintCallable, Category = "ELGameFramework | ELSystem")
	bool ActorIsInSystem(AActor* actor);

	UFUNCTION(BlueprintNativeEvent, Category = "ELGameFramework")
	void OnEnable();

	UFUNCTION(BlueprintNativeEvent, Category = "ELGameFramework")
	void OnDisable();

	UFUNCTION(BlueprintImplementableEvent, Category = "ELGameFramework")
	void OnActorJoin(AActor* actor);

	UFUNCTION(BlueprintImplementableEvent, Category = "ELGameFramework")
	void OnActorLeave(AActor* actor);

	UFUNCTION(BlueprintImplementableEvent, Category = "ELGameFramework")
	void OnActorDestoryed(AActor* actor);

	FString GetSystemName() { return name; };

	void CheckRemoveActor(AActor* actor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform & Transform) override;

	virtual void OnEnableNative();
	virtual void OnDisableNative();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "ELGameFramework")
	FString name;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "ELGameFramework")
	bool enable = true;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "ELGameFramework")
	AELWorldBase* myWorld = nullptr;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "ELGameFramework")
	TArray<TSubclassOf<AActor>> acceptableActorClass;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "ELGameFramework")
	TArray<TSubclassOf<UActorComponent>> acceptableComponentClass;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "ELGameFramework")
	TArray<AActor*> actors;

	/** Check actor is class or actor has component. */
	UPROPERTY(Replicated, EditAnywhere, Category = "ELGameFramework")
	EELFrameworkCheckType checkType = EELFrameworkCheckType::VE_Actor;

	/** If check actor is class, check is derived class or is absolute class*/
	UPROPERTY(Replicated, EditAnywhere, Category = "ELGameFramework")
	EELFrameworkCheckActor actorCheckMethod = EELFrameworkCheckActor::VE_Derived;

	/** If check actor has component, check has all components in list or just has any one of them. */
	UPROPERTY(Replicated, EditAnywhere, Category = "ELGameFramework")
	EELFrameworkCheckComponent componentCheckMethod = EELFrameworkCheckComponent::VE_All;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ELGameFramework")
	TArray<AActor*> dependentOnActors;

	virtual void DoAddActor(AActor* actor);
	virtual void DoRemoveActor(AActor* actor);

	UFUNCTION()
	virtual void DoOnActorDestroyed(AActor* actor);

	bool CheckActor(AActor* actor);
	bool CheckComponent(AActor * actor);

	bool CheckActorIsAcceptableAbsolute(AActor* actor);
	bool CheckActorIsAcceptableDerived(AActor* actor);
	bool CheckAllComponentIsAcceptable(AActor* actor);
	bool CheckAnyComponentIsAcceptable(AActor* actor);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:



};
