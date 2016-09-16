#pragma once
#include <envire_core/items/ItemBase.hpp>
#include <envire_core/graph/EnvireGraph.hpp>
#include <envire_core/graph/TreeView.hpp>
class QWidget;


namespace envire { namespace viz
{
  
class ItemManipulatorFactoryInterface 
{
public:
  /** Create a new item manipulator widget.
   * @param selectedItem the item that the user selected
   * @param graph the graph that the selected item is part of.
   * @param treeView The currently displayed treeView.
   * @return a widget containing all manipulator options. The ownership of the
   *         widget is transfered to the caller.
   */
  virtual QWidget* create(envire::core::ItemBase::Ptr selectedItem,
                          std::shared_ptr<envire::core::EnvireGraph> graph,
                          const envire::core::TreeView& treeView) = 0;       
  
  /**Returns a vector of all types that this factory can create.
   * @note This should be the item types, not the types of the embedded data.
   *       E.g. if you want to support envire::pcl::Pointcloud this list should
   *       contain the result of envire::pcl::PointCloud::getTypeInfo() */
  virtual const std::vector<std::type_index>& getSupportedTypes() = 0;
  virtual ~ItemManipulatorFactoryInterface() {}
};

}}
