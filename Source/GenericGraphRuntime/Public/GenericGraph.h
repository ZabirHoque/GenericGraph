#pragma once

#include "CoreMinimal.h"
#include "GenericGraphNode.h"
#include "GenericGraphEdge.h"
#include "GameplayTagContainer.h"
#include "GenericGraph.generated.h"

UCLASS(Blueprintable)
class GENERICGRAPHRUNTIME_API UGenericGraph : public UObject
{
	GENERATED_BODY()

public:
	//-----------------------------------------------------------------------------
	// Torbie Begin Change
	UGenericGraph();

	UPROPERTY(EditDefaultsOnly, Category = "GenericGraph")
	FText Name;
	// Torbie End Change
	//-----------------------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "GenericGraph")
	TSubclassOf<UGenericGraphNode> NodeType;

	UPROPERTY(EditDefaultsOnly, Category = "GenericGraph")
	TSubclassOf<UGenericGraphEdge> EdgeType;

	UPROPERTY(BlueprintReadOnly, Category = "GenericGraph")
	TArray<UGenericGraphNode*> RootNodes;

	UPROPERTY(BlueprintReadOnly, Category = "GenericGraph")
	TArray<UGenericGraphNode*> AllNodes;

	UFUNCTION(BlueprintCallable, Category = "GenericGraph")
	void Print(bool ToConsole = true, bool ToScreen = true);

	UFUNCTION(BlueprintCallable, Category = "GenericGraph")
	int GetLevelNum() const;

	UFUNCTION(BlueprintCallable, Category = "GenericGraph")
	void GetNodesByLevel(int Level, TArray<UGenericGraphNode*>& Nodes);

	void ClearGraph();

	//-----------------------------------------------------------------------------
	// Torbie Begin Change
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	UEdGraph* EdGraph = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "GenericGraph_Editor")
	uint8 bCanRenameNode : 1;

	UPROPERTY(EditDefaultsOnly, Category = "GenericGraph_Editor")
	uint8 bCanBeCyclical : 1;

	UPROPERTY(EditDefaultsOnly, Category = "GenericGraph_Editor")
	uint8 bAllowDuplicateEdges : 1;
#endif

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GenericGraph", meta = (DisplayAfter="AllNodes"))
	uint8 bEdgeEnabled : 1;
	// Torbie End Change
	//-----------------------------------------------------------------------------
};
