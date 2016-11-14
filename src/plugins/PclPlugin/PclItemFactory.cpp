#include <QObject> //included at the top to fix an issue with the boost::foreach macro shadowing qt foreach (see http://stackoverflow.com/questions/15191361/boostq-foreach-has-not-been-declared)
#include "PclItemFactory.hpp"
#include <envire_core/items/Item.hpp>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <pcl/io/pcd_io.h>
#include <envire_core/items/Item.hpp>
namespace envire { namespace viz 
{

PclItemFactory::PclItemFactory() : widget(new PclWidget())
{
  envire::core::Item<pcl::PCLPointCloud2> item;
  supportedTypes.emplace_back(*(item.getTypeInfo()));
}

QWidget* PclItemFactory::getConfigurationWidget(const std::type_index& type)
{
  return widget;
}

void PclItemFactory::addItem(const std::type_index& type, std::shared_ptr<envire::core::EnvireGraph> graph,
                             const envire::core::FrameId& frame, const QWidget* configWidget)
{
  if(type == supportedTypes[0])
  {
    envire::core::Item<pcl::PCLPointCloud2>::Ptr item(new envire::core::Item<pcl::PCLPointCloud2>());
    const QString pclFile = widget->getPclPath();
    ::pcl::PCDReader reader;
    if(QFile::exists(pclFile))
    {
      reader.read(pclFile.toStdString(), item->getData());
    }
    else
    {
      throw std::runtime_error("File does not exist: " + pclFile.toStdString());
    }
    graph->addItemToFrame(frame, item);
  }
}

const std::vector< std::type_index >& PclItemFactory::getSupportedTypes()
{
  return supportedTypes;
}

}}
