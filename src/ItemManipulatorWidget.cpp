#include "ItemManipulatorWidget.hpp"
#include <envire_core/items/ItemBase.hpp>
#include <QVBoxLayout>
#include "ItemTableModel.hpp"
#include "plugins/PclPlugin/PclManipulator.hpp"
#include "plugins/ItemManipulatorFactoryInterface.hpp"

using namespace envire::viz;
using namespace envire::core;

ItemManipulatorWidget::ItemManipulatorWidget(QWidget* parent): QWidget(parent)
{
  layout = new QVBoxLayout();
  setLayout(layout);
  
  //for now plugnis are hard coded, this may change later
  std::shared_ptr<ItemManipulatorFactoryInterface> factory(new PclItemManipulatorFactory());
  addItemManipulator(factory);
  
}

void ItemManipulatorWidget::itemSelected(envire::core::ItemBase::Ptr item,
                                         std::shared_ptr<envire::core::EnvireGraph> graph,
                                         const envire::core::TreeView& tree)
{
  std::cout << "ITEM ITEM ITEM" << std::endl;
  if(item != selectedItem)
  {
    selectedItem = item;
    clearLayout();
    if(selectedItem)
    {
      const std::type_index type = item->getTypeIndex();
      if(manipulators.find(type) != manipulators.end())
      {
        const auto& factory = manipulators[type];
        QWidget* widget = factory->create(item, graph, tree);
        widget->setParent(this);
        layout->addWidget(widget);
        widget->show();
      }
    }
  }
}

void ItemManipulatorWidget::clearLayout()
{
  QLayoutItem *child;
  while ((child = layout->takeAt(0)) != 0) 
  {
    delete child;
  }
}

void ItemManipulatorWidget::addItemManipulator(std::shared_ptr<ItemManipulatorFactoryInterface> manipulator)
{
  for(const std::type_index& index : manipulator->getSupportedTypes())
  {
    if(manipulators.find(index) != manipulators.end())
    {
      //FIXME throw or do something cool
      std::cerr << "There is already an item manipulator factory registered for type "
                << index.name() << std::endl;
      return;
    }
    else
    {
      manipulators[index] = manipulator;
    }
  }
}


