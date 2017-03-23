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

#pragma once
#include "envire_visualizer/plugins/ItemManipulatorFactoryInterface.hpp"
#include <QWidget>


class QTableWidget;

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
  PclItemManipulator(core::ItemBase::Ptr selectedItem,
                          std::shared_ptr<envire::core::EnvireGraph> graph,
                          const core::TreeView& treeView, QWidget* parent = nullptr);
  
private slots:
  //called when the user selects a different item for icp alignment
  void itemSelectionChanged();
  
  void alignButtonClicked(bool checked);
private:
  
  /**Returns a list of all items of @p type in the graph */
  std::vector<envire::core::ItemBase::Ptr> findMatchingItems(const std::type_index& type) const;
  
  std::vector<envire::core::ItemBase::Ptr> otherItems; //contains all other items from the graph
  envire::core::ItemBase::Ptr selectedItem;
  envire::core::ItemBase::Ptr alignToItem;
  std::shared_ptr<envire::core::EnvireGraph> graph;
  const core::TreeView& treeView;
  QTableWidget* itemTable;
};


}}