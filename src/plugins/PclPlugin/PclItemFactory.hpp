#pragma once
#include "envire_visualizer/plugins/ItemFactoryInterface.hpp"
namespace envire { namespace viz 
{
class PclItemFactory : ItemFactoryInterface
{
public:
  PclItemFactory();
  envire::core::ItemBase::Ptr createItem(const std::type_index& type);
  const std::vector<std::type_index>& getSupportedTypes();
private:
  std::vector<std::type_index> supportedTypes;
};
}}