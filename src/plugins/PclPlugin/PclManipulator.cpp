#include <pcl/registration/icp.h>
#include <pcl/conversions.h>
#include <pcl/common/transforms.h>
#include "PclManipulator.hpp"
#include <QHBoxLayout>
#include <QLabel> 
#include <envire_core/items/Item.hpp>
#include <envire_core/graph/GraphTypes.hpp>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>


using namespace envire::viz;
using namespace envire::core;


PclItemManipulatorFactory::PclItemManipulatorFactory()
{
  envire::core::Item<pcl::PCLPointCloud2> item;
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
    
    QPushButton* button = new QPushButton("Align", this);
    layout->addWidget(button);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(alignButtonClicked(bool)));
    setLayout(layout);
    
    connect(itemTable, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
    
    otherItems = findMatchingItems(selectedItem->getTypeIndex());
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
    assert(row < otherItems.size());
    alignToItem = otherItems[row];
  }
}

void PclItemManipulator::alignButtonClicked(bool checked)
{
  assert(alignToItem && selectedItem);

  using EnvPcl = envire::core::Item<pcl::PCLPointCloud2>;
  EnvPcl::Ptr selectedCloud = boost::dynamic_pointer_cast<EnvPcl>(selectedItem);
  EnvPcl::Ptr otherCloud = boost::dynamic_pointer_cast<EnvPcl>(alignToItem);
    //they have to be of the correct type, otherwise the user wouldnt have been able to select them
  assert(selectedCloud);
  assert(otherCloud);
  
  ::pcl::PointCloud<::pcl::PointXYZ> selectedPcl;
  ::pcl::fromPCLPointCloud2(selectedCloud->getData(), selectedPcl);
  ::pcl::PointCloud<::pcl::PointXYZ> otherPcl;
  ::pcl::fromPCLPointCloud2(otherCloud->getData(), otherPcl);
  
  GraphTraits::vertex_descriptor selectedDesc = graph->getVertex(selectedItem->getFrame());
  
  //the pointclouds are relative to their frames, to align them we have to move
  //them to the origin coordinate system
  const Transform selectedTf = graph->getTransform(treeView.root, selectedDesc);
  ::pcl::PointCloud<::pcl::PointXYZ>::Ptr selectedPclTransformed(new ::pcl::PointCloud<::pcl::PointXYZ>);
  ::pcl::transformPointCloud(selectedPcl, *selectedPclTransformed.get(), selectedTf.transform.getTransform().cast<float>());
  
  const Transform otherTf = graph->getTransform(treeView.root, graph->getVertex(alignToItem->getFrame()));
  ::pcl::PointCloud<::pcl::PointXYZ>::Ptr otherPclTransformed(new ::pcl::PointCloud<::pcl::PointXYZ>);
  ::pcl::transformPointCloud(otherPcl, *otherPclTransformed.get(), otherTf.transform.getTransform().cast<float>());
  
  
  ::pcl::IterativeClosestPoint<::pcl::PointXYZ, ::pcl::PointXYZ> icp;
  icp.setInputCloud(selectedPclTransformed);
  icp.setInputTarget(otherPclTransformed);
    
  ::pcl::PointCloud<::pcl::PointXYZ> selectedAlignedToOther;
  //use the known transformation between the two frames as icp starting guess
  const Eigen::Affine3d guess = (otherTf.transform * selectedTf.transform.inverse()).getTransform();
  std::cout << "Guess: " << std::endl  << guess.matrix() << std::endl;
  icp.align(selectedAlignedToOther, guess.cast<float>().matrix());
  if(icp.hasConverged())
  {
    const base::Affine3d finalTfMatrix(icp.getFinalTransformation().cast<double>());
     std::cout << "icp result: " << std::endl << finalTfMatrix.matrix() << std::endl;
    const base::TransformWithCovariance finalTf(finalTfMatrix);
    

    GraphTraits::vertex_descriptor parent = treeView.getParent(selectedDesc);
    std::cout << "parent is: " << graph->getFrameId(parent) << std::endl;
    if(graph->null_vertex() == parent)
      parent = selectedDesc;
    const base::TransformWithCovariance rootToParent(graph->getTransform(treeView.root, parent).transform);
    
    std::cout << (rootToParent * selectedTf.transform.inverse()).getTransform().matrix() << std::endl;
    
    
    const base::TransformWithCovariance finalTfInParent =  finalTf * rootToParent;
    Transform parentToSelected = graph->getTransform(parent, selectedDesc);
    parentToSelected.transform = finalTfInParent * parentToSelected.transform;
    graph->updateTransform(parent, selectedDesc, parentToSelected);
  }
  else
  {
    std::cerr << "ICP did NOT converge" << std::endl;
  }
}




