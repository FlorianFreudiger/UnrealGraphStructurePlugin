// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GraphStructureEdge.generated.h"

class UGraphStructureVertex;
/**
 * 
 */
UCLASS(BlueprintType)
class UNREALGRAPHSTRUCTUREPLUGIN_API UGraphStructureEdge : public UObject
{
	GENERATED_BODY()

public:
	UGraphStructureEdge();

	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn=true))
	UGraphStructureVertex* Source;

	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn=true))
	UGraphStructureVertex* Target;

	// Debugging

	UFUNCTION(Category="GraphStructure|Debugging")
	FString GetGraphvizDotRepresentation();
};
