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
#include <QDialog>
#include "ui_additemdialog.h"
#include <memory>
#ifndef Q_MOC_RUN
#include "plugins/ItemFactoryInterface.hpp"
#endif
#include <type_traits>

namespace envire { namespace core {
  class EnvireGraph;
}}

namespace envire { namespace viz {
  
class AddItemDialog : public QDialog
{
  Q_OBJECT
public:
  AddItemDialog(QWidget* parent = nullptr);
  
  /**Shows the dialog and, if the user accepts, adds the item to the specified frame */
  void addItem(std::shared_ptr<envire::core::EnvireGraph>& graph, const QString frame);
  
public slots:
  virtual void accept() override;
  
private slots:
  void currentItemTypeChanged(QString itemType);
  
private:
  /** @throw std::runtime_error if a factory has already been added for one of
   *                            the types provided by @p factory*/
  void addItemFactory(std::shared_ptr<ItemFactoryInterface> factory);
  
  Ui::DialogAddItem window;
  using ItemFactoryHash = QHash<QString, std::shared_ptr<ItemFactoryInterface>>;
  ItemFactoryHash itemFactories;
  using TypeHash = QHash<QString, std::type_index>;
  TypeHash itemTypes;
  QWidget* currentConfigWidget;
  QString currentItemType;
  std::shared_ptr<envire::core::EnvireGraph> graph;
  QString targetFrame;
};
}}
