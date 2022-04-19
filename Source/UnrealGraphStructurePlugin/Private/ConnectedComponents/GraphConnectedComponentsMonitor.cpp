// Fill out your copyright notice in the Description page of Project Settings.


#include "ConnectedComponents/GraphConnectedComponentsMonitor.h"

void UGraphConnectedComponentsMonitor::GraphStructure_VertexAdded(UGraphStructureVertex* Vertex)
{
	check(Vertex != nullptr);
	// Vertex shouldn't already be in VerticesComponentsMap
	check(!VerticesComponentsMap.Contains(Vertex));

	// Since freshly spawned vertices never have any connection yet just spawn a new ConnectionComponent, but check to be sure
	check(Vertex->Edges.IsEmpty());

	UGraphConnectedComponent* NewConnectedComponent = NewObject<UGraphConnectedComponent>();
	OnConnectedComponentCreated.Broadcast(NewConnectedComponent);

	NewConnectedComponent->AddVertex(Vertex);
	check(NewConnectedComponent->Vertices.Num() == 1);

	VerticesComponentsMap.Add(Vertex, NewConnectedComponent);
}

void UGraphConnectedComponentsMonitor::GraphStructure_VertexRemoved(UGraphStructureVertex* Vertex)
{
	check(Vertex != nullptr);
	// Since the RemoveVertex functions in the graph always remove Edges first we don't have to deal with them, but check to be sure
	check(Vertex->Edges.IsEmpty());

	UGraphConnectedComponent* ConnectedComponent = VerticesComponentsMap.FindAndRemoveChecked(Vertex);
	ConnectedComponent->RemoveVertex(Vertex);

	if (ConnectedComponent->Vertices.IsEmpty())
	{
		OnConnectedComponentRemoved.Broadcast(ConnectedComponent);
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
		MergeConnectedComponent->RemoveVertex(MigrateVertex);
		KeepConnectedComponent->AddVertex(MigrateVertex);
		VerticesComponentsMap.Add(MigrateVertex, KeepConnectedComponent);
	}
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

	UGraphConnectedComponent* NewConnectedComponent = NewObject<UGraphConnectedComponent>();

	for (UGraphStructureVertex* SplitOffVertex : SplitOffVertices)
	{
		AffectedConnectedComponent->RemoveVertex(SplitOffVertex);
		NewConnectedComponent->AddVertex(SplitOffVertex);
		VerticesComponentsMap.Add(SplitOffVertex, NewConnectedComponent);
	}
}

void UGraphConnectedComponentsMonitor::Setup(UGraphStructure* MonitorGraph)
{
	if (SetupCompleted)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGraphConnectedComponentsMonitor::Setup() called after it already has been setup"));
		return;
	}

	check(Graph == nullptr);
	Graph = MonitorGraph;
	if (Graph == nullptr)
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

			UGraphConnectedComponent* NewConnectedComponent = NewObject<UGraphConnectedComponent>();
			OnConnectedComponentCreated.Broadcast(NewConnectedComponent);

			// Remove any found vertex from the RemainingVertices Set and add them to the Map so we know what component we assigned it to
			for (UGraphStructureVertex* FoundVertex : FoundVertices)
			{
				RemainingVertices.Remove(FoundVertex);
				NewConnectedComponent->AddVertex(FoundVertex);
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
