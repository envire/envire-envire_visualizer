#include <envire_visualizer/EnvireVisualizerWindow.hpp>
#include <envire_core/graph/EnvireGraph.hpp>
#include <QApplication>

using namespace envire::core;
using namespace envire::viz;

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  EnvireVisualizerWindow window;
  std::shared_ptr<EnvireGraph> graph(new EnvireGraph);
  graph->addFrame("Root");
  window.displayGraph(graph, "Root");
  window.show();
  app.exec();
  return 0;
}