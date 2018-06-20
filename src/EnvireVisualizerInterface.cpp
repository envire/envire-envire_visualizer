#include "EnvireVisualizerInterface.hpp"
#include "EnvireVisualizerWindow.hpp"
#include "EnvireGraphVisualizer.hpp"
#include <QApplication>
#include <memory>
#include <envire_core/events/GraphEventDispatcher.hpp>

using namespace envire::core;
using namespace envire::viz;

struct DontDeleteGraph // deleter that can be passed to std::shared_ptr ctor
{
    void operator()(EnvireGraph* graph) {}
};


struct ItemCallbackSubscriber : public GraphEventDispatcher
{
    ItemCallbackSubscriber(EnvireGraph& graph, FrameId targetFrame, EnvireVisualizerInterfaceCallbackReceiver* parent) : 
      GraphEventDispatcher(&graph),
      targetFrame(targetFrame),
      parent(parent){}

    virtual void itemAdded(const envire::core::ItemAddedEvent& event)
    {
        parent->itemAdded(event);
    }
    
    virtual void itemRemoved(const envire::core::ItemRemovedEvent& event)
    {
        parent->itemRemoved(event);
    }

    FrameId targetFrame;
    EnvireVisualizerInterfaceCallbackReceiver* parent;
    
};


class EnvireVisualizerImpl: public EnvireVisualizerInterfaceCallbackReceiver
{
    EnvireVisualizerWindow window;
    EnvireGraph* graph;
    boost::shared_ptr<ItemCallbackSubscriber> subscriber;
public:
    EnvireVisualizerImpl() : graph(0)
    {
    }

    ~EnvireVisualizerImpl(){
        graph = NULL;
    }

    void displayGraph(EnvireGraph& graph, const std::string& base)
    {
        this->graph = &graph;
        std::shared_ptr<EnvireGraph> graphPtr(&graph, DontDeleteGraph());
        window.displayGraph(graphPtr, QString::fromStdString(base));
        subscriber = boost::shared_ptr<ItemCallbackSubscriber> (new ItemCallbackSubscriber(graph, base, this));
    }

    void redraw()
    {
        window.redraw();
    }

    void show()
    {
        window.show();
        QApplication::instance()->connect(
            &window, SIGNAL(widgetClosed()),
            QApplication::instance(), SLOT(quit()));
    }

    virtual void itemAdded(const envire::core::ItemAddedEvent& e)
    {
          //e.item->connectContentsChangedCallback([&](ItemBase& item){ redraw(););
          e.item->connectContentsChangedCallback([&](ItemBase& item){updateViz(item);});
    }

    virtual void itemRemoved(const envire::core::ItemRemovedEvent& e)
    {
        // cannot remove lambda function
    }

    void updateViz(ItemBase& item){
        window.getVisualizer()->updateVisual(item);
    }

};


EnvireVisualizerInterface::EnvireVisualizerInterface() : impl(new EnvireVisualizerImpl)
{
}

EnvireVisualizerInterface::~EnvireVisualizerInterface()
{
    delete impl;
}

void EnvireVisualizerInterface::displayGraph(EnvireGraph& graph, const std::string& base)
{
    impl->displayGraph(graph, base);
}

void EnvireVisualizerInterface::redraw()
{
    impl->redraw();
}

void EnvireVisualizerInterface::show()
{
    // TODO make sure graph is initialized?
    impl->show();
}

// void EnvireVisualizerInterface::itemAdded(const envire::core::ItemAddedEvent& e)
// {
//     impl->itemAdded(e);
// }

// void EnvireVisualizerInterface::itemRemoved(const envire::core::ItemRemovedEvent& e)
// {
//     impl->itemRemoved(e);
// }
