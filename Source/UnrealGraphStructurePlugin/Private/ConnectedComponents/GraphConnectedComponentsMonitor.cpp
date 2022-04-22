// Fill out your copyright notice in the Description page of Project Settings.


#include "ConnectedComponents/GraphConnectedComponentsMonitor.h"

UGraphConnectedComponent* UGraphConnectedComponentsMonitor::ConnectedComponent_Spawn()
{
	UGraphConnectedComponent* NewConnectedComponent = NewObject<UGraphConnectedComponent>(this, ConnectedComponentClass);

	NewConnectedComponent->OnCreated();

	return NewConnectedComponent;
}

void UGraphConnectedComponentsMonitor::ConnectedComponent_Destroy(UGraphConnectedComponent* ConnectedComponent)
{
	check(ConnectedComponent != nullptr);
	check(ConnectedComponent->Vertices.IsEmpty());

	ConnectedComponent->OnDestroyed();
}

void UGraphConnectedComponentsMonitor::ConnectedComponent_AddVertex(UGraphConnectedComponent* ConnectedComponent,
                                                                    UGraphStructureVertex* Vertex)
{
	check(ConnectedComponent != nullptr);
	check(Vertex != nullptr);
	check(!ConnectedComponent->Vertices.Contains(Vertex));

	ConnectedComponent->Vertices.Add(Vertex);

	ConnectedComponent->OnVertexAdded(Vertex);
}

void UGraphConnectedComponentsMonitor::ConnectedComponent_RemoveVertex(UGraphConnectedComponent* ConnectedComponent,
                                                                       UGraphStructureVertex* Vertex)
{
	check(ConnectedComponent != nullptr);
	check(Vertex != nullptr);
	check(ConnectedComponent->Vertices.Contains(Vertex));

	ConnectedComponent->Vertices.Remove(Vertex);

	ConnectedComponent->OnVertexRemoved(Vertex);
}

void UGraphConnectedComponentsMonitor::GraphStructure_VertexAdded(UGraphStructureVertex* Vertex)
{
	check(Vertex != nullptr);
	// Vertex shouldn't already be in VerticesComponentsMap
	check(!VerticesComponentsMap.Contains(Vertex));

	// Since freshly spawned vertices never have any connection yet just spawn a new ConnectionComponent, but check to be sure
	check(Vertex->Edges.IsEmpty());

	UGraphConnectedComponent* NewConnectedComponent = ConnectedComponent_Spawn();
	ConnectedComponent_AddVertex(NewConnectedComponent, Vertex);
	check(NewConnectedComponent->Vertices.Num() == 1);

	VerticesComponentsMap.Add(Vertex, NewConnectedComponent);
}

void UGraphConnectedComponentsMonitor::GraphStructure_VertexRemoved(UGraphStructureVertex* Vertex)
{
	check(Vertex != nullptr);
	// Since the RemoveVertex functions in the graph always remove Edges first we don't have to deal with them, but check to be sure
	check(Vertex->Edges.IsEmpty());

	UGraphConnectedComponent* ConnectedComponent = VerticesComponentsMap.FindAndRemoveChecked(Vertex);
	ConnectedComponent_RemoveVertex(ConnectedComponent, Vertex);

	if (ConnectedComponent->Vertices.IsEmpty())
	{
		ConnectedComponent_Destroy(ConnectedComponent);
	}
}

void UGraphConnectedComponentsMonitor::GraphStructure_EdgeAdded(UGraphStructureEdge* Edge)
{
	check(Edge != nullptr);

	UGraphStructureVertex* Source = Edge->Source;
	check(Source != nullptr);
	UGraphStructureVertex* Target = Edge->Target;
	check(Target != nullptr);

	UGraphConnectedComponent* SourceConnectedComponent = VerticesComponentsMap.FindChecked(Source);
	UGraphConnectedComponent* TargetConnectedComponent = VerticesComponentsMap.FindChecked(Target);

	if (SourceConnectedComponent == TargetConnectedComponent)
	{
		// Both sides already part of the same ConnectionComponent, no merging required
		return;
	}
	check(SourceConnectedComponent->Vertices.Intersect(TargetConnectedComponent->Vertices).IsEmpty());

	// Merging required

	UGraphConnectedComponent* KeepConnectedComponent;
	UGraphConnectedComponent* MergeConnectedComponent;
	// Merge smaller component into bigger component
	if (SourceConnectedComponent->Vertices.Num() > TargetConnectedComponent->Vertices.Num())
	{
		KeepConnectedComponent = SourceConnectedComponent;
		MergeConnectedComponent = TargetConnectedComponent;
	}
	else
	{
		KeepConnectedComponent = TargetConnectedComponent;
		MergeConnectedComponent = SourceConnectedComponent;
	}

	// Copy Vertices set and move each vertex to the bigger component
	TSet<UGraphStructureVertex*> MigrateVertices = MergeConnectedComponent->Vertices;
	for (UGraphStructureVertex* MigrateVertex : MigrateVertices)
	{
		ConnectedComponent_RemoveVertex(MergeConnectedComponent, MigrateVertex);
		ConnectedComponent_AddVertex(KeepConnectedComponent, MigrateVertex);
		VerticesComponentsMap.Add(MigrateVertex, KeepConnectedComponent);
	}

	ConnectedComponent_Destroy(MergeConnectedComponent);
}

void UGraphConnectedComponentsMonitor::GraphStructure_EdgeRemoved(UGraphStructureEdge* Edge)
{
	check(Edge != nullptr);

	UGraphStructureVertex* Source = Edge->Source;
	check(Source != nullptr);
	UGraphStructureVertex* Target = Edge->Target;
	check(Target != nullptr);

	UGraphConnectedComponent* AffectedConnectedComponent = VerticesComponentsMap.FindChecked(Source);

	// Both Vertices should be assigned the same ConnectionComponent and should be in the components Vertices list
	check(AffectedConnectedComponent == VerticesComponentsMap.FindChecked(Target));
	check(AffectedConnectedComponent->Vertices.Contains(Source));
	check(AffectedConnectedComponent->Vertices.Contains(Target));

	const TSet<UGraphStructureVertex*> FoundVertices = TSet(Graph->BreadthFirstSearch(Source));
	if (FoundVertices.Contains(Target))
	{
		// Target vertex found, the vertices are still connected someway else, no need to split component
		return;
	}

	// Component needs to be split

	// Determine which side of edge is easier to split off into new component, split off smaller side
	const int32 OriginalCount = AffectedConnectedComponent->Vertices.Num();
	const int32 FoundCount = FoundVertices.Num();

	const TSet<UGraphStructureVertex*> OtherVertices = AffectedConnectedComponent->Vertices.Difference(FoundVertices);
	check(!OtherVertices.Contains(Source));

	TSet<UGraphStructureVertex*> SplitOffVertices = FoundCount > OriginalCount / 2 ? OtherVertices : FoundVertices;

	UGraphConnectedComponent* NewConnectedComponent = ConnectedComponent_Spawn();

	for (UGraphStructureVertex* SplitOffVertex : SplitOffVertices)
	{
		ConnectedComponent_RemoveVertex(AffectedConnectedComponent, SplitOffVertex);
		ConnectedComponent_AddVertex(NewConnectedComponent, SplitOffVertex);
		VerticesComponentsMap.Add(SplitOffVertex, NewConnectedComponent);
	}
}

void UGraphConnectedComponentsMonitor::Setup(UGraphStructure* MonitorGraph, TSubclassOf<UGraphConnectedComponent> ConnectedCompClass)
{
	if (SetupCompleted)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGraphConnectedComponentsMonitor::Setup() called after it already has been setup"));
		return;
	}

	check(Graph == nullptr);
	Graph = MonitorGraph;
	ConnectedComponentClass = ConnectedCompClass;
	if (Graph == nullptr || ConnectedCompClass == nullptr)
	{
		return;
	}

	// Bind delegates
	Graph->OnVertexAdded.AddDynamic(this, &UGraphConnectedComponentsMonitor::GraphStructure_VertexAdded);
	Graph->OnVertexRemoved.AddDynamic(this, &UGraphConnectedComponentsMonitor::GraphStructure_VertexRemoved);
	Graph->OnEdgeAdded.AddDynamic(this, &UGraphConnectedComponentsMonitor::GraphStructure_EdgeAdded);
	Graph->OnEdgeRemoved.AddDynamic(this, &UGraphConnectedComponentsMonitor::GraphStructure_EdgeRemoved);

	// Setup initial connected components

	// Copy Vertices Set
	TSet<UGraphStructureVertex*> RemainingVertices = Graph->GetVertices();

	// Iterate over Vertices, spawn Components until there are no more Vertices left that aren't assigned to a Component
	while (RemainingVertices.Num() > 0)
	{
		// Get first Element via for loop iterator
		for (UGraphStructureVertex* NextVertex : RemainingVertices)
		{
			TSet<UGraphStructureVertex*> FoundVertices = TSet(Graph->BreadthFirstSearch(NextVertex));

			UGraphConnectedComponent* NewConnectedComponent = ConnectedComponent_Spawn();

			// Remove any found vertex from the RemainingVertices Set and add them to the Map so we know what component we assigned it to
			for (UGraphStructureVertex* FoundVertex : FoundVertices)
			{
				RemainingVertices.Remove(FoundVertex);
				ConnectedComponent_AddVertex(NewConnectedComponent, FoundVertex);
				VerticesComponentsMap.Add(FoundVertex, NewConnectedComponent);
			}

			break;
		}
	}

	// Set SetupCompleted so future setup calls will be ignored and logged
	SetupCompleted = true;
}

TSet<UGraphConnectedComponent*> UGraphConnectedComponentsMonitor::GetAllUsedConnectedComponents()
{
	TSet<UGraphConnectedComponent*> AllUsedConnectedComponents;
	for (auto Pair : VerticesComponentsMap)
	{
		AllUsedConnectedComponents.Add(Pair.Value);
	}
	return AllUsedConnectedComponents;
}
