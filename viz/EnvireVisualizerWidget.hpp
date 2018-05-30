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

#ifndef envire_visualizer_EnvireVisualizerWidget_H
#define envire_visualizer_EnvireVisualizerWidget_H

#if QT_VERSION >= 0x050000 || !defined(Q_MOC_RUN)
#include <boost/noncopyable.hpp>
#endif
#include <vizkit3d/Vizkit3DPlugin.hpp>
#include <osg/Geode>
#if QT_VERSION >= 0x050000 || !defined(Q_MOC_RUN)
#include <envire_core/graph/EnvireGraph.hpp>
#endif

namespace vizkit3d
{
    class EnvireVisualizerWidget
        : public vizkit3d::Vizkit3DPlugin<envire::core::EnvireGraph>
        , boost::noncopyable
    {
    Q_OBJECT
    public:
        EnvireVisualizerWidget();
        ~EnvireVisualizerWidget();

    Q_INVOKABLE void updateData(envire::core::EnvireGraph const &sample)
    {vizkit3d::Vizkit3DPlugin<envire::core::EnvireGraph>::updateData(sample);}
    
    protected:
        virtual osg::ref_ptr<osg::Node> createMainNode();
        virtual void updateMainNode(osg::Node* node);
        virtual void updateDataIntern(envire::core::EnvireGraph const& plan);
        
    private:
        struct Data;
        Data* p;
    };
}
#endif
