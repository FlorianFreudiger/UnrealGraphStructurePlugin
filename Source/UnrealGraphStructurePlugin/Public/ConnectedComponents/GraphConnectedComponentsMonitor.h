// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GraphConnectedComponent.h"
#include "GraphStructure.h"
#include "UObject/NoExportTypes.h"
#include "GraphConnectedComponentsMonitor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGraphConnectedComponentMonitor_OnConnectedComponentCreated_Signature,
                                            UGraphConnectedComponent*, ConnectedComponent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGraphConnectedComponentMonitor_OnConnectedComponentRemoved_Signature,
                                            UGraphConnectedComponent*, ConnectedComponent);

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
	void Setup(UGraphStructure* MonitorGraph);

	UFUNCTION(BlueprintCallable, Category="GraphStructure|ConnectedComponents")
	TSet<UGraphConnectedComponent*> GetAllUsedConnectedComponents();

	// Delegates

	UPROPERTY(BlueprintAssignable)
	FGraphConnectedComponentMonitor_OnConnectedComponentCreated_Signature OnConnectedComponentCreated;

	UPROPERTY(BlueprintAssignable)
	FGraphConnectedComponentMonitor_OnConnectedComponentRemoved_Signature OnConnectedComponentRemoved;
};
