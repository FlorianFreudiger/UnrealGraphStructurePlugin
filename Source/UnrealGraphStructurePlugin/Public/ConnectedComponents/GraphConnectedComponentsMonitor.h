// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GraphConnectedComponent.h"
#include "GraphStructure.h"
#include "UObject/NoExportTypes.h"
#include "GraphConnectedComponentsMonitor.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class UNREALGRAPHSTRUCTUREPLUGIN_API UGraphConnectedComponentsMonitor : public UObject
{
	GENERATED_BODY()

	bool SetupCompleted = false;

	UPROPERTY()
	UGraphStructure* Graph;

	UPROPERTY()
	TMap<UGraphStructureVertex*, UGraphConnectedComponent*> VerticesComponentsMap;

	UPROPERTY()
	TSubclassOf<UGraphConnectedComponent> ConnectedComponentClass;

	// Internal functions that additionally call implementable functions of the ConnectedComponents

	UGraphConnectedComponent* ConnectedComponent_Spawn();

	void ConnectedComponent_Destroy(UGraphConnectedComponent* ConnectedComponent);

	void ConnectedComponent_AddVertex(UGraphConnectedComponent* ConnectedComponent, UGraphStructureVertex* Vertex);

	void ConnectedComponent_RemoveVertex(UGraphConnectedComponent* ConnectedComponent, UGraphStructureVertex* Vertex);

	// Functions for binding to graph delegates

	UFUNCTION()
	void GraphStructure_VertexAdded(UGraphStructureVertex* Vertex);

	UFUNCTION()
	void GraphStructure_VertexRemoved(UGraphStructureVertex* Vertex);

	UFUNCTION()
	void GraphStructure_EdgeAdded(UGraphStructureEdge* Edge);

	UFUNCTION()
	void GraphStructure_EdgeRemoved(UGraphStructureEdge* Edge);

public:
	UFUNCTION(BlueprintCallable, Category="GraphStructure|ConnectedComponents")
	void Setup(UGraphStructure* MonitorGraph, TSubclassOf<UGraphConnectedComponent> ConnectedCompClass);

	UFUNCTION(BlueprintCallable, Category="GraphStructure|ConnectedComponents")
	TSet<UGraphConnectedComponent*> GetAllUsedConnectedComponents();
};
