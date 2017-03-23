//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

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


