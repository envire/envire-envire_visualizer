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

#include "AddItemDialog.hpp"
#include "plugins/PclPlugin/PclItemFactory.hpp"
#include "plugins/SmurfPlugin/SmurfItemFactory.h"
#include <envire_core/util/Demangle.hpp>
#include <envire_core/graph/EnvireGraph.hpp>

using namespace envire::core;
using namespace envire::viz;
using namespace std;

AddItemDialog::AddItemDialog(QWidget* parent): QDialog(parent), 
    currentConfigWidget(nullptr), currentItemType("")
{
  window.setupUi(this);
  
  
  connect(window.comboBoxType, SIGNAL(currentIndexChanged(QString)),
          this, SLOT(currentItemTypeChanged(QString)));
  
  //for now plugins are simply added manually. this might change later.
  addItemFactory(shared_ptr<ItemFactoryInterface>(new PclItemFactory()));
  addItemFactory(shared_ptr<SmurfItemFactory>(new SmurfItemFactory()));
  
  for(ItemFactoryHash::const_iterator it = itemFactories.begin();
      it != itemFactories.end(); ++it)
  {
    window.comboBoxType->addItem(it.key());
  }
}

void AddItemDialog::addItemFactory(shared_ptr<ItemFactoryInterface> factory)
{
  for(const type_index& index : factory->getSupportedTypes())
  {
    const QString type = QString::fromStdString(demangleTypeName(index));
    if(itemFactories.contains(type))
    {
      throw std::runtime_error("There is already an item factory registered for type " + type.toStdString());
    }
    else
    {
      itemFactories.insert(type, factory);
      itemTypes.insert(type, index);
    }
  }
}

void AddItemDialog::currentItemTypeChanged(QString itemType)
{
  if(currentItemType != "")
  {
    assert(currentConfigWidget != nullptr);
    currentConfigWidget->hide();
    window.verticalLayout->removeWidget(currentConfigWidget);
  }
  currentItemType = itemType;
  std::shared_ptr<ItemFactoryInterface> factory = itemFactories[itemType];
  TypeHash::iterator indexIterator = itemTypes.find(itemType);
  assert(indexIterator != itemTypes.end());
  std::type_index index = indexIterator.value();
  currentConfigWidget = factory->getConfigurationWidget(index);
  window.verticalLayout->addWidget(currentConfigWidget);
  currentConfigWidget->show();
}

void AddItemDialog::accept()
{
  std::shared_ptr<ItemFactoryInterface> factory = itemFactories[currentItemType];
  TypeHash::iterator indexIterator = itemTypes.find(currentItemType);
  assert(indexIterator != itemTypes.end());
  
  factory->addItem(indexIterator.value(), graph, targetFrame.toStdString(), currentConfigWidget);
  
  QDialog::accept();
}

void AddItemDialog::addItem(std::shared_ptr<envire::core::EnvireGraph>& graph,
                            const QString frame)
{
  this->graph = graph;
  targetFrame = frame;
  show();
}



