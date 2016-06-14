#pragma once
#include "envire_visualizer/plugins/ItemManipulatorFactoryInterface.hpp"
#include <QWidget>

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
  PclItemManipulator(QWidget* parent = nullptr);
};


}}