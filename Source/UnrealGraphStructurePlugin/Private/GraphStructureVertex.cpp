// Fill out your copyright notice in the Description page of Project Settings.


#include "GraphStructureVertex.h"

#include "Kismet/KismetSystemLibrary.h"

UGraphStructureVertex::UGraphStructureVertex()
{
}

FString UGraphStructureVertex::GetGraphvizDotNodeName()
{
	return UKismetSystemLibrary::GetDisplayName(this);
}

FString UGraphStructureVertex::GetGraphvizDotRepresentation()
{
	return GetGraphvizDotNodeName();
}
