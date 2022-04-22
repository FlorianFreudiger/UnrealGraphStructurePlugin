// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ObjectWithWorldContext.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class UNREALGRAPHSTRUCTUREPLUGIN_API UObjectWithWorldContext : public UObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;
};
