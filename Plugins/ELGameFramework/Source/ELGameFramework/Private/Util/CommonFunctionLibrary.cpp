// Fill out your copyright notice in the Description page of Project Settings.

#include "Util/CommonFunctionLibrary.h"
#include "AI/NavigationSystemBase.h"

void UCommonFunctionLibrary::SetActive(AActor * actor, bool active)
{
	// Hides visible components
	actor->SetActorHiddenInGame(!active);

	// Disables collision components
	actor->SetActorEnableCollision(active);

	// Stops the Actor from ticking
	actor->SetActorTickEnabled(active);
}

void UCommonFunctionLibrary::SetActiveAdvanced(AActor* actor, bool visible, bool collisionEnable, bool tickEnable)
{
	// Hides visible components
	actor->SetActorHiddenInGame(!visible);

	// Disables collision components
	actor->SetActorEnableCollision(collisionEnable);

	// Stops the Actor from ticking
	actor->SetActorTickEnabled(tickEnable);
}

void UCommonFunctionLibrary::IsActive(AActor * actor, bool &active, bool & visible, bool & collisionEnable, bool & tickEnable)
{
	visible = actor->IsHidden();
	collisionEnable = actor->GetActorEnableCollision();
	tickEnable = actor->IsActorTickEnabled();
	active = visible && collisionEnable && tickEnable;
}

TArray<FString> UCommonFunctionLibrary::SortStringArrayCopy(const TArray<FString>& StringArray, const bool Reversed /*= false*/)
{
	TArray<FString> Array = StringArray;
	SortStringArrayDirectly(Array, Reversed);
	return Array;
}

void UCommonFunctionLibrary::SortStringArrayDirectly(UPARAM(ref) TArray<FString>& StringArray, const bool Reversed /*= false*/)
{
	if (!Reversed)
	{
		StringArray.Sort([](const FString& A, const FString& B)
		{
			return A < B;
		});
	}
	else
	{
		StringArray.Sort([](const FString& A, const FString& B)
		{
			return A > B;
		});
	}
}

TArray<int32> UCommonFunctionLibrary::SortIntegerArrayCopy(const TArray<int32>& IntegerArray, const bool Reversed /*= false*/)
{
	TArray<int32> Array = IntegerArray;
	SortIntegerArrayDirectly(Array, Reversed);
	return Array;
}

void UCommonFunctionLibrary::SortIntegerArrayDirectly(UPARAM(ref) TArray<int32>& IntegerArray, const bool Reversed /*= false*/)
{
	if (!Reversed)
	{
		IntegerArray.Sort([](const int32 A, const int32 B)
		{
			return A < B;
		});
	}
	else
	{
		IntegerArray.Sort([](const int32 A, const int32 B)
		{
			return A > B;
		});
	}
}

TArray<float> UCommonFunctionLibrary::SortFloatArrayCopy(const TArray<float>& FloatArray, const bool Reversed /*= false*/)
{
	TArray<float> Array = FloatArray;
	SortFloatArrayDirectly(Array, Reversed);
	return Array;
}

void UCommonFunctionLibrary::SortFloatArrayDirectly(UPARAM(ref) TArray<float>& FloatArray, const bool Reversed /*= false*/)
{
	if (!Reversed)
	{
		FloatArray.Sort([](const float A, const float B)
		{
			return A < B;
		});
	}
	else
	{
		FloatArray.Sort([](const float A, const float B)
		{
			return A > B;
		});
	}
}

TArray<UObject*> UCommonFunctionLibrary::SortObjectArrayCopy(const TArray<UObject*>& ObjectArray, FCompareObjects Comparator, const bool Reversed /*= false*/)
{
	TArray<UObject*> Array = ObjectArray;
	SortObjectArrayDirectly(Array, Comparator, Reversed);
	return Array;
}

void UCommonFunctionLibrary::SortObjectArrayDirectly(UPARAM(ref) TArray<UObject*>& ObjectArray, FCompareObjects Comparator, const bool Reversed /*= false*/)
{
	if (!Reversed)
	{
		ObjectArray.Sort([Comparator](UObject& A, UObject& B)
		{
			bool Result = false;
			Comparator.Execute(&A, &B, Result);
			return Result;
		});
	}
	else
	{
		ObjectArray.Sort([Comparator](UObject& A, UObject& B)
		{
			bool Result = false;
			Comparator.Execute(&A, &B, Result);
			return !Result;
		});
	}
}

float UCommonFunctionLibrary::GetClosestPointOnMesh(const UPrimitiveComponent* meshComponent, const FVector & Point, FVector & ClosestPointOnMesh)
{
	if (IsValid(meshComponent))
	{
		return meshComponent->GetDistanceToCollision(Point, ClosestPointOnMesh);
	}
	else
	{
		return -1;
	}
}

UActorComponent * UCommonFunctionLibrary::AddActorComponentFromClass(AActor* Actor, UClass * ActorComponentClass)
{
	checkf(Actor != nullptr, TEXT("[AddActorComponentFromClass] actor is null"));

	UClass * baseClass = FindObject<UClass>(nullptr, TEXT("ActorComponent"));
	if (ActorComponentClass->IsChildOf(baseClass))
	{
		UActorComponent* NewComp = NewObject<UActorComponent>(Actor, ActorComponentClass);
		if (!NewComp)
		{
			return nullptr;
		}
		//~~~~~~~~~~~~~

		NewComp->bAllowAnyoneToDestroyMe = 1;
		NewComp->RegisterComponent();        //You must ConstructObject with a valid Outer that has world, see above     

		return NewComp;
	}
	else
	{
		return nullptr;
	}
}

void UCommonFunctionLibrary::ForceDestroyComponent(UActorComponent* ActorComponent)
{
	if (IsValid(ActorComponent))
	{
		//ActorComponent->bAllowAnyoneToDestroyMe = 1;
		ActorComponent->DestroyComponent();
	}
}

bool UCommonFunctionLibrary::CheckModuleIsLoaded(FName moduleName)
{
	return FModuleManager::Get().IsModuleLoaded(moduleName);
}

void UCommonFunctionLibrary::TakeHighResScreenshot(UCameraComponent* camera)
{
	//GetWorld()->GetGameViewport()->Viewport->TakeHighResScreenShot();
	camera->GetWorld()->GetGameViewport()->Viewport->TakeHighResScreenShot();
}

void UCommonFunctionLibrary::UpdateNavigationData(UActorComponent* ActorComponent)
{
	FNavigationSystem::UpdateComponentData(*ActorComponent);
}

