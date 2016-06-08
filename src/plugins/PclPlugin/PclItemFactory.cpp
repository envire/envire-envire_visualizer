#include "PclItemFactory.hpp"

namespace envire { namespace viz 
{

PclItemFactory::PclItemFactory()
{
//   supportedTypes.emplace_back(typeid());
}


envire::core::ItemBase::Ptr PclItemFactory::createItem(const std::type_index& type)
{

}

const std::vector< std::type_index >& PclItemFactory::getSupportedTypes()
{
  return supportedTypes;
}

}}
