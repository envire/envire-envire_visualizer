#include <QObject> //included at the top to fix an issue with the boost::foreach macro shadowing qt foreach (see http://stackoverflow.com/questions/15191361/boostq-foreach-has-not-been-declared)
#include "PclItemFactory.hpp"
#include <envire_pcl/PointCloud.hpp>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <pcl/io/pcd_io.h>
namespace envire { namespace viz 
{

PclItemFactory::PclItemFactory() : widget(new PclWidget())
{
  envire::pcl::PointCloud item;
  supportedTypes.emplace_back(*(item.getTypeInfo()));
}

QWidget* PclItemFactory::getConfigurationWidget(const std::type_index& type)
{
  return widget;
}
envire::core::ItemBase::Ptr PclItemFactory::createItem(const std::type_index& type,
                                                       const QWidget* configWidget)
{
  if(type == supportedTypes[0])
  {
    envire::pcl::PointCloud::Ptr item(new envire::pcl::PointCloud());
    const QString pclFile = widget->getPclPath();
    //FIXME check if file exists
    ::pcl::PCDReader reader;
    if(QFile::exists(pclFile))
    {
      reader.read(pclFile.toStdString(), item->getData());
    }
    else
    {
      //FIXME ERROR
    }
    return item;
  }
}

const std::vector< std::type_index >& PclItemFactory::getSupportedTypes()
{
  return supportedTypes;
}

}}
