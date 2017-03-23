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
#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QDir>

namespace envire { namespace viz {
  
class SmurfWidget : public QWidget
{
  Q_OBJECT
public:
  SmurfWidget(QWidget* parent = nullptr) : QWidget(parent), 
    horizontalLayout(new QHBoxLayout(this)), lineEdit(new QLineEdit()),
    pushButton(new QPushButton())
  {
    lineEdit->setEnabled(false);
    horizontalLayout->addWidget(lineEdit);
    pushButton->setText("Select SMURF File");
    horizontalLayout->addWidget(pushButton);
    setLayout(horizontalLayout);
    
    connect(pushButton, SIGNAL(clicked(bool)), this, SLOT(buttonClicked(bool)));
  }
  
public:
  QString getPath() const
  {
    return lineEdit->text();
  }
  
private slots:
  void buttonClicked(bool checked)
  {
    const QString file = QFileDialog::getOpenFileName(this, tr("Open SMURF File"),
                                                    QDir::currentPath(), QString(),
                                                    0, QFileDialog::DontUseNativeDialog);
    lineEdit->setText(file);
  }
  
private:
  QHBoxLayout *horizontalLayout;
  QLineEdit *lineEdit;
  QPushButton *pushButton;
};
  
}}