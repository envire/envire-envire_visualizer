#include "EnvireVisualizerInterface.hpp"
#include "EnvireVisualizerWindow.hpp"
#include <QApplication>
#include <memory>

using namespace envire::core;
using namespace envire::viz;

struct DontDeleteGraph // deleter that can be passed to std::shared_ptr ctor
{
    void operator()(EnvireGraph* graph) {}
};


class EnvireVisualizerImpl
{
    EnvireVisualizerWindow window;
    EnvireGraph* graph;
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

    void itemAdded(const envire::core::ItemAddedEvent& e)
    {
        e.item->connectContentsChangedCallback(
            [&](const ItemBase& item){ redraw(); });
    }

    void itemRemoved(const envire::core::ItemRemovedEvent& e)
    {
        // cannot remove lambda function
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

void EnvireVisualizerInterface::itemAdded(const envire::core::ItemAddedEvent& e)
{
    impl->itemAdded(e);
}

void EnvireVisualizerInterface::itemRemoved(const envire::core::ItemRemovedEvent& e)
{
    impl->itemRemoved(e);
}
