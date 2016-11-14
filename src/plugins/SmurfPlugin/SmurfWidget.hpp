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