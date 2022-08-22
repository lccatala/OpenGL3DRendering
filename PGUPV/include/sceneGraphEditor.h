#pragma once
// 2016

#include <memory>

#include "widget.h"
#include "label.h"
#include "separator.h"
#include "node.h"
#include "treeWidget.h"
#include "animationClip.h"
#include "listBoxWidget.h"

namespace PGUPV {
  class Scene;
  class CheckBoxWidget;
  class Button;
  class TransformationWidget;
  class FloatSliderWidget;
  class HBox;

  class SceneGraphEditor : public Widget {
  public:
    SceneGraphEditor(std::shared_ptr<Scene> scene);
    ~SceneGraphEditor();
    /**
    Vuelve a recorrer la escena y reconstruye la lista de nodos, seleccionando el primero
    */
    void refresh();
    /**
    \return el último nodo seleccionado (nullptr si no hay nodos seleccionados)
    */
    const NodePath &getLastSelection() const { return selectedNodePath; }

    void renderWidget() override;

	void setScene(std::shared_ptr<Scene> scene);

    void selectNode(uint32_t nodeId);
  private:
    std::shared_ptr<Scene> theScene;
    void build();
    std::shared_ptr<CheckBoxWidget> visible;
    std::shared_ptr<TransformationWidget> transformWidget;
	std::shared_ptr<HBox> xformsButtons;
    std::shared_ptr<Button> removeNodeBtn, resetBtn, editNodeBtn;
	std::shared_ptr<FloatSliderWidget> animationSpeed;
	std::shared_ptr<ListBoxWidget<AnimationClip::WrapMode>> animWrapMode;
	std::shared_ptr<HBox> animationButtons;
	std::shared_ptr<Button> playBtn, stopBtn;
    std::shared_ptr<TreeWidget<NodePath>> nodeListTree;
    Label bsLabel, bsCenterInfo, bsRadiusInfo, bbLabel, bb1Info, bb2Info;
    Separator sep;

    void updateListBox();
    void selectNode(const NodePath &np);
    void editNode(const NodePath &np);
    NodePath selectedNodePath;


  };
};
