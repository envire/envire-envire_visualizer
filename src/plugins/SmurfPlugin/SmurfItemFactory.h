#pragma once
#include "envire_visualizer/plugins/ItemFactoryInterface.hpp"

namespace envire { namespace viz 
{
class SmurfItemFactory : public ItemFactoryInterface
{
public:
  SmurfItemFactory();

  virtual QWidget* getConfigurationWidget(const std::type_index& type);
  
  virtual void addItem(const std::type_index& type, std::shared_ptr<envire::core::EnvireGraph> graph,
                       const envire::core::FrameId& frame, const QWidget* configWidget);
  
  const std::vector<std::type_index>& getSupportedTypes();
private:
  std::vector<std::type_index> supportedTypes;
};
}}