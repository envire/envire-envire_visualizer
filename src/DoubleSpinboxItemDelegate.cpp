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

#include "DoubleSpinboxItemDelegate.hpp"
#include <QDoubleSpinBox>
#include <limits>
#include <iostream>

namespace envire { namespace viz {

DoubleSpinboxItemDelegate::DoubleSpinboxItemDelegate(QObject *parent)
  : QStyledItemDelegate(parent), decimals(5)
{
}

QWidget* DoubleSpinboxItemDelegate::createEditor(QWidget *parent,
                                                 const QStyleOptionViewItem &/* option */,
                                                 const QModelIndex &/* index */) const
{
    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setFrame(false);
    editor->setDecimals(decimals);
    //numeric_limits<int> is used instead of double because setMinimum() and
    //setMaximum internally round. The rounding breaks for numeric_limits<double>::min()
    //and just returns 0. Thus we use numeric_limits<int> which should be enough
    //for the use case.
    editor->setMinimum(std::numeric_limits<int>::min());
    editor->setMaximum(std::numeric_limits<int>::max());
    return editor;
}

void DoubleSpinboxItemDelegate::setDecimals(const unsigned int value)
{
  decimals = value;
}

void DoubleSpinboxItemDelegate::setEditorData(QWidget *editor,
                                              const QModelIndex &index) const
{
  const double value = index.model()->data(index, Qt::DisplayRole).toDouble();

  QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
  spinBox->setValue(value);
}

void DoubleSpinboxItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                            const QModelIndex &index) const
{
  QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
  spinBox->interpretText();
  const double value = spinBox->value();

  model->setData(index, value, Qt::EditRole);
}

void DoubleSpinboxItemDelegate::updateEditorGeometry(QWidget *editor,
                                                     const QStyleOptionViewItem &option,
                                                     const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
  
}}