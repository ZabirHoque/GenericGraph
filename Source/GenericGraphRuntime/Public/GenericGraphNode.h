#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GenericGraphNode.generated.h"

class UGenericGraph;
class UGenericGraphEdge;
//-----------------------------------------------------------------------------
// Torbie Begin Change
class UGenericGraphNode;
// Torbie End Change
//-----------------------------------------------------------------------------

UENUM(BlueprintType)
enum class ENodeLimit : uint8
{
	Unlimited,
    Limited
};

//-----------------------------------------------------------------------------
// Torbie Begin Change
USTRUCT(BlueprintType)
struct FGenericGraphConnection
{
	GENERATED_BODY()

	FGenericGraphConnection(
		) = default;

	FGenericGraphConnection(
		UGenericGraphNode* InNode,
		UGenericGraphEdge* InEdge 
		)
		: Node(InNode)
		  , Edge(InEdge)
	{
	}
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GenericGraphNode")
	UGenericGraphNode* Node = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GenericGraphNode")
	UGenericGraphEdge* Edge = nullptr;

	friend bool operator==(const FGenericGraphConnection& a, const FGenericGraphConnection& b)
	{
		return a.Node == b.Node && a.Edge == b.Edge;
	}
};
// Torbie End Change
//-----------------------------------------------------------------------------


UCLASS(Blueprintable)
class GENERICGRAPHRUNTIME_API UGenericGraphNode : public UObject
{
	GENERATED_BODY()

public:
	UGenericGraphNode();

	UPROPERTY(VisibleDefaultsOnly, Category = "GenericGraphNode")
	UGenericGraph* Graph;

	//-----------------------------------------------------------------------------
	// Torbie Begin Change
	UPROPERTY(BlueprintReadOnly, Category = "GenericGraphNode")
	TArray<FGenericGraphConnection> ParentNodes;

	UPROPERTY(BlueprintReadOnly, Category = "GenericGraphNode")
	TArray<FGenericGraphConnection> ChildrenNodes;
	// Torbie End Change
	//-----------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "GenericGraphNode")
	bool IsLeafNode() const;

	UFUNCTION(BlueprintCallable, Category = "GenericGraphNode")
	UGenericGraph* GetGraph() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericGraphNode")
	FText GetDescription() const;
	virtual FText GetDescription_Implementation() const;

	//////////////////////////////////////////////////////////////////////////
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "GenericGraphNode_Editor")
	FText NodeTitle;

	UPROPERTY(VisibleDefaultsOnly, Category = "GenericGraphNode_Editor")
	TSubclassOf<UGenericGraph> CompatibleGraphType;

	UPROPERTY(EditDefaultsOnly, Category = "GenericGraphNode_Editor")
	FLinearColor BackgroundColor;

	UPROPERTY(EditDefaultsOnly, Category = "GenericGraphNode_Editor")
	FText ContextMenuName;

	//-----------------------------------------------------------------------------
	// Torbie Begin Change
	UPROPERTY(EditDefaultsOnly, Category = "GenericGraphNode_Editor", meta = (ClampMin = "0",EditCondition = "ParentLimitType == ENodeLimit::Limited", EditConditionHides))
	int32 ParentLimit = 0;

	UPROPERTY(EditDefaultsOnly, Category = "GenericGraphNode_Editor", meta = (ClampMin = "0",EditCondition = "ChildrenLimitType == ENodeLimit::Limited", EditConditionHides))
	int32 ChildrenLimit = 0;

	UPROPERTY(EditDefaultsOnly, Category = "GenericGraphNode_Editor", meta = (DisplayBefore = "ParentLimit"))
	ENodeLimit ParentLimitType = ENodeLimit::Unlimited;

	UPROPERTY(EditDefaultsOnly, Category = "GenericGraphNode_Editor", meta = (DisplayBefore = "ChildrenLimit"))
	ENodeLimit ChildrenLimitType = ENodeLimit::Unlimited;

	UPROPERTY(EditDefaultsOnly, Category = "GenericGraphNode_Editor")
	uint8 bEdgesEnabled : 1;
	// Torbie End Change
	//-----------------------------------------------------------------------------
#endif

#if WITH_EDITOR
	virtual bool IsNameEditable() const;

	virtual FLinearColor GetBackgroundColor() const;

	virtual FText GetNodeTitle() const;

	virtual void SetNodeTitle(const FText& NewTitle);

	virtual bool CanCreateConnection(UGenericGraphNode* Other, FText& ErrorMessage);

	virtual bool CanCreateConnectionTo(UGenericGraphNode* Other, int32 NumberOfChildrenNodes, FText& ErrorMessage);
	virtual bool CanCreateConnectionFrom(UGenericGraphNode* Other, int32 NumberOfParentNodes, FText& ErrorMessage);

	//----------------------------------------------------------------------------
	// Torbie Begin Change
	virtual void ChildrenListChanged() {};
	// Torbie End Change
	//----------------------------------------------------------------------------
#endif
};
