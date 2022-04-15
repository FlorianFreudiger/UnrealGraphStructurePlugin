// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GraphStructureEdge.h"
#include "GraphStructureVertex.h"
#include "UObject/NoExportTypes.h"
#include "GraphStructure.generated.h"

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

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Construction")
	bool AddVertex(UGraphStructureVertex* Vertex);

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Construction")
	UGraphStructureVertex* AddDefaultVertex();

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Construction")
	bool AddEdge(UGraphStructureEdge* Edge);

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Construction")
	UGraphStructureEdge* AddDefaultEdgeBetween(UGraphStructureVertex* SourceVertex, UGraphStructureVertex* TargetVertex);

	// Destruction

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Destruction")
	bool RemoveVertex(UGraphStructureVertex* Vertex);

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Destruction")
	bool RemoveEdge(UGraphStructureEdge* Edge);

	// Queries

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Query")
	UGraphStructureEdge* GetEdgeBetween(UGraphStructureVertex* SourceVertex, UGraphStructureVertex* TargetVertex);

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Query")
	TSet<UGraphStructureEdge*> GetAllEdgesBetween(UGraphStructureVertex* SourceVertex, UGraphStructureVertex* TargetVertex);

	UFUNCTION(BlueprintCallable, Category="GraphStructure|Query")
	bool HasEdgeBetween(UGraphStructureVertex* SourceVertex, UGraphStructureVertex* TargetVertex);
};
