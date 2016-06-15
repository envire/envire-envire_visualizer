#include "ItemManipulatorWidget.hpp"
#include "ItemTableModel.hpp"
#include "plugins/PclPlugin/PclManipulator.hpp"
#include "plugins/ItemManipulatorFactoryInterface.hpp"
#include <envire_core/items/ItemBase.hpp>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>


using namespace envire::viz;
using namespace envire::core;

ItemManipulatorWidget::ItemManipulatorWidget(QWidget* parent): QWidget(parent)
{
  currentManipulatorWidget = nullptr;
  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  groupBox = new QGroupBox(this);
  groupBox->setTitle("Item Manipulator");
  mainLayout->addWidget(groupBox);
  setLayout(mainLayout);
  
  layout = new QVBoxLayout();
  groupBox->setLayout(layout);
  
  noItemSelected = new QLabel("No item Selected");
  noManipulatorAvailable = new QLabel("No manipulator available for this item type");
  layout->addWidget(noItemSelected);
  layout->addWidget(noManipulatorAvailable);
  noManipulatorAvailable->hide();
  
  //for now plugnis are hard coded, this may change later
  std::shared_ptr<ItemManipulatorFactoryInterface> factory(new PclItemManipulatorFactory());
  addItemManipulator(factory);
  
}

void ItemManipulatorWidget::itemSelected(envire::core::ItemBase::Ptr item,
                                         std::shared_ptr<envire::core::EnvireGraph> graph,
                                         const envire::core::TreeView& tree)
{
  if(item != selectedItem)
  {
    selectedItem = item;
    noItemSelected->hide();
    noManipulatorAvailable->hide();
    removeCurrentManipulatorWidget();
    groupBox->setTitle("Item Manipulator");
      
    if(selectedItem)
    {
      const std::type_index type = item->getTypeIndex();
      if(manipulators.find(type) != manipulators.end())
      {
        const auto& factory = manipulators[type];
        currentManipulatorWidget = factory->create(item, graph, tree);
        currentManipulatorWidget->setParent(this);
        layout->addWidget(currentManipulatorWidget);
        currentManipulatorWidget->show();
        groupBox->setTitle(QString("Item Manipulator (") +
                           QString::fromStdString(demangleTypeName(type)) + 
                           QString(")"));
      }
      else
      {
        noManipulatorAvailable->show();
      }
    }
  }
}

void ItemManipulatorWidget::clearSelection()
{
  selectedItem.reset();
  removeCurrentManipulatorWidget();
  noManipulatorAvailable->hide();
  noItemSelected->show();
  groupBox->setTitle("Item Manipulator");
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

void ItemManipulatorWidget::removeCurrentManipulatorWidget()
{
  if(currentManipulatorWidget != nullptr)
  {
    layout->removeWidget(currentManipulatorWidget);
    delete currentManipulatorWidget;
    currentManipulatorWidget = nullptr;
  }
}


