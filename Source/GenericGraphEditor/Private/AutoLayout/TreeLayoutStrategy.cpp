#include "AutoLayout/TreeLayoutStrategy.h"
#include "GenericGraphEditorPCH.h"
#include "GenericGraphAssetEditor/SEdNode_GenericGraphNode.h"

UTreeLayoutStrategy::UTreeLayoutStrategy()
{
}

UTreeLayoutStrategy::~UTreeLayoutStrategy()
{

}

void UTreeLayoutStrategy::Layout(UEdGraph* _EdGraph)
{
	EdGraph = Cast<UEdGraph_GenericGraph>(_EdGraph);
	check(EdGraph != nullptr);

	EdGraph->RebuildGenericGraph();
	Graph = EdGraph->GetGenericGraph();
	check(Graph != nullptr);

	bool bFirstPassOnly = false;

	if (Settings != nullptr)
	{
		OptimalDistance = Settings->OptimalDistance;
		MaxIteration = Settings->MaxIteration;
		bFirstPassOnly = Settings->bFirstPassOnly;
	}

	FVector2D Anchor(0.f, 0.f);
	for (int32 i = 0; i < Graph->RootNodes.Num(); ++i)
	{
		UGenericGraphNode* RootNode = Graph->RootNodes[i];
		InitPass(RootNode, Anchor);

		if (!bFirstPassOnly)
		{
			for (int32 j = 0; j < MaxIteration; ++j)
			{
				bool HasConflict = ResolveConflictPass(RootNode);
				if (!HasConflict)
				{
					break;
				}
			}
		}
	}

	for (int32 i = 0; i < Graph->RootNodes.Num(); ++i)
	{
		for (int32 j = 0; j < i; ++j)
		{
			ResolveConflict(Graph->RootNodes[j], Graph->RootNodes[i]);
		}
	}
}

void UTreeLayoutStrategy::InitPass(UGenericGraphNode* RootNode, const FVector2D& Anchor)
{
	UEdNode_GenericGraphNode* EdNode_RootNode = EdGraph->NodeMap[RootNode];

	FVector2D ChildAnchor(FVector2D(0.f, GetNodeHeight(EdNode_RootNode) + OptimalDistance + Anchor.Y));
	for (int32 i = 0; i < RootNode->ChildrenNodes.Num(); ++i)
	{
		//-----------------------------------------------------------------------------
		// Torbie Begin Change
		UGenericGraphNode* Child = RootNode->ChildrenNodes[i].Node;
		// Torbie End Change
		//-----------------------------------------------------------------------------
		UEdNode_GenericGraphNode* EdNode_ChildNode = EdGraph->NodeMap[Child];
		if (i > 0)
		{
			//-----------------------------------------------------------------------------
			// Torbie Begin Change
			UGenericGraphNode* PreChild = RootNode->ChildrenNodes[i - 1].Node;
			// Torbie End Change
			//-----------------------------------------------------------------------------
			UEdNode_GenericGraphNode* EdNode_PreChildNode = EdGraph->NodeMap[PreChild];
			ChildAnchor.X += OptimalDistance + GetNodeWidth(EdNode_PreChildNode) / 2;
		}
		ChildAnchor.X += GetNodeWidth(EdNode_ChildNode) / 2;
		InitPass(Child, ChildAnchor);
	}
	
	float NodeWidth = GetNodeWidth(EdNode_RootNode);

	EdNode_RootNode->NodePosY = Anchor.Y;
	if (RootNode->ChildrenNodes.Num() == 0)
	{
		EdNode_RootNode->NodePosX = Anchor.X - NodeWidth / 2;
	}
	else
	{
		UpdateParentNodePosition(RootNode);
	}
}

bool UTreeLayoutStrategy::ResolveConflictPass(UGenericGraphNode* Node)
{
	bool HasConflict = false;
	for (int32 i = 0; i < Node->ChildrenNodes.Num(); ++i)
	{
		//-----------------------------------------------------------------------------
		// Torbie Begin Change
		UGenericGraphNode* Child = Node->ChildrenNodes[i].Node;
		// Torbie End Change
		//-----------------------------------------------------------------------------
		if (ResolveConflictPass(Child))
		{
			HasConflict = true;
		}
	}

	for (int32 i = 0; i < Node->ParentNodes.Num(); ++i)
	{
		//-----------------------------------------------------------------------------
		// Torbie Begin Change
		UGenericGraphNode* ParentNode = Node->ParentNodes[i].Node;
		// Torbie End Change
		//-----------------------------------------------------------------------------
		for (int32 j = 0; j < ParentNode->ChildrenNodes.Num(); ++j)
		{
			//-----------------------------------------------------------------------------
			// Torbie Begin Change
			UGenericGraphNode* LeftSibling = ParentNode->ChildrenNodes[j].Node;
			// Torbie End Change
			//-----------------------------------------------------------------------------
			if (LeftSibling == Node)
				break;
			if (ResolveConflict(LeftSibling, Node))
			{
				HasConflict = true;
			}
		}
	}

	return HasConflict;
}

bool UTreeLayoutStrategy::ResolveConflict(UGenericGraphNode* LRoot, UGenericGraphNode* RRoot)
{
	TArray<UEdNode_GenericGraphNode*> RightContour, LeftContour;

	GetRightContour(LRoot, 0, RightContour);
	GetLeftContour(RRoot, 0, LeftContour);

	int32 MaxOverlapDistance = 0;
	int32 Num = FMath::Min(LeftContour.Num(), RightContour.Num());
	for (int32 i = 0; i < Num; ++i)
	{
		if (RightContour.Contains(LeftContour[i]) || LeftContour.Contains(RightContour[i]))
			break;

		int32 RightBound = RightContour[i]->NodePosX + GetNodeWidth(RightContour[i]);
		int32 LeftBound = LeftContour[i]->NodePosX;
		int32 Distance = RightBound + OptimalDistance - LeftBound;
		if (Distance > MaxOverlapDistance)
		{
			MaxOverlapDistance = Distance;
		}
	}

	if (MaxOverlapDistance > 0)
	{
		ShiftSubTree(RRoot, FVector2D(MaxOverlapDistance, 0.f));

		//-----------------------------------------------------------------------------
		// Torbie Begin Change
		TArray<FGenericGraphConnection> ParentNodes = RRoot->ParentNodes;
		TArray<FGenericGraphConnection> NextParentNodes;
		while (ParentNodes.Num() != 0)
		{
			for (int32 i = 0; i < ParentNodes.Num(); ++i)
			{
				UpdateParentNodePosition(ParentNodes[i].Node);

				NextParentNodes.Append(ParentNodes[i].Node->ParentNodes);
			}

			ParentNodes = NextParentNodes;
			NextParentNodes.Reset();
		}
		// Torbie End Change
		//-----------------------------------------------------------------------------

		return true;
	}
	else
	{
		return false;
	}
}

void UTreeLayoutStrategy::GetLeftContour(UGenericGraphNode* RootNode, int32 Level, TArray<UEdNode_GenericGraphNode*>& Contour)
{
	UEdNode_GenericGraphNode* EdNode_Node = EdGraph->NodeMap[RootNode];
	if (Level >= Contour.Num())
	{
		Contour.Add(EdNode_Node);
	}
	else if (EdNode_Node->NodePosX < Contour[Level]->NodePosX)
	{
		Contour[Level] = EdNode_Node;
	}

	for (int32 i = 0; i < RootNode->ChildrenNodes.Num(); ++i)
	{
		//-----------------------------------------------------------------------------
		// Torbie Begin Change
		GetLeftContour(RootNode->ChildrenNodes[i].Node, Level + 1, Contour);
		// Torbie End Change
		//-----------------------------------------------------------------------------
	}
}

void UTreeLayoutStrategy::GetRightContour(UGenericGraphNode* RootNode, int32 Level, TArray<UEdNode_GenericGraphNode*>& Contour)
{
	UEdNode_GenericGraphNode* EdNode_Node = EdGraph->NodeMap[RootNode];
	if (Level >= Contour.Num())
	{
		Contour.Add(EdNode_Node);
	}
	else if (EdNode_Node->NodePosX + GetNodeWidth(EdNode_Node) > Contour[Level]->NodePosX + GetNodeWidth(Contour[Level]))
	{
		Contour[Level] = EdNode_Node;
	}

	for (int32 i = 0; i < RootNode->ChildrenNodes.Num(); ++i)
	{
		//-----------------------------------------------------------------------------
		// Torbie Begin Change
		GetRightContour(RootNode->ChildrenNodes[i].Node, Level + 1, Contour);
		// Torbie End Change
		//-----------------------------------------------------------------------------
	}
}

void UTreeLayoutStrategy::ShiftSubTree(UGenericGraphNode* RootNode, const FVector2D& Offset)
{
	UEdNode_GenericGraphNode* EdNode_Node = EdGraph->NodeMap[RootNode];
	EdNode_Node->NodePosX += Offset.X;
	EdNode_Node->NodePosY += Offset.Y;

	for (int32 i = 0; i < RootNode->ChildrenNodes.Num(); ++i)
	{
		//-----------------------------------------------------------------------------
		// Torbie Begin Change
		FGenericGraphConnection Child = RootNode->ChildrenNodes[i];

		if (Child.Node->ParentNodes[0].Node == RootNode)
		{
			ShiftSubTree(RootNode->ChildrenNodes[i].Node, Offset);
		}
		// Torbie End Change
		//-----------------------------------------------------------------------------
	}
}

void UTreeLayoutStrategy::UpdateParentNodePosition(UGenericGraphNode* ParentNode)
{
	//-----------------------------------------------------------------------------
	// Torbie Begin Change
	UEdNode_GenericGraphNode* EdNode_ParentNode = EdGraph->NodeMap[ParentNode];
	if (ParentNode->ChildrenNodes.Num() % 2 == 0)
	{
		UEdNode_GenericGraphNode* FirstChild = EdGraph->NodeMap[ParentNode->ChildrenNodes[0].Node];
		UEdNode_GenericGraphNode* LastChild = EdGraph->NodeMap[ParentNode->ChildrenNodes.Last().Node];
		float LeftBound = FirstChild->NodePosX;
		float RightBound = LastChild->NodePosX + GetNodeWidth(LastChild);
		EdNode_ParentNode->NodePosX = (LeftBound + RightBound) / 2 - GetNodeWidth(EdNode_ParentNode) / 2;
	}
	else
	{
		UEdNode_GenericGraphNode* MidChild = EdGraph->NodeMap[ParentNode->ChildrenNodes[ParentNode->ChildrenNodes.Num() / 2].Node];
		EdNode_ParentNode->NodePosX = MidChild->NodePosX + GetNodeWidth(MidChild) / 2 - GetNodeWidth(EdNode_ParentNode) / 2;
	}
	// Torbie End Change
	//-----------------------------------------------------------------------------
}
