#pragma once
#include <QWidget>
#include <QModelIndex>
#include <QVBoxLayout>
#include <unordered_map>
#include <envire_core/items/ItemBase.hpp>
#include <memory>
#include <osgViz/plugins/Object.h>

namespace envire { namespace core {
  class EnvireGraph;
  class TreeView;
}}

namespace envire { namespace viz 
{
class ItemTableModel;
class ItemManipulatorFactoryInterface;
  
class ItemManipulatorWidget : public QWidget
{
  Q_OBJECT;
public:
  ItemManipulatorWidget(QWidget* parent = nullptr);
  
public slots:
  /** @param item The item that was selected
   *  @param graph the graph that the item belongs to
   *  @param tree the TreeView that the item belongs to
   *  @param vizNode A node of the scene graph that is located at the root of
   *                 frame that the item is in. Visualizations will be added to
   *                 this node.*/
  void itemSelected(envire::core::ItemBase::Ptr item,
                    std::shared_ptr<envire::core::EnvireGraph> graph,
                    const envire::core::TreeView& tree); 
private:
  
  void clearLayout();
  
  void addItemManipulator(std::shared_ptr<ItemManipulatorFactoryInterface> manipulator);
  
  envire::core::ItemBase::Ptr selectedItem;
  QVBoxLayout* layout;
  using ManipulatorHash = std::unordered_map<std::type_index, std::shared_ptr<ItemManipulatorFactoryInterface>>;
  ManipulatorHash manipulators;
   
};
  
}}