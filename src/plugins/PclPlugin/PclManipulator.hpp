#pragma once
#include "envire_visualizer/plugins/ItemManipulatorFactoryInterface.hpp"
#include <QWidget>

class QTableWidget;

namespace envire { namespace viz 
{
class PclItemManipulatorFactory : public ItemManipulatorFactoryInterface
{
public:
  PclItemManipulatorFactory();
  
  virtual QWidget* create(core::ItemBase::Ptr selectedItem,
                          std::shared_ptr<envire::core::EnvireGraph> graph,
                          const core::TreeView& treeView) override;
                          
  virtual const std::vector<std::type_index>& getSupportedTypes() override;
  
private:
  std::vector<std::type_index> types;
};


class PclItemManipulator : public QWidget
{
  Q_OBJECT
public:
  PclItemManipulator(core::ItemBase::Ptr selectedItem,
                          std::shared_ptr<envire::core::EnvireGraph> graph,
                          const core::TreeView& treeView, QWidget* parent = nullptr);
  
private slots:
  //called when the user selects a different item for icp alignment
  void itemSelectionChanged();
private:
  
  /**Returns a list of all items of @p type in the graph */
  std::vector<envire::core::ItemBase::Ptr> findMatchingItems(const std::type_index& type) const;
  
  envire::core::ItemBase::Ptr selectedItem;
  envire::core::ItemBase::Ptr alignToItem;
  std::shared_ptr<envire::core::EnvireGraph> graph;
  const core::TreeView& treeView;
  QTableWidget* itemTable;
};


}}