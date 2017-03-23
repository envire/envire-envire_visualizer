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

#include "TransformModel.hpp"
#include <envire_core/items/Transform.hpp>
#include <glog/logging.h>

namespace envire { namespace viz {

  
TransformModel::TransformModel() :
  QStandardItemModel()
{
  tf.setIdentity();
  setColumnCount(2);
  QStringList headerLabels;
  headerLabels.append("");
  headerLabels.append("Value");
  setHorizontalHeaderLabels(headerLabels);
  
  QStandardItem *parentItem = invisibleRootItem();
  
  QList<QStandardItem*> transItemList;
  QStandardItem* transItem = new QStandardItem("Translation");
  QStandardItem* transItemValue = new QStandardItem("");
  transItem->setEditable(false);
  transItemValue->setEditable(false);
  transItemList.append(transItem);
  transItemList.append(transItemValue);
  parentItem->appendRow(transItemList);
  
  QList<QStandardItem*> rotItemList;
  QStandardItem* rotItem = new QStandardItem("Orientation");
  QStandardItem* rotItemValue = new QStandardItem("");
  rotItem->setEditable(false);
  rotItemValue->setEditable(false);
  rotItemList.append(rotItem);
  rotItemList.append(rotItemValue);
  parentItem->appendRow(rotItemList);

  QList<QStandardItem*> transX;
  QList<QStandardItem*> transY;
  QList<QStandardItem*> transZ;
  
  QStandardItem* transXText = new QStandardItem("X");
  transXText->setEditable(false);
  QStandardItem* transYText = new QStandardItem("Y");
  transYText->setEditable(false);
  QStandardItem* transZText = new QStandardItem("Z");
  transZText->setEditable(false);
  
  transX.append(transXText);
  transY.append(transYText);
  transZ.append(transZText);
  transX.append(&transXItem);
  transY.append(&transYItem);
  transZ.append(&transZItem);
  
  transItem->appendRow(transX);
  transItem->appendRow(transY);
  transItem->appendRow(transZ);
  
  QList<QStandardItem*> rotX;
  QList<QStandardItem*> rotY;
  QList<QStandardItem*> rotZ;
  QList<QStandardItem*> rotW;
  
  QStandardItem* rotXText = new QStandardItem("X");
  QStandardItem* rotYText = new QStandardItem("Y");
  QStandardItem* rotZText = new QStandardItem("Z");
  QStandardItem* rotWText = new QStandardItem("W");
  rotXText->setEditable(false);
  rotYText->setEditable(false);
  rotZText->setEditable(false);
  rotWText->setEditable(false);
  
  rotX.append(rotXText);
  rotY.append(rotYText);
  rotZ.append(rotZText);
  rotW.append(rotWText);
  
  rotX.append(&rotXItem);
  rotY.append(&rotYItem);
  rotZ.append(&rotZItem);
  rotW.append(&rotWItem);
  
  rotItem->appendRow(rotX);
  rotItem->appendRow(rotY);
  rotItem->appendRow(rotZ);
  rotItem->appendRow(rotW);

  QList<QStandardItem*> timestampList;
  QStandardItem* timestampText = new QStandardItem("Timestamp");
  timestampText->setEditable(false);
  timestampList.append(timestampText);
  timestampList.append(&timestampItem);
  parentItem->appendRow(timestampList);
  
  
  connect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChangedSlot(QStandardItem*)));
  setTransform(tf);//works because tf is default constructed to Identity  
}

void TransformModel::setTransform(const envire::core::Transform& newValue)
{
  tf = newValue;
  transXItem.setData(QVariant::fromValue<double>(tf.transform.translation.x()) ,Qt::DisplayRole);
  transYItem.setData(QVariant::fromValue<double>(tf.transform.translation.y()) ,Qt::DisplayRole);
  transZItem.setData(QVariant::fromValue<double>(tf.transform.translation.z()) ,Qt::DisplayRole);
  
  rotXItem.setData(QVariant::fromValue<double>(tf.transform.orientation.x()) ,Qt::DisplayRole);
  rotYItem.setData(QVariant::fromValue<double>(tf.transform.orientation.y()) ,Qt::DisplayRole);
  rotZItem.setData(QVariant::fromValue<double>(tf.transform.orientation.z()) ,Qt::DisplayRole);
  rotWItem.setData(QVariant::fromValue<double>(tf.transform.orientation.w()) ,Qt::DisplayRole);
  
  timestampItem.setData(QVariant(QString::fromStdString(tf.time.toString())), Qt::DisplayRole);
} 

void  TransformModel::itemChangedSlot(QStandardItem * item)
{
  const double data = item->data(Qt::DisplayRole).toDouble();
  if(item == &transXItem)
  {
    tf.transform.translation.x() = data;
  }
  else if(item == &transYItem)
  {
    tf.transform.translation.y() = data;
  }
  else if(item == &transZItem)
  {
    tf.transform.translation.z() = data;
  }
  else if(item == &rotXItem)
  {
    tf.transform.orientation.x() = data;
  }
  else if(item == &rotYItem)
  {
    tf.transform.orientation.y() = data;
  }
  else if(item == &rotZItem)
  {
    tf.transform.orientation.z() = data;
  }
  else if(item == &rotWItem)
  {
    tf.transform.orientation.w() = data;
  }  
  else if(item == &timestampItem)
  {
    const std::string timeStr = item->data(Qt::DisplayRole).toString().toStdString();
    try 
    {
      std::cout << "TIME: " << timeStr << std::endl;
      tf.time = base::Time::fromString(timeStr);
    }
    catch(const std::runtime_error& ex)
    {
      LOG(ERROR) << "Cannot parse timestamp: " << ex.what();
      timestampItem.setData(QVariant(QString::fromStdString(tf.time.toString())), Qt::DisplayRole);
    }
  }
  emit transformChanged(tf);
}

void TransformModel::setEditable(const bool value)
{
  transXItem.setEditable(value);
  transYItem.setEditable(value);
  transZItem.setEditable(value);
  
  rotXItem.setEditable(value);
  rotYItem.setEditable(value);
  rotZItem.setEditable(value);
  rotWItem.setEditable(value);
  
  timestampItem.setEditable(value);
}

const envire::core::Transform& TransformModel::getTransform() const
{
  return tf;
}
  
  
}}