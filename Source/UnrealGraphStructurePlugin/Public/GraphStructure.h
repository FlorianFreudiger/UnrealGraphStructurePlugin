// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GraphStructureEdge.h"
#include "GraphStructureVertex.h"
#include "UObject/NoExportTypes.h"
#include "GraphStructure.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGraphStructure_OnVertexAdded_Signature, UGraphStructureVertex*, Vertex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGraphStructure_OnVertexRemoved_Signature, UGraphStructureVertex*, Vertex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGraphStructure_OnEdgeAdded_Signature, UGraphStructureEdge*, Edge);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGraphStructure_OnEdgeRemoved_Signature, UGraphStructureEdge*, Edge);

/**
 * 
 */
UCLASS(BlueprintType)
class UNREALGRAPHSTRUCTUREPLUGIN_API UGraphStructure : public UObject
{
	GENERATED_BODY()

public:
	UGraphStructure();

private:
	UPROPERTY()
	TSet<UGraphStructureVertex*> Vertices;

	UPROPERTY()
	TSet<UGraphStructureEdge*> Edges;

public:
	// Construction

	UPROPERTY(BlueprintAssignable)
	FGraphStructure_OnVertexAdded_Signature GraphStructure_OnVertexAdded;

	UPROPERTY(BlueprintAssignable)
	FGraphStructure_OnEdgeAdded_Signature GraphStructure_OnEdgeAdded;

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Construction")
	bool AddVertex(UGraphStructureVertex* Vertex);

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Construction")
	UGraphStructureVertex* AddDefaultVertex();

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Construction")
	bool AddEdge(UGraphStructureEdge* Edge);

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Construction")
	UGraphStructureEdge* AddDefaultEdgeBetween(UGraphStructureVertex* SourceVertex, UGraphStructureVertex* TargetVertex);

	// Destruction

	UPROPERTY(BlueprintAssignable)
	FGraphStructure_OnVertexRemoved_Signature GraphStructure_OnVertexRemoved;

	UPROPERTY(BlueprintAssignable)
	FGraphStructure_OnEdgeRemoved_Signature GraphStructure_OnEdgeRemoved;

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Destruction")
	bool RemoveVertex(UGraphStructureVertex* Vertex);

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Destruction")
	bool RemoveEdge(UGraphStructureEdge* Edge);

	// Queries - Edges directly between 2 vertices

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Query")
	UGraphStructureEdge* GetEdgeBetween(UGraphStructureVertex* SourceVertex, UGraphStructureVertex* TargetVertex);

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Query")
	TSet<UGraphStructureEdge*> GetAllEdgesBetween(UGraphStructureVertex* SourceVertex, UGraphStructureVertex* TargetVertex);

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Query")
	bool HasEdgeBetween(UGraphStructureVertex* SourceVertex, UGraphStructureVertex* TargetVertex);

	// Queries - Traversal

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Query")
	TArray<UGraphStructureVertex*> BreadthFirstSearch(UGraphStructureVertex* RootVertex);

	// Debugging

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Debugging")
	FString ExportGraphvizDotString();
};
