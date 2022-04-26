// Fill out your copyright notice in the Description page of Project Settings.


#include "GraphStructure.h"

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

TSet<UGraphStructureVertex*> UGraphStructure::FindAllConnectedVertices(UGraphStructureVertex* RootVertex)
{
	if (RootVertex == nullptr)
	{
		TSet<UGraphStructureVertex*> EmptySet;
		return EmptySet;
	}

	// Use Breadth-First-Search to find all connected vertices

	TSet<UGraphStructureVertex*> DiscoveredNodes;
	TQueue<UGraphStructureVertex*> Queue;

	DiscoveredNodes.Add(RootVertex);
	Queue.Enqueue(RootVertex);

	while (!Queue.IsEmpty())
	{
		UGraphStructureVertex* NextNode;
		verify(Queue.Dequeue(NextNode));
		check(NextNode != nullptr);

		for (const UGraphStructureEdge* Edge : NextNode->Edges)
		{
			check(Edge != nullptr);

			auto ProcessVertex = [&](UGraphStructureVertex* Vertex)
			{
				check(Vertex != nullptr);
				if (!DiscoveredNodes.Contains(Vertex))
				{
					check(Vertex != NextNode);
					DiscoveredNodes.Add(Vertex);
					Queue.Enqueue(Vertex);
				}
			};

			ProcessVertex(Edge->Source);
			ProcessVertex(Edge->Target);
		}
	}

	return DiscoveredNodes;
}

bool UGraphStructure::BfsShortestPath(UGraphStructureVertex* SourceVertex, UGraphStructureVertex* TargetVertex,
                                      TArray<UGraphStructureVertex*>& ShortestPath)
{
	check(ShortestPath.IsEmpty());
	if (SourceVertex == nullptr || TargetVertex == nullptr)
	{
		return false;
	}

	TMap<UGraphStructureVertex*, UGraphStructureVertex*> ParentMap;
	TQueue<UGraphStructureVertex*> Queue;

	ParentMap.Add(SourceVertex, nullptr);
	Queue.Enqueue(SourceVertex);

	while (!Queue.IsEmpty())
	{
		UGraphStructureVertex* NextNode;
		verify(Queue.Dequeue(NextNode));
		check(NextNode != nullptr);

		for (const UGraphStructureEdge* Edge : NextNode->Edges)
		{
			check(Edge != nullptr);

			auto ProcessVertex = [&](UGraphStructureVertex* Vertex) -> bool
			{
				check(Vertex != nullptr);
				if (!ParentMap.Contains(Vertex))
				{
					check(Vertex != NextNode);
					ParentMap.Add(Vertex, NextNode);

					if (Vertex == TargetVertex)
					{
						// We can stop mapping ParentNodes as soon as we found the TargetNode
						Queue.Empty();
						return true;
					}

					Queue.Enqueue(Vertex);
				}
				return false;
			};

			// Only process Target if Source is not found to be TargetVertex
			if (!ProcessVertex(Edge->Source))
			{
				ProcessVertex(Edge->Target);
			}
		}
	}

	// If ParentMap doesn't contain TargetVertex then there is no path between SourceVertex and TargetVertex
	if (!ParentMap.Contains(TargetVertex))
	{
		return false;
	}

	// Backtrack ParentMap to find path
	UGraphStructureVertex* CurrentVertex = TargetVertex;
	while (CurrentVertex != nullptr)
	{
		ShortestPath.Add(CurrentVertex);

		check(CurrentVertex != ParentMap.FindChecked(CurrentVertex));
		CurrentVertex = ParentMap.FindChecked(CurrentVertex);
	}

	// Reverse path since the last vertices of the path have been inserted first, we could optimize this by running BFS in reverse,
	// but that would decrease code readability
	Algo::Reverse(ShortestPath);

	return true;
}

FString UGraphStructure::ExportGraphvizDotString(FString Name)
{
	auto ConvertMap = [](TMap<FString, FString> Map) -> FString
	{
		if (Map.IsEmpty())
		{
			return "";
		}

		FString MapString = FString(TEXT("["));
		for (auto Element : Map)
		{
			MapString += "\"" + Element.Key + "\"=\"" + Element.Value + "\" ";
		}
		MapString += "]";
		return MapString;
	};

	FString DotString = FString(TEXT("graph ")) + Name + "{\n";

	TMap<UGraphStructureVertex*, int32> VerticesIds;
	int32 i = 0;
	for (UGraphStructureVertex* Vertex : Vertices)
	{
		VerticesIds.Add(Vertex, i);
		DotString += FString::FromInt(i) + ConvertMap(Vertex->GetGraphvizDotAttributes()) + ";\n";
		++i;
	}
	for (UGraphStructureEdge* Edge : Edges)
	{
		const int32 SourceId = VerticesIds.FindChecked(Edge->Source);
		const int32 TargetId = VerticesIds.FindChecked(Edge->Target);
		DotString += FString::FromInt(SourceId) + "--" + FString::FromInt(TargetId) + ConvertMap(Edge->GetGraphvizDotAttributes()) + ";\n";
	}
	DotString += "}";

	return DotString;
}
