#ifndef envire_visualizer_EnvireVisualizerWidget_H
#define envire_visualizer_EnvireVisualizerWidget_H

#include <boost/noncopyable.hpp>
#include <vizkit3d/Vizkit3DPlugin.hpp>
#include <osg/Geode>
#include <envire_core/graph/EnvireGraph.hpp>

namespace vizkit3d
{
    class EnvireVisualizerWidget
        : public vizkit3d::Vizkit3DPlugin<envire_visualizer>
        , boost::noncopyable
    {
    Q_OBJECT
    public:
        EnvireVisualizerWidget();
        ~EnvireVisualizerWidget();

    Q_INVOKABLE void updateData(envire_visualizer const &sample)
    {vizkit3d::Vizkit3DPlugin<envire_visualizer>::updateData(sample);}

    protected:
        virtual osg::ref_ptr<osg::Node> createMainNode();
        virtual void updateMainNode(osg::Node* node);
        virtual void updateDataIntern(envire_visualizer const& plan);
        
    private:
        struct Data;
        Data* p;
    };
}
#endif
