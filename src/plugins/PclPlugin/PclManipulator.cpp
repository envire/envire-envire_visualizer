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

#include <pcl/registration/gicp.h>
#include <pcl/conversions.h>
#include <pcl/common/transforms.h>
#include "PclManipulator.hpp"
#include <QHBoxLayout>
#include <QLabel> 
#include <envire_core/items/Item.hpp>
#include <envire_core/graph/GraphTypes.hpp>
#include <envire_core/items/Item.hpp>
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
  const Transform otherTf = graph->getTransform(treeView.root, graph->getVertex(alignToItem->getFrame()));

  // other in selected tf
  const Eigen::Affine3d otherInSelected = (selectedTf.transform.inverse() * otherTf.transform).getTransform();
  std::cout << "otherInSelected: " << std::endl  << otherInSelected.matrix() << std::endl;

  ::pcl::PointCloud<::pcl::PointXYZ>::Ptr otherPclTransformed(new ::pcl::PointCloud<::pcl::PointXYZ>);
  ::pcl::transformPointCloud(otherPcl, *otherPclTransformed.get(), otherInSelected.cast<float>());
  
  ::pcl::GeneralizedIterativeClosestPoint<::pcl::PointXYZ, ::pcl::PointXYZ> icp;
  // TODO make max correspondence distance configurable
  icp.setMaxCorrespondenceDistance(1.0);
  icp.setInputCloud(selectedPcl.makeShared());
  icp.setInputTarget(otherPclTransformed);

  ::pcl::PointCloud<::pcl::PointXYZ> selectedAlignedToOther;
  //use the known transformation between the two frames as icp starting guess
  icp.align(selectedAlignedToOther);

  if(icp.hasConverged())
  {
    const base::Affine3d finalTfMatrix(icp.getFinalTransformation().cast<double>());
     std::cout << "icp result: " << std::endl << finalTfMatrix.matrix() << std::endl;
    const base::TransformWithCovariance result(finalTfMatrix);

    GraphTraits::vertex_descriptor parent = treeView.getParent(selectedDesc);
    std::cout << "parent is: " << graph->getFrameId(parent) << std::endl;
    if(graph->null_vertex() == parent)
      parent = selectedDesc;
    const base::TransformWithCovariance rootToParent(graph->getTransform(treeView.root, parent).transform);
    
    std::cout << (rootToParent * selectedTf.transform.inverse()).getTransform().matrix() << std::endl;
    
    
    Transform parentToSelected = graph->getTransform(parent, selectedDesc);
    parentToSelected.transform = parentToSelected.transform * result;
    graph->updateTransform(parent, selectedDesc, parentToSelected);
    graph->updateTransform(selectedDesc, parent, parentToSelected.inverse());
  }
  else
  {
    std::cerr << "ICP did NOT converge" << std::endl;
  }
}