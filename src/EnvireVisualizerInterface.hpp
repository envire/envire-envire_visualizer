#pragma once
#include <string>
#include <envire_core/graph/EnvireGraph.hpp>

class EnvireVisualizerImpl;


class EnvireVisualizerInterface
{
    EnvireVisualizerImpl* impl;
public:
    EnvireVisualizerInterface();
    virtual ~EnvireVisualizerInterface();
    virtual void displayGraph(envire::core::EnvireGraph& graph, const std::string& base);
    virtual void redraw();
    void show();
};

class EnvireVisualizerInterfaceCallbackReceiver{
    public:
    virtual void itemAdded(const envire::core::ItemAddedEvent& e) = 0;
    virtual void itemRemoved(const envire::core::ItemRemovedEvent& e) = 0;
    virtual void edgeModified(const envire::core::EdgeModifiedEvent& e) = 0;
};