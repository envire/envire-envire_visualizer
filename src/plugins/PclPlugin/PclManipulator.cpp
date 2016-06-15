#include "PclManipulator.hpp"
#include <QHBoxLayout>
#include <QLabel> //FIXME remove after test
#include <envire_pcl/PointCloud.hpp>
#include <envire_core/graph/GraphTypes.hpp>
#include <QTableWidget>
#include <QHeaderView>

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
  return new PclItemManipulator(selectedItem, graph, treeView);
}

const std::vector<std::type_index>& PclItemManipulatorFactory::getSupportedTypes()
{
  return types;
}

PclItemManipulator::PclItemManipulator(ItemBase::Ptr selectedItem,
                                       std::shared_ptr< EnvireGraph > graph,
                                       const TreeView& treeView, QWidget* parent) :
  QWidget(parent), selectedItem(selectedItem), graph(graph), treeView(treeView)
{
    QLabel* alignTo = new QLabel("Align to:", this);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(alignTo);
    
    
    itemTable = new QTableWidget(this);
    layout->addWidget(itemTable);
    setLayout(layout);
    
    
    connect(itemTable, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
    
    std::vector<ItemBase::Ptr> otherItems = findMatchingItems(selectedItem->getTypeIndex());
    itemTable->setRowCount(otherItems.size());
    itemTable->setColumnCount(2);
    for(int i = 0; i < otherItems.size(); ++i)
    {
      const ItemBase::Ptr& item = otherItems[i];
      itemTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(item->getFrame())));
      itemTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(item->getIDString())));
    }
    QStringList header;
    header << "Frame" << "Item";
    itemTable->setHorizontalHeaderLabels(header);
    itemTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    itemTable->setSelectionMode(QAbstractItemView::SingleSelection);
    itemTable->verticalHeader()->setVisible(false);
    if(otherItems.size() > 0)
      itemTable->selectRow(0);

    
}

std::vector<ItemBase::Ptr> PclItemManipulator::findMatchingItems(const std::type_index& type) const
{
  std::vector<ItemBase::Ptr> items;
  using Vd = GraphTraits::vertex_descriptor;
  treeView.visitDfs(treeView.root, [this, &items, &type] (Vd node, Vd parent)
  {
    if(this->graph->containsItems(node, type))
    {
      const envire::core::Frame::ItemList& itemsInFrame = this->graph->getItems(node, type);
      items.insert(items.end(), itemsInFrame.begin(), itemsInFrame.end());
    }
  });
  return items;
}

void PclItemManipulator::itemSelectionChanged()
{
  QItemSelectionModel* select = itemTable->selectionModel();
  if(select->hasSelection())
  {
    QModelIndexList rows = select->selectedRows();
    assert(rows.size() > 0);
    //multiselect is disbled, thus all elements in the list belong to the same row
    const int row = rows.at(0).row();
    std::cout << "ROW: " << row << std::endl;
  }
}



