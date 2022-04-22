// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GraphStructureVertex.h"
#include "ObjectWithWorldContext.h"
#include "UObject/NoExportTypes.h"
#include "GraphConnectedComponent.generated.h"

class UGraphConnectedComponentsMonitor;
/**
 * 
 */
UCLASS(Blueprintable)
class UNREALGRAPHSTRUCTUREPLUGIN_API UGraphConnectedComponent : public UObjectWithWorldContext
{
	friend UGraphConnectedComponentsMonitor;
	GENERATED_BODY()

	UPROPERTY()
	TSet<UGraphStructureVertex*> Vertices;

protected:
	// Implementable functions

	UFUNCTION(BlueprintImplementableEvent)
	void OnCreated();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDestroyed();

	UFUNCTION(BlueprintImplementableEvent)
	void OnVertexAdded(UGraphStructureVertex* Vertex);

	UFUNCTION(BlueprintImplementableEvent)
	void OnVertexRemoved(UGraphStructureVertex* Vertex);

public:
	UFUNCTION(BlueprintPure)
	TSet<UGraphStructureVertex*> GetVertices();
};
