// Fill out your copyright notice in the Description page of Project Settings.


#include "ConnectedComponents/GraphConnectedComponent.h"

void UGraphConnectedComponent::AddVertex(UGraphStructureVertex* Vertex)
{
	check(!Vertices.Contains(Vertex));
	Vertices.Add(Vertex);
	OnVertexAdded.Broadcast(Vertex);
}

void UGraphConnectedComponent::RemoveVertex(UGraphStructureVertex* Vertex)
{
	check(Vertices.Contains(Vertex));
	Vertices.Remove(Vertex);
	OnVertexRemoved.Broadcast(Vertex);
}

TSet<UGraphStructureVertex*> UGraphConnectedComponent::GetVertices()
{
	return Vertices;
}
