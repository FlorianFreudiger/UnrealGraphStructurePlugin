// Fill out your copyright notice in the Description page of Project Settings.


#include "GraphStructureEdge.h"

#include "GraphStructureVertex.h"

UGraphStructureEdge::UGraphStructureEdge()
{
}

FString UGraphStructureEdge::GetGraphvizDotRepresentation()
{
	return Source->GetGraphvizDotNodeName() + FString(TEXT("--")) + Target->GetGraphvizDotNodeName();
}
