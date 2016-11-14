#include <QObject> //included at the top to fix an issue with the boost::foreach macro shadowing qt foreach (see http://stackoverflow.com/questions/15191361/boostq-foreach-has-not-been-declared)
#include "SmurfItemFactory.h"
#include <envire_core/items/Item.hpp>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <envire_core/items/Item.hpp>
#include <envire_smurf/Visual.hpp>
#include <envire_smurf/GraphLoader.hpp>
#include <smurf/Robot.hpp>
#include "SmurfWidget.hpp"
namespace envire { namespace viz 
{

    
    
SmurfItemFactory::SmurfItemFactory()
{
  envire::core::Item<envire::smurf::Visual> item;
  supportedTypes.emplace_back(*(item.getTypeInfo()));
}

QWidget* SmurfItemFactory::getConfigurationWidget(const std::type_index& type)
{
  return new SmurfWidget();
}

void SmurfItemFactory::addItem(const std::type_index& type, std::shared_ptr<envire::core::EnvireGraph> graph,
                             const envire::core::FrameId& frame, const QWidget* configWidget)
{
  if(type == supportedTypes[0])
  {
    
    const SmurfWidget* widget = dynamic_cast<const SmurfWidget*>(configWidget);
    assert(widget != nullptr);
    widget->getPath();
    
    envire::core::Transform robotTf;
    robotTf.setIdentity();
    envire::smurf::GraphLoader graphLoader(graph, robotTf);
    ::smurf::Robot* robot = new(::smurf::Robot);
    robot->loadFromSmurf(widget->getPath().toStdString());
    int nextGrpId = 0;
    graphLoader.loadStructure(graph->getVertex(frame), *robot);
    graphLoader.loadVisuals(*robot);
  }
}

const std::vector< std::type_index >& SmurfItemFactory::getSupportedTypes()
{
  return supportedTypes;
}

}}
