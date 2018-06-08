#pragma once
#include <string>
#include <envire_core/graph/EnvireGraph.hpp>
#include <envire_core/events/GraphEventDispatcher.hpp>

class EnvireVisualizerImpl;


class EnvireVisualizerInterface : public envire::core::GraphEventDispatcher
{
    EnvireVisualizerImpl* impl;
public:
    EnvireVisualizerInterface();
    virtual ~EnvireVisualizerInterface();
    virtual void displayGraph(envire::core::EnvireGraph& graph, const std::string& base);
    virtual void redraw();

    virtual void itemAdded(const envire::core::ItemAddedEvent& e);
    virtual void itemRemoved(const envire::core::ItemRemovedEvent& e);

    void show();
};

