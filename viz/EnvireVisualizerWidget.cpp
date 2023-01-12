//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

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
            p->visualizer.reset(new EnvireGraphVisualizer());
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
            p->visualizer->redraw();
        }
    }
}

