// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GraphStructureVertex.h"
#include "UObject/NoExportTypes.h"
#include "GraphConnectedComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGraphConnectedComponent_OnVertexAdded_Signature, UGraphStructureVertex*, Vertex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGraphConnectedComponent_OnVertexRemoved_Signature, UGraphStructureVertex*, Vertex);

class UGraphConnectedComponentsMonitor;
/**
 * 
 */
UCLASS(BlueprintType)
class UNREALGRAPHSTRUCTUREPLUGIN_API UGraphConnectedComponent : public UObject
{
	friend UGraphConnectedComponentsMonitor;
	GENERATED_BODY()

	UPROPERTY()
	TSet<UGraphStructureVertex*> Vertices;

	void AddVertex(UGraphStructureVertex* Vertex);

	void RemoveVertex(UGraphStructureVertex* Vertex);

public:
	UFUNCTION(BlueprintPure)
	TSet<UGraphStructureVertex*> GetVertices();

	UPROPERTY(BlueprintAssignable)
	FGraphConnectedComponent_OnVertexAdded_Signature OnVertexAdded;

	UPROPERTY(BlueprintAssignable)
	FGraphConnectedComponent_OnVertexRemoved_Signature OnVertexRemoved;
};
