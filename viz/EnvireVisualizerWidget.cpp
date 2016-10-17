#include <iostream>
#include "EnvireVisualizerWidget.hpp"
#include <envire_visualizer/EnvireGraphVisualizer.hpp>
#include <memory>

using namespace vizkit3d;
using namespace envire::core;
using namespace std;
using namespace envire::viz;

struct EnvireVisualizerWidget::Data {
    shared_ptr<EnvireGraph> graph;
    shared_ptr<EnvireGraphVisualizer> visualizer;
    shared_ptr<Vizkit3dPluginInformation> pluginInfos;
    bool initialized = false;
    
};



EnvireVisualizerWidget::EnvireVisualizerWidget()
    : p(new Data)
{}

EnvireVisualizerWidget::~EnvireVisualizerWidget()
{
    delete p;
}

osg::ref_ptr<osg::Node> EnvireVisualizerWidget::createMainNode()
{
    return new osg::Geode();
}

void EnvireVisualizerWidget::updateMainNode ( osg::Node* node )
{
    osg::Geode* geode = static_cast<osg::Geode*>(node);
    // Update the main node using the data in p->data
    
}

void EnvireVisualizerWidget::updateDataIntern(envire::core::EnvireGraph const& value)
{
    if(!p->initialized)
    {
        //wait for the widget to become fully initialized. depending on who created it this might never happen
        if(getWidget() != nullptr) 
        {
            p->graph.reset(new EnvireGraph);
            p->pluginInfos.reset(new Vizkit3dPluginInformation(getWidget()));
            p->visualizer.reset(new EnvireGraphVisualizer(getWidget(), p->pluginInfos));
            p->initialized = true;
        }
    }
    
    if(p->initialized)
    {
        *(p->graph.get()) = value;
        if(p->graph->num_vertices())
        {
            const FrameId root = p->graph->getFrameId(*(p->graph->getVertices().first));
            //FIXME the user should be able to choose the root node
            //FIXME reinitializing every time is probably really expensive
            p->visualizer->init(p->graph, root);
        }
    }
}

//Macro that makes this plugin loadable in ruby, this is optional.
VizkitQtPlugin(EnvireVisualizerWidget)

