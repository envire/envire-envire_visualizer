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
#include <QStandardItemModel>
#include <envire_core/items/Transform.hpp>

namespace envire { namespace viz {
  
/**A qt model for TransformWithCovariance that can be used with tables and
 * tree views etc. */
class TransformModel : public QStandardItemModel
{
  Q_OBJECT
public:
  TransformModel();
  
signals:
  /**Emitted when the transform is changed either by the user or by 
   * setTransform().*/
  void transformChanged(const envire::core::Transform& newValue);
  
public slots:
  /** Used to programatically change the transform
   * This method should be invoked in the gui thread*/
  void setTransform(const envire::core::Transform& newValue);
  /*If set to false, the model will be read only */
  void setEditable(const bool value);
  
  const envire::core::Transform& getTransform() const;
private slots:

  void  itemChangedSlot(QStandardItem * item);
private:
  QStandardItemModel model;
  envire::core::Transform tf;
  
  QStandardItem transXItem;
  QStandardItem transYItem;
  QStandardItem transZItem;
  QStandardItem rotXItem;
  QStandardItem rotYItem;
  QStandardItem rotZItem;
  QStandardItem rotWItem;
  QStandardItem timestampItem;
};

}}
