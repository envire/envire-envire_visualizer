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

#include "ItemTableModel.hpp"
#include <glog/logging.h>
#include <envire_core/items/ItemMetadata.hpp>

using namespace envire::core;

namespace envire { namespace viz {

ItemTableModel::ItemTableModel(QObject* parent): QAbstractTableModel(parent),
  numColumns(4)
{}


void ItemTableModel::addItem(envire::core::ItemBase::Ptr item)
{
  {
    beginInsertRows(QModelIndex(), items.size(), items.size());
    items.push_back(item);
    endInsertRows();
  }
}

void ItemTableModel::clear()
{
  if(items.size() > 0)
  {
    beginRemoveRows(QModelIndex(), 0, items.size()-1);
    items.clear();
    endRemoveRows();
  }
}

int ItemTableModel::columnCount(const QModelIndex& parent) const
{
  return numColumns;
}

int ItemTableModel::rowCount(const QModelIndex& parent) const
{
  return items.size();
}

QVariant ItemTableModel::data(const QModelIndex& index, int role) const
{
  const int row = index.row();
  const int col = index.column();

  if(row >= int(items.size()))
  {
    LOG(ERROR) << "ItemTableModel: row index out of range: " << row;
    return QVariant();
  }
  
  if(col >= numColumns)
  {
    LOG(ERROR) << "ItemTableModel: column index out of range: " << col;
    return QVariant();
  }
  
  if (role == Qt::DisplayRole)
  {
    const ItemBase::Ptr item = items[row];
    switch(col)
    {
      case 0: //uuid
        return QVariant(QString::fromStdString(item->getIDString()));
      case 1: //type
        return QVariant(QString::fromStdString(item->getClassName()));
      case 2: //raw type
      {
        std::string rawType("Unknown Type");
        try 
        {
          //this fails if the type was not properly loaded using the plugin system
          rawType = ItemMetadataMapping::getMetadata(*(item->getTypeInfo())).embeddedTypename;
        }
        catch(const std::out_of_range& ex){}
        return QVariant(QString::fromStdString(rawType));
      }
      case 3: //timestamp
        return QVariant(QString::fromStdString(item->getTime().toString()));        
      default:
        LOG(ERROR) << "ItemTableModel: unknown column: " << col;
        return QVariant();
    }
  }
  return QVariant();
}

QVariant ItemTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) 
  {
    switch (section) 
    {
      case 0:
        return tr("UUID");
      case 1:
        return tr("Class Name");
      case 2:
        return tr("Raw Type");
      case 3:
        return tr("Timestamp");
      default:
        return QVariant();
    }
  }
  return QVariant();
}

envire::core::ItemBase::Ptr ItemTableModel::getItem(const QModelIndex& index) const
{
  const int row = index.row();
  assert(row < items.size());
  return items[row];
}

}}
