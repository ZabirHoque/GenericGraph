#pragma once

#include "CoreMinimal.h"
#include "Settings_GenericGraphEditor.h"
#include "GenericGraph.h"

#if ENGINE_MAJOR_VERSION == 5
#include "UObject/ObjectSaveContext.h"
#endif // #if ENGINE_MAJOR_VERSION == 5

class FGGAssetEditorToolbar;
//-----------------------------------------------------------------------------
// Torbie Begin Change
class UAssetGraphSchema_GenericGraph;
class UEdGraph_GenericGraph;
// Torbie End Change
//-----------------------------------------------------------------------------

class GENERICGRAPHEDITOR_API FAssetEditor_GenericGraph : public FAssetEditorToolkit, public FNotifyHook, public FGCObject
{
public:
	FAssetEditor_GenericGraph();
	virtual ~FAssetEditor_GenericGraph();

	void InitGenericGraphAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UGenericGraph* Graph);

	// IToolkit interface
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	// End of IToolkit interface

	// FAssetEditorToolkit
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FString GetDocumentationLink() const override;
	virtual void SaveAsset_Execute() override;
	// End of FAssetEditorToolkit

	//Toolbar
	void UpdateToolbar();
	TSharedPtr<class FAssetEditorToolbar_GenericGraph> GetToolbarBuilder() { return ToolbarBuilder; }
	void RegisterToolbarTab(const TSharedRef<class FTabManager>& TabManager);


	// FSerializableObject interface
	//-----------------------------------------------------------------------------
	// Torbie Begin Change
	void AddReferencedObjects(FReferenceCollector& Collector) override;
	FString GetReferencerName() const override;
	bool GetReferencerPropertyName(UObject* Object, FString& OutPropertyName) const override;
	// Torbie End Change
	//-----------------------------------------------------------------------------
	// End of FSerializableObject interface

#if ENGINE_MAJOR_VERSION == 5
	// FGCObject interface
	virtual FString GetReferencerName() const
	{
		return TEXT("FAssetEditor_LTGenericGraph");
	}
	// ~FGCObject interface
#endif // #if ENGINE_MAJOR_VERSION == 5

	UGenericGraphEditorSettings* GetSettings() const;

	//-------------------------------------------------------------------------
	// Torbie Begin Change
	void SetGraphClass(TSubclassOf<UEdGraph_GenericGraph> InGraphClass)
	{
		GraphClass = InGraphClass;
	}

	void SetSchemaClass(TSubclassOf<UAssetGraphSchema_GenericGraph> InSchemaClass)
	{
		SchemaClass = InSchemaClass;
	}

	void SetEditorCornerText(const FText& InEditorCornerText)
	{
		EditorCornerText = InEditorCornerText;
	}
	// Torbie End Change
	//-------------------------------------------------------------------------

protected:
	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_EditorSettings(const FSpawnTabArgs& Args);

	void CreateInternalWidgets();
	TSharedRef<SGraphEditor> CreateViewportWidget();


	void BindCommands();

	void CreateEdGraph();

	void CreateCommandList();

	TSharedPtr<SGraphEditor> GetCurrGraphEditor() const;

	FGraphPanelSelectionSet GetSelectedNodes() const;

	void RebuildGenericGraph();

	// Delegates for graph editor commands
	void SelectAllNodes();
	bool CanSelectAllNodes();
	void DeleteSelectedNodes();
	bool CanDeleteNodes();
	void DeleteSelectedDuplicatableNodes();
	void CutSelectedNodes();
	bool CanCutNodes();
	void CopySelectedNodes();
	bool CanCopyNodes();
	void PasteNodes();
	void PasteNodesHere(const FVector2D& Location);
	bool CanPasteNodes();
	void DuplicateNodes();
	bool CanDuplicateNodes();

	void GraphSettings();
	bool CanGraphSettings() const;

	void AutoArrange();
	bool CanAutoArrange() const;

	void OnRenameNode();
	bool CanRenameNodes() const;

	//////////////////////////////////////////////////////////////////////////
	// graph editor event
	void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection);

	void OnNodeDoubleClicked(UEdGraphNode* Node);

	void OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent);

#if ENGINE_MAJOR_VERSION < 5
	void OnPackageSaved(const FString& PackageFileName, UObject* Outer);
#else // #if ENGINE_MAJOR_VERSION < 5
	void OnPackageSavedWithContext(const FString& PackageFileName, UPackage* Package, FObjectPostSaveContext ObjectSaveContext);
#endif // #else // #if ENGINE_MAJOR_VERSION < 5

protected:
	UGenericGraphEditorSettings* GenricGraphEditorSettings;

	UGenericGraph* EditingGraph;

	//Toolbar
	TSharedPtr<class FAssetEditorToolbar_GenericGraph> ToolbarBuilder;

	/** Handle to the registered OnPackageSave delegate */
	FDelegateHandle OnPackageSavedDelegateHandle;

	TSharedPtr<SGraphEditor> ViewportWidget;
	TSharedPtr<class IDetailsView> PropertyWidget;
	TSharedPtr<class IDetailsView> EditorSettingsWidget;

	/** The command list for this editor */
	TSharedPtr<FUICommandList> GraphEditorCommands;

	//-------------------------------------------------------------------------
	// Torbie chabge begin
	TSubclassOf<UEdGraph_GenericGraph> GraphClass;
	TSubclassOf<UAssetGraphSchema_GenericGraph> SchemaClass;
	FText EditorCornerText;
	// Torbie change end
	//-------------------------------------------------------------------------
};


