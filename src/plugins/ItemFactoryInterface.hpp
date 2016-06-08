#pragma once
#include <envire_core/items/ItemBase.hpp>
#include <vector>
#include <type_traits>
namespace envire { namespace viz
{

class ItemFactoryInterface 
{
public:
  /**Create a new item of @p type.
   * This method is called in the gui thread.
   * When this method is called a QApplication exists. Thus it is safe to 
   * popup a QDialog to ask for user input.*/
  virtual envire::core::ItemBase::Ptr createItem(const std::type_index& type) = 0;
  
  /**Returns a vector of all types that this factory can create. */
  virtual const std::vector<std::type_index>& getSupportedTypes() = 0;
  
  virtual ~ItemFactoryInterface(){};
};

}}