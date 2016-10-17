#include <iostream>
#include "EnvireVisualizerWidget.hpp"

using namespace vizkit3d;

struct EnvireVisualizerWidget::Data {
    // Copy of the value given to updateDataIntern.
    //
    // Making a copy is required because of how OSG works
    envire::core::EnvireGraph data;
};


EnvireVisualizerWidget::EnvireVisualizerWidget()
    : p(new Data)
{
}

EnvireVisualizerWidget::~EnvireVisualizerWidget()
{
    delete p;
}

osg::ref_ptr<osg::Node> EnvireVisualizerWidget::createMainNode()
{
    // Geode is a common node used for vizkit3d plugins. It allows to display
    // "arbitrary" geometries
    return new osg::Geode();
}

void EnvireVisualizerWidget::updateMainNode ( osg::Node* node )
{
    osg::Geode* geode = static_cast<osg::Geode*>(node);
    // Update the main node using the data in p->data
    
}

void EnvireVisualizerWidget::updateDataIntern(envire::core::EnvireGraph const& value)
{
    p->data = value;
    std::cout << "got new sample data" << std::endl;
}

//Macro that makes this plugin loadable in ruby, this is optional.
VizkitQtPlugin(EnvireVisualizerWidget)

