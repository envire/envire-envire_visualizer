#include "EnvireVisualizerInterface.hpp"
#include "EnvireVisualizerWindow.hpp"
#include "EnvireGraphVisualizer.hpp"
#include <QApplication>
#include <memory>
#include <envire_core/events/GraphEventDispatcher.hpp>

using namespace envire::core;
using namespace envire::viz;

//To use the envire::core::ItemBase::Ptr in signal/slot conenctions, we need to register the (typedef) type
Q_DECLARE_METATYPE(envire::core::ItemBase::Ptr)

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

    virtual void edgeModified(const envire::core::EdgeModifiedEvent& e) {
        parent->edgeModified(e);
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
        qRegisterMetaType<envire::core::ItemBase::Ptr>("envire::core::ItemBase::Ptr");
    }

    virtual ~EnvireVisualizerImpl(){
        graph = NULL;
    }

    void displayGraph(EnvireGraph& graph, const std::string& base)
    {
        this->graph = &graph;
        std::shared_ptr<EnvireGraph> graphPtr(&graph, DontDeleteGraph());

        //add callbacks for items already in the graph
        graph.visitVertices([&](GraphTraits::vertex_descriptor vd) 
        {
            Frame& f = graph[graph.getFrameId(vd)];
            f.visitItems([&](const ItemBase::Ptr& item){
                    item->connectContentsChangedCallback([&](ItemBase& item){updateViz(item);});
                });
        });

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
          updateViz(*e.item);
    }

    virtual void itemRemoved(const envire::core::ItemRemovedEvent& e)
    {
        // cannot remove lambda function
    }

    virtual void edgeModified(const envire::core::EdgeModifiedEvent& e){
        redraw();
    }

    void updateViz(ItemBase& item){
        //update item through invoke to make the update thread save (happens in QT main Loop)
        //calls the slot without connection
        envire::core::ItemBase::Ptr itemptr = item.clone();
        QMetaObject::invokeMethod( window.getVisualizer().get(), "updateVisual", Q_ARG( envire::core::ItemBase::Ptr, itemptr ) );
    }

    EnvireVisualizerWindow& getEnvireVisualizerWindow(){
        return window;
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

EnvireVisualizerWindow& EnvireVisualizerInterface::getEnvireVisualizerWindow(){
    return impl->getEnvireVisualizerWindow();
}