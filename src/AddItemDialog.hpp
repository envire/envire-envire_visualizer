#pragma once
#include <QDialog>
#include "ui_additemdialog.h"
#include <memory>
#include "plugins/ItemFactoryInterface.hpp"
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