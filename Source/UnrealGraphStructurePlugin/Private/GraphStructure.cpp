// Fill out your copyright notice in the Description page of Project Settings.


#include "GraphStructure.h"

#include "Kismet/KismetSystemLibrary.h"

UGraphStructure::UGraphStructure()
{
}

TSet<UGraphStructureVertex*> UGraphStructure::GetVertices()
{
	return Vertices;
}

TSet<UGraphStructureEdge*> UGraphStructure::GetEdges()
{
	return Edges;
}

bool UGraphStructure::AddVertex(UGraphStructureVertex* Vertex)
{
	if (ensure(Vertex != nullptr))
	{
		bool AlreadyInSet = false;
		Vertices.Add(Vertex, &AlreadyInSet);
		if (AlreadyInSet)
		{
			return false;
		}

		OnVertexAdded.Broadcast(Vertex);
		return true;
	}
	return false;
}

UGraphStructureVertex* UGraphStructure::AddDefaultVertex()
{
	UGraphStructureVertex* Vertex = NewObject<UGraphStructureVertex>();

	// Since we have just created this vertex the addition should not fail
	verify(AddVertex(Vertex));

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
		if (AlreadyInSet)
		{
			return false;
		}

		OnEdgeAdded.Broadcast(Edge);
		return true;
	}
	return false;
}

UGraphStructureEdge* UGraphStructure::AddDefaultEdgeBetween(UGraphStructureVertex* SourceVertex, UGraphStructureVertex* TargetVertex)
{
	UGraphStructureEdge* Edge = NewObject<UGraphStructureEdge>();
	Edge->Source = SourceVertex;
	Edge->Target = TargetVertex;

	// Since we have just created this edge the addition should not fail
	verify(AddEdge(Edge));

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
		// Copy TSet first to avoid modifying Set while iterating
		for (UGraphStructureEdge* Edge : Vertex->Edges.Array())
		{
			// If RemoveEdge is false halt since there is something wrong with our graph
			verify(RemoveEdge(Edge));
		}

		verify(Vertices.Remove(Vertex) > 0);

		OnVertexRemoved.Broadcast(Vertex);

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
		// Remove edge from both source and target vertices Edges TSet, if edge is self-loop ignore second removal result
		verify(Edge->Source->Edges.Remove(Edge));
		verify(Edge->Target->Edges.Remove(Edge) || Edge->Source == Edge->Target);

		verify(Edges.Remove(Edge));

		OnEdgeRemoved.Broadcast(Edge);

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

TArray<UGraphStructureVertex*> UGraphStructure::BreadthFirstSearch(UGraphStructureVertex* RootVertex)
{
	if (RootVertex == nullptr)
	{
		TArray<UGraphStructureVertex*> EmptyArray;
		return EmptyArray;
	}

	TArray<UGraphStructureVertex*> Nodes;
	TSet<UGraphStructureVertex*> DiscoveredNodes;

	Nodes.Add(RootVertex);
	// Maintain additional Set of discovered nodes so lookups are (hopefully) O(1) instead of O(N)
	DiscoveredNodes.Add(RootVertex);

	for (int i = 0; i < Nodes.Num(); ++i)
	{
		UGraphStructureVertex* NextNode = Nodes[i];
		check(NextNode != nullptr);

		for (const UGraphStructureEdge* Edge : NextNode->Edges)
		{
			check(Edge != nullptr);

			UGraphStructureVertex* Source = Edge->Source;
			UGraphStructureVertex* Target = Edge->Target;

			check(Source != nullptr);
			check(Target != nullptr);

			if (!DiscoveredNodes.Contains(Source))
			{
				Nodes.Add(Source);
				DiscoveredNodes.Add(Source);
			}

			if (!DiscoveredNodes.Contains(Target))
			{
				Nodes.Add(Target);
				DiscoveredNodes.Add(Target);
			}
		}
	}

	return Nodes;
}

FString UGraphStructure::ExportGraphvizDotString()
{
	FString DotString = FString(TEXT("graph "));
	DotString += UKismetSystemLibrary::GetDisplayName(this);
	DotString += " {\n";

	for (UGraphStructureVertex* Vertex : Vertices)
	{
		DotString += Vertex->GetGraphvizDotRepresentation() + ";\n";
	}
	for (UGraphStructureEdge* Edge : Edges)
	{
		DotString += Edge->GetGraphvizDotRepresentation() + ";\n";
	}
	DotString += "}";

	return DotString;
}
