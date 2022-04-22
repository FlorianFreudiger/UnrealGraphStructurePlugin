// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectWithWorldContext.h"

UWorld* UObjectWithWorldContext::GetWorld() const
{
	// Check if CDO in which case return null since calling GetWorld on the Outer will disable world context
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return nullptr;
	}

	// Search for AActor Outer, should find one as long as some Actor or ActorComponent spawned this or parent or parent-parent-... UObject
	UObject* Outer = GetTypedOuter<AActor>();
	if (ensure(Outer != nullptr))
	{
		return Outer->GetWorld();
	}
	return nullptr;
}
