// Fill out your copyright notice in the Description page of Project Settings.


#include "GraphStructure.h"

UGraphStructure::UGraphStructure()
{
}

bool UGraphStructure::AddVertex(UGraphStructureVertex* Vertex)
{
	if (ensure(Vertex != nullptr))
	{
		bool AlreadyInSet = false;
		Vertices.Add(Vertex, &AlreadyInSet);
		return AlreadyInSet;
	}
	return false;
}

UGraphStructureVertex* UGraphStructure::AddDefaultVertex()
{
	UGraphStructureVertex* Vertex = NewObject<UGraphStructureVertex>();
	AddVertex(Vertex);
	return Vertex;
}

bool UGraphStructure::AddEdge(UGraphStructureEdge* Edge)
{
	if (ensure(Edge != nullptr) && ensure(Edge->Source != nullptr) && ensure(Edge->Target != nullptr))
	{
		bool AlreadyInSet = false;
		Edges.Add(Edge, &AlreadyInSet);

		Edge->Source->Edges.Add(Edge);
		Edge->Target->Edges.Add(Edge);

		return AlreadyInSet;
	}
	return false;
}

UGraphStructureEdge* UGraphStructure::AddDefaultEdgeBetween(UGraphStructureVertex* SourceVertex, UGraphStructureVertex* TargetVertex)
{
	UGraphStructureEdge* Edge = NewObject<UGraphStructureEdge>();
	Edge->Source = SourceVertex;
	Edge->Target = TargetVertex;

	AddEdge(Edge);
	return Edge;
}

bool UGraphStructure::RemoveVertex(UGraphStructureVertex* Vertex)
{
	if (!Vertices.Contains(Vertex))
	{
		return false;
	}

	if (ensure(Vertex != nullptr))
	{
		// One may make this quicker by not copying the edges first but iterating directly and adding an internal remove edge
		// function that can skip the removal of the edge from the Source or Target Edges TSet.

		// Copy TSet first to avoid modifying Set while iterating
		for (UGraphStructureEdge* Edge : Edges.Array())
		{
			// If RemoveEdge is false halt since there is something wrong with our graph
			verify(RemoveEdge(Edge));
		}

		verify(Vertices.Remove(Vertex) > 0);
		return true;
	}
	return false;
}

bool UGraphStructure::RemoveEdge(UGraphStructureEdge* Edge)
{
	if (!Edges.Contains(Edge))
	{
		return false;
	}

	if (ensure(Edge != nullptr) && ensure(Edge->Source != nullptr) && ensure(Edge->Target != nullptr))
	{
		verify(Edge->Source->Edges.Remove(Edge));
		verify(Edge->Target->Edges.Remove(Edge));

		verify(Edges.Remove(Edge));

		return true;
	}
	return false;
}

UGraphStructureEdge* UGraphStructure::GetEdgeBetween(UGraphStructureVertex* SourceVertex, UGraphStructureVertex* TargetVertex)
{
	for (UGraphStructureEdge* Edge : GetAllEdgesBetween(SourceVertex, TargetVertex))
	{
		return Edge;
	}
	return nullptr;
}

TSet<UGraphStructureEdge*> UGraphStructure::GetAllEdgesBetween(UGraphStructureVertex* SourceVertex, UGraphStructureVertex* TargetVertex)
{
	if (ensure(SourceVertex != nullptr) && ensure(TargetVertex != nullptr))
	{
		return SourceVertex->Edges.Intersect(TargetVertex->Edges);
	}
	TSet<UGraphStructureEdge*> EmptySet;
	return EmptySet;
}

bool UGraphStructure::HasEdgeBetween(UGraphStructureVertex* SourceVertex, UGraphStructureVertex* TargetVertex)
{
	return GetEdgeBetween(SourceVertex, TargetVertex) != nullptr;
}
