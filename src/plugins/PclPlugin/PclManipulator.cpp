#include "PclManipulator.hpp"
#include <QHBoxLayout>
#include <QLabel> //FIXME remove after test
#include <envire_pcl/PointCloud.hpp>

using namespace envire::viz;
using namespace envire::core;


PclItemManipulatorFactory::PclItemManipulatorFactory()
{
  envire::pcl::PointCloud item;
  types.emplace_back(*(item.getTypeInfo()));
}

QWidget* PclItemManipulatorFactory::create(ItemBase::Ptr selectedItem,
                                           std::shared_ptr<envire::core::EnvireGraph> graph,
                                           const envire::core::TreeView& treeView)
{
  //FIXME do type check
  return new PclItemManipulator();
}

const std::vector<std::type_index>& PclItemManipulatorFactory::getSupportedTypes()
{
  return types;
}



PclItemManipulator::PclItemManipulator(QWidget* parent): QWidget(parent)
{
  QHBoxLayout* layout = new QHBoxLayout(this);
  layout->addWidget(new QLabel("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  setLayout(layout);
}
