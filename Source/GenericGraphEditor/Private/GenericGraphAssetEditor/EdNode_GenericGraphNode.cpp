#include "GenericGraphAssetEditor/EdNode_GenericGraphNode.h"
//-----------------------------------------------------------------------------
// Torbie Begin Change
#include "Algo/Count.h"

#include "GenericGraphAssetEditor/EdNode_GenericGraphEdge.h"
// Torbie End Change
//-----------------------------------------------------------------------------
#include "GenericGraphAssetEditor/EdGraph_GenericGraph.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "EdNode_GenericGraph"

UEdNode_GenericGraphNode::UEdNode_GenericGraphNode()
{
	bCanRenameNode = true;
}

UEdNode_GenericGraphNode::~UEdNode_GenericGraphNode()
{

}

void UEdNode_GenericGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, "MultipleNodes", FName(), TEXT("In"));
	CreatePin(EGPD_Output, "MultipleNodes", FName(), TEXT("Out"));
}

UEdGraph_GenericGraph* UEdNode_GenericGraphNode::GetGenericGraphEdGraph()
{
	return Cast<UEdGraph_GenericGraph>(GetGraph());
}

FText UEdNode_GenericGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (GenericGraphNode == nullptr)
	{
		return Super::GetNodeTitle(TitleType);
	}
	else
	{
		return GenericGraphNode->GetNodeTitle();
	}
}

void UEdNode_GenericGraphNode::PrepareForCopying()
{
	GenericGraphNode->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
}

void UEdNode_GenericGraphNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);

	if (FromPin != nullptr)
	{
		if (GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
	}
}

void UEdNode_GenericGraphNode::SetGenericGraphNode(UGenericGraphNode* InNode)
{
	GenericGraphNode = InNode;
}

FLinearColor UEdNode_GenericGraphNode::GetBackgroundColor() const
{
	return GenericGraphNode == nullptr ? FLinearColor::Black : GenericGraphNode->GetBackgroundColor();
}

UEdGraphPin* UEdNode_GenericGraphNode::GetInputPin() const
{
	return Pins[0];
}

UEdGraphPin* UEdNode_GenericGraphNode::GetOutputPin() const
{
	return Pins[1];
}

//-----------------------------------------------------------------------------
// Torbie Begin Change
int32 UEdNode_GenericGraphNode::GetEdgeCount(UEdNode_GenericGraphNode* ChildNode) const
{
	return Algo::CountIf(
		GetOutputPin()->LinkedTo,
		[ChildNode](const UEdGraphPin* Pin)
		{
			UEdNode_GenericGraphEdge* EdNode_Edge = Cast<UEdNode_GenericGraphEdge>(Pin->GetOwningNode());
			return EdNode_Edge && EdNode_Edge->GetEndNode() == ChildNode;
		});
}

int32 UEdNode_GenericGraphNode::GetEdgeIndex(UEdNode_GenericGraphNode* ChildNode, UEdNode_GenericGraphEdge* EdgeNode) const
{
	int32 idx = 0;
	for (const UEdGraphPin* Pin : GetOutputPin()->LinkedTo)
	{
		UEdNode_GenericGraphEdge* EdNode_Edge = Cast<UEdNode_GenericGraphEdge>(Pin->GetOwningNode());
		if (EdNode_Edge && EdNode_Edge == EdgeNode)
		{
			return idx;
		}

		if (EdNode_Edge && EdNode_Edge->GetEndNode() == ChildNode)
		{
			++idx;
		}
	}

	return idx;
}
// Torbie End Change
//-----------------------------------------------------------------------------

void UEdNode_GenericGraphNode::PostEditUndo()
{
	UEdGraphNode::PostEditUndo();
}

#undef LOCTEXT_NAMESPACE
