#include "AddItemDialog.hpp"
#include "plugins/PclPlugin/PclItemFactory.hpp"
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
  
  ItemBase::Ptr item = factory->createItem(indexIterator.value(), currentConfigWidget);
  item->setFrame(targetFrame.toStdString());
  graph->addItem(item);
  
  QDialog::accept();
}

void AddItemDialog::addItem(std::shared_ptr<envire::core::EnvireGraph>& graph,
                            const QString frame)
{
  this->graph = graph;
  targetFrame = frame;
  show();
}



