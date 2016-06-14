#pragma once
#include <envire_core/items/ItemBase.hpp>
#include <vector>
#include <type_traits>

class QWidget;

namespace envire { namespace viz
{

class ItemFactoryInterface 
{
public:
  
  
  /**Returns a widget used for configurating the item. The widget will be passed
   * to the createItem() method later on*/
  virtual QWidget* getConfigurationWidget(const std::type_index& type) = 0;
  
  /**Create a new item of @p type.
   * This method is called in the gui thread.
   * When this method is called a QApplication exists.
   * @return a new instance of the requested type or nullptr if the type is
   *         not supported.*/
  virtual envire::core::ItemBase::Ptr createItem(const std::type_index& type,
                                                 const QWidget* configWidget) = 0;
  
  /**Returns a vector of all types that this factory can create.
   * @note This should be the item types, not the types of the embedded data.
   *       E.g. if you want to support envire::pcl::Pointcloud this list should
   *       contain the result of envire::pcl::PointCloud::getTypeInfo() */
  virtual const std::vector<std::type_index>& getSupportedTypes() = 0;
  
  virtual ~ItemFactoryInterface(){};
};

}}