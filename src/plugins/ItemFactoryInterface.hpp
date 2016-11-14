#pragma once
#include <envire_core/graph/EnvireGraph.hpp>
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
  
  /**Create a new item of @p type and add it to @p frame in @p graph.
   * This method is called in the gui thread.
   * When this method is called a QApplication exists.
   * */
  virtual void addItem(const std::type_index& type, std::shared_ptr<envire::core::EnvireGraph> graph,
                       const envire::core::FrameId& frame, const QWidget* configWidget) = 0;
  
  /**Returns a vector of all types that this factory can create.
   * @note This should be the item types, not the types of the embedded data.
   *       E.g. if you want to support envire::pcl::Pointcloud this list should
   *       contain the result of envire::pcl::PointCloud::getTypeInfo() */
  virtual const std::vector<std::type_index>& getSupportedTypes() = 0;
  
  virtual ~ItemFactoryInterface(){};
};

}}