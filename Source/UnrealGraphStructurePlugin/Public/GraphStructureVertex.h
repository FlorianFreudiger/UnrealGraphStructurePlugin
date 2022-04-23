// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GraphStructureVertex.generated.h"

class UGraphStructureEdge;
/**
 * 
 */
UCLASS(Blueprintable)
class UNREALGRAPHSTRUCTUREPLUGIN_API UGraphStructureVertex : public UObject
{
	GENERATED_BODY()

public:
	UGraphStructureVertex();

	UPROPERTY(BlueprintReadOnly)
	TSet<UGraphStructureEdge*> Edges;

	// Debugging

	UFUNCTION(BlueprintImplementableEvent, Category="GraphStructure|Debugging")
	TMap<FString, FString> GetGraphvizDotAttributes();
};
