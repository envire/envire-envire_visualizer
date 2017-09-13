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

#include "EnvireVisualizerWindow.hpp"
#include "ui_mainwindow.h"
#include "EnvireGraphVisualizer.hpp"
#include "TransformModel.hpp"
#include "Helpers.hpp" 
#include <vizkit3d_plugin_information/Vizkit3dPluginInformation.hpp>
#include "DoubleSpinboxItemDelegate.hpp"
#include "AddTransformDialog.hpp"
#include "AddItemDialog.hpp"
#include <envire_core/graph/EnvireGraph.hpp>
#include <envire_core/events/EdgeEvents.hpp>
#include <envire_core/graph/GraphDrawing.hpp>

#include <QMessageBox>
#include <QInputDialog>
#include <QTreeView>
#include <QTableView>
#include <glog/logging.h>
#include <QListWidget>
#include <fstream>
#include "envire_core/EnvireGraph2DStructurWidget.hpp"
#include "ItemManipulatorWidget.hpp"
#include <vizkit3d/Vizkit3DWidget.hpp>

using namespace envire::core;  
using vertex_descriptor = GraphTraits::vertex_descriptor;

namespace envire { namespace viz
{
  
EnvireVisualizerWindow::EnvireVisualizerWindow(): QMainWindow(), GraphEventDispatcher(),
window(new Ui::MainWindow()), rootFrame(""), ignoreEdgeModifiedEvent(false),
firstTimeDisplayingItems(true)
{
  numUpdates = 0;
  totalNumUpdates = 0;
  window->setupUi(this);
  vizkit3dWidget = new vizkit3d::Vizkit3DWidget;
  tableViewItems = new QTableView(vizkit3dWidget);
  treeViewSelectedFrame = new QTreeView(vizkit3dWidget);
  listWidgetFrames = new QListWidget(vizkit3dWidget);
  
  window->tabWidget->addTab(vizkit3dWidget, "3D View");
  
  view2D = new EnvireGraph2DStructurWidget(1000);
  addItemDialog = new AddItemDialog(this); 
  itemManipulator = new ItemManipulatorWidget(this);
  
  
  QDockWidget* itemDock = new QDockWidget("Items", vizkit3dWidget);
  itemDock->setWidget(tableViewItems);
  
  QDockWidget* itemManipulatorDock = new QDockWidget("Item Manipulator", vizkit3dWidget);
  itemManipulatorDock->setWidget(itemManipulator);
  
  QDockWidget* selectedFrameDock = new QDockWidget("Selected Frame", vizkit3dWidget);
  selectedFrameDock->setWidget(treeViewSelectedFrame);
  
  QDockWidget* framesDock = new QDockWidget("Frames", vizkit3dWidget);
  framesDock->setWidget(listWidgetFrames);
  
  
  
  vizkit3dWidget->addDockWidget(Qt::BottomDockWidgetArea, itemManipulatorDock);
  vizkit3dWidget->addDockWidget(Qt::LeftDockWidgetArea, itemDock);
  vizkit3dWidget->addDockWidget(Qt::LeftDockWidgetArea,  selectedFrameDock);
  vizkit3dWidget->addDockWidget(Qt::LeftDockWidgetArea, framesDock);

  
  window->tabWidget->addTab(view2D, "2D View");
  
  treeViewSelectedFrame->setModel(&currentTransform);
  treeViewSelectedFrame->expandAll();
  DoubleSpinboxItemDelegate* del = new DoubleSpinboxItemDelegate(treeViewSelectedFrame);
  treeViewSelectedFrame->setItemDelegateForColumn(1, del);
  treeViewSelectedFrame->setItemDelegateForRow(2, treeViewSelectedFrame->itemDelegate()); //the row delegate will take precedence over the column delegate
  listWidgetFrames->setSortingEnabled(true);
  tableViewItems->setModel(&currentItems);//tableView will not take ownership
  tableViewItems->horizontalHeader()->setResizeMode(QHeaderView::Interactive);

  
  statusBar = new QStatusBar();
  setStatusBar(statusBar);
  statusBar->showMessage("test");
  
  lastStatisticTime = std::chrono::system_clock::now();
  
  connect(vizkit3dWidget, SIGNAL(frameSelected(const QString&)), this, SLOT(framePicked(const QString&)));
  connect(vizkit3dWidget, SIGNAL(frameMoved(const QString&, const QVector3D&, const QQuaternion)),
          this, SLOT(frameMoved(const QString&, const QVector3D&, const QQuaternion&)));          
  connect(vizkit3dWidget, SIGNAL(frameMoving(const QString&, const QVector3D&, const QQuaternion)),
          this, SLOT(frameMoving(const QString&, const QVector3D&, const QQuaternion&)));          
  connect(window->actionRemove_Frame, SIGNAL(activated(void)), this, SLOT(removeFrame()));
  connect(window->actionAdd_Frame, SIGNAL(activated(void)), this, SLOT(addFrame()));
  connect(window->actionLoad_Graph, SIGNAL(activated(void)), this, SLOT(loadGraph()));
  connect(window->actionSave_Graph, SIGNAL(activated(void)), this, SLOT(storeGraph()));
  connect(window->actionAdd_Item, SIGNAL(activated(void)), this, SLOT(addItem()));
  connect(listWidgetFrames, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
          this, SLOT(listWidgetItemChanged(QListWidgetItem*, QListWidgetItem*)));
  connect(&currentTransform, SIGNAL(transformChanged(const envire::core::Transform&)),
          this, SLOT(transformChanged(const envire::core::Transform&)));
  connect(tableViewItems, SIGNAL(clicked(const QModelIndex&)), this,
          SLOT(itemClicked(const QModelIndex&)));
  
  pluginInfos.reset(new vizkit3d::Vizkit3dPluginInformation(vizkit3dWidget));
  
  //disable everything until a graph is loaded
  treeViewSelectedFrame->setEnabled(false);
  listWidgetFrames->setEnabled(false);
  vizkit3dWidget->setEnabled(true);
  window->actionAdd_Frame->setEnabled(false);
  window->actionRemove_Frame->setEnabled(false);
  window->actionSave_Graph->setEnabled(false);
  window->actionAdd_Item->setEnabled(false);
  
}
  
void EnvireVisualizerWindow::redraw()
{
    std::lock_guard<std::mutex> lock(redrawMutex);
    if(visualzier)
    {
        visualzier->redraw();
    }
    
    if(graph)
    {
        std::stringstream stream;
        GraphDrawing::write(*graph, stream);
        
        const QString dotStr = QString::fromStdString(stream.str());
        QMetaObject::invokeMethod(view2D, "displayGraph", Qt::QueuedConnection,
                            Q_ARG(QString, dotStr));  
    }
    
    QMetaObject::invokeMethod(this, "showStatistics", Qt::QueuedConnection);  //redraw might be called from any thread
}

void EnvireVisualizerWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
    emit widgetClosed();
}
  
void EnvireVisualizerWindow::displayGraph(std::shared_ptr<envire::core::EnvireGraph> graph,
                              const QString& rootNode)
{
  const Qt::ConnectionType conType = Helpers::determineBlockingConnectionType(this);
  QMetaObject::invokeMethod(this, "displayGraphInternal", conType,
                            Q_ARG(std::shared_ptr<envire::core::EnvireGraph>, graph),
                            Q_ARG(QString, rootNode));  
}
  
void EnvireVisualizerWindow::displayGraphInternal(std::shared_ptr<envire::core::EnvireGraph> graph,
                              const QString& rootNode)
{ 
  if(this->graph)
  {
    this->graph->unsubscribe(this);
  }
  this->graph = graph;
  this->graph->subscribe(this);
  
  //reset the widget because this might not be the first time the user loads a graph
  vizkit3dWidget->clear();
  vizkit3dWidget->setWorldName(rootNode);
  vizkit3dWidget->setEnabledManipulators(true);
  
  visualzier.reset(new EnvireGraphVisualizer(graph, vizkit3dWidget,
                                             rootNode.toStdString(), pluginInfos));
  vizkit3dWidget->setRootFrame(rootNode);
  
  connect(visualzier.get(), SIGNAL(frameAdded(const QString&)), this,
          SLOT(frameNameAdded(const QString&)));
  connect(visualzier.get(), SIGNAL(frameRemoved(const QString&)), this,
          SLOT(frameNameRemoved(const QString&)));
    
  //get initially present frame names
  listWidgetFrames->clear();
  EnvireGraph::vertex_iterator it, end;
  std::tie(it, end) = graph->getVertices();
  for(; it != end; it++) 
  {
    const FrameId& id = graph->getFrameId(*it);
    listWidgetFrames->addItem(QString::fromStdString(id));
  }
     
  treeViewSelectedFrame->setEnabled(false); //leave disabled because initially no frame is selected
  listWidgetFrames->setEnabled(true);
  vizkit3dWidget->setEnabled(true);
  window->actionAdd_Frame->setEnabled(false);
  window->actionRemove_Frame->setEnabled(false); //leave disabled because initially no frame is selected
  window->actionSave_Graph->setEnabled(true);
  window->actionAdd_Item->setEnabled(false); //leave disabled because initially no frame is selected
  
  rootFrame = rootNode;
  selectedFrame = "";//otherwise we might try to unhighlight a no longer existing frame
  
  redraw();
}

void EnvireVisualizerWindow::displayGraph(const QString& filePath)
{
  try 
  {
    if(!QFile::exists(filePath))
    {
      LOG(ERROR) << "Cannot load graph. File doesn't exist: " << filePath.toStdString();
      return;
    }
    std::shared_ptr<EnvireGraph> pGraph(new EnvireGraph());
    pGraph->loadFromFile(filePath.toStdString());
    
    QStringList frames;
    EnvireGraph::vertex_iterator it, end;
    std::tie(it, end) = pGraph->getVertices();
    for(; it != end; it++)
    {
      const FrameId id = pGraph->getFrameId(*it);
      frames << QString::fromStdString(id);
    }

    bool ok;
    QString rootNode = QInputDialog::getItem(this, tr("Select World Frame"),
                                              tr("Frame:"), frames, 0, false, &ok);
    if (ok && !rootNode.isEmpty())
    {
      displayGraph(pGraph, rootNode);
    }
  }
  catch(const boost::archive::archive_exception& ex)
  {
    LOG(ERROR) << "Error while loading envire graph: " << ex.what();
  }
  catch(std::ios_base::failure& ex)
  {
    LOG(ERROR) << "Error while loading envire graph: " << ex.what();
  }
}

void EnvireVisualizerWindow::addFrame()
{
  AddTransformDialog dialog(this);
  if(dialog.exec() == QDialog::Accepted)
  {
    Transform tf(dialog.getTransform());
    const FrameId frame = dialog.getFrameId().toStdString();
    if(frame.size() > 0)
    {
      graph->addTransform(selectedFrame.toStdString(), frame, tf);
    }
  }
}

void EnvireVisualizerWindow::removeFrame()
{
  if(!selectedFrame.isEmpty())
  {
    const FrameId frameId = selectedFrame.toStdString();
    
    //has to be done before anything is removed, because some of the event handlers
    //depend on selectedFrame beeing valid
    selectedFrame = ""; 
    
    graph->disconnectFrame(frameId);
    graph->removeFrame(frameId);
    //this will trigger events, that will remove the frame from the list widget as well.
  }
}

void EnvireVisualizerWindow::framePicked(const QString& frame)
{
  selectFrame(frame);
}

void EnvireVisualizerWindow::listWidgetItemChanged(QListWidgetItem * current, QListWidgetItem * previous)
{
  //current is nullptr if the list is cleared
  if(current != nullptr)
    selectFrame(current->text());
}

void EnvireVisualizerWindow::selectFrame(const QString& name)
{  
  if(name != selectedFrame)
  {     
    //select in list widget
    QList<QListWidgetItem*> items = listWidgetFrames->findItems(name, Qt::MatchExactly);
    assert(items.size() == 1);
    if(!items.first()->isSelected())
      items.first()->setSelected(true);
    
    //display corresponding Transform
    const vertex_descriptor selectedVertex = graph->getVertex(name.toStdString());
    
    if(visualzier->getTree().tree.find(selectedVertex) == visualzier->getTree().tree.end())
    {
      LOG(ERROR) << "vertex not in tree: " << name.toStdString();
    }
    const vertex_descriptor parentVertex = visualzier->getTree().tree.at(selectedVertex).parent;
    Transform tf;
    if(parentVertex != graph->null_vertex())//happens when the root node is selected
      tf = graph->getTransform(parentVertex, selectedVertex);
    updateDisplayedTransform(parentVertex, selectedVertex, tf.transform);
    
    //user should not be able to delete the root frame
    if(name == rootFrame)
      window->actionRemove_Frame->setEnabled(false);
    else
      window->actionRemove_Frame->setEnabled(true);
    
    //are disabled if no frame was selected before
    window->actionAdd_Item->setEnabled(true);
    window->actionAdd_Frame->setEnabled(true);
    
    vizkit3dWidget->selectFrame(name, true);
    selectedFrame = name;
    displayItems(selectedFrame);
  }
}

void EnvireVisualizerWindow::updateDisplayedTransform(const vertex_descriptor parent,
                                          const vertex_descriptor selected,
                                          const base::TransformWithCovariance& tf)
{
  //disconnect before changing the transform model because changing the
  //value triggers events that are indistinguishable from user input
  disconnect(&currentTransform, SIGNAL(transformChanged(const envire::core::Transform&)),
      this, SLOT(transformChanged(const envire::core::Transform&)));
  if(parent != GraphTraits::null_vertex())
  {
    currentTransform.setTransform(tf);
    currentTransform.setEditable(true);
    treeViewSelectedFrame->setEnabled(true);
  }
  else
  { //this is the root node, it should not be edited
    currentTransform.setTransform(base::TransformWithCovariance());
    currentTransform.setEditable(false);
    treeViewSelectedFrame->setEnabled(false);
  }
  connect(&currentTransform, SIGNAL(transformChanged(const envire::core::Transform&)),
          this, SLOT(transformChanged(const envire::core::Transform&)));
}

void EnvireVisualizerWindow::frameNameAdded(const QString& name)
{
    std::cout << "ADDED: " << name.toStdString() << std::endl;
  listWidgetFrames->addItem(name);
}

void EnvireVisualizerWindow::frameNameRemoved(const QString& name)
{
  QList<QListWidgetItem*> items = listWidgetFrames->findItems(name, Qt::MatchExactly);
  assert(items.size() == 1); //the frame ids are unique and should not be in the list more than once
  delete items.first(); //this will remove the item from the listWidget
}

void EnvireVisualizerWindow::transformChanged(const envire::core::Transform& newValue)
{
  const vertex_descriptor selectedVertex = graph->getVertex(selectedFrame.toStdString());
  
  if(visualzier->getTree().tree.find(selectedVertex) == visualzier->getTree().tree.end())
  {
    LOG(ERROR) << "vertex not in tree: " << selectedFrame.toStdString();
  }
  
  const vertex_descriptor parentVertex = visualzier->getTree().tree.at(selectedVertex).parent;
  const FrameId source = graph->getFrameId(parentVertex);
  const FrameId target = selectedFrame.toStdString();
  ignoreEdgeModifiedEvent = true;
  graph->updateTransform(source, target, newValue);//will trigger EdgeModifiedEvent
  ignoreEdgeModifiedEvent = false;
}

void EnvireVisualizerWindow::edgeModified(const EdgeModifiedEvent& e)
{
    ++numUpdates;
    
    if(ignoreEdgeModifiedEvent)
        return;
    const QString origin = QString::fromStdString(e.origin);
    const QString target = QString::fromStdString(e.target);
    //need to invoke because the graph might have been modified from a different
    //thread
    const Qt::ConnectionType conType = Qt::QueuedConnection;//Helpers::determineConnectionType(this);
    QMetaObject::invokeMethod(this, "edgeModifiedInternal", conType,
                              Q_ARG(QString, origin), Q_ARG(QString, target));  
}

void EnvireVisualizerWindow::edgeModifiedInternal(const QString& originFrame, const QString& targetFrame)
{
  vertex_descriptor originVertex = graph->getVertex(originFrame.toStdString());
  vertex_descriptor targetVertex = graph->getVertex(targetFrame.toStdString());
  const TreeView& tree = visualzier->getTree();
  
  if(tree.vertexExists(originVertex) && tree.vertexExists(targetVertex))
  {
    if(selectedFrame == originFrame)
    {
      if(tree.isParent(targetVertex, originVertex))
      {
        const Transform tf = graph->getTransform(targetVertex, originVertex);
        updateDisplayedTransform(targetVertex, originVertex, tf.transform);
      }
    }
    else if(selectedFrame == targetFrame)
    {
      if(tree.isParent(originVertex, targetVertex))
      {
        const Transform tf = graph->getTransform(originVertex, targetVertex);
        updateDisplayedTransform(originVertex, targetVertex, tf.transform);
      }
    }
  }  
}

void EnvireVisualizerWindow::loadGraph()
{
  //DontUseNativeDialog is used because the native dialog on xfce hangs and crashes...
  const QString file = QFileDialog::getOpenFileName(this, tr("Load Envire Graph"),
                                                    QDir::currentPath(), QString(),
                                                    0, QFileDialog::DontUseNativeDialog);
  if(!file.isEmpty())
  {
    LOG(INFO) << "Loading graph from " << file.toStdString();
    displayGraph(file);
  }
}

void EnvireVisualizerWindow::storeGraph()
{
  //DontUseNativeDialog is used because the native dialog on xfce hangs and crashes...
  const QString file = QFileDialog::getSaveFileName(this, tr("Save Envire Graph"),
                                                    QDir::currentPath(), QString(),
                                                    0, QFileDialog::DontUseNativeDialog);

  if(!file.isEmpty())
  {
    LOG(INFO) << "Saving graph to " << file.toStdString();
    if(graph)
    {
      graph->saveToFile(file.toStdString());
    }
  }
}

void EnvireVisualizerWindow::displayItems(const QString& frame)
{
  const FrameId frameId = frame.toStdString();
  currentItems.clear();
  itemManipulator->clearSelection();
  bool visited = false;//changed to true if at least one item is visited
  graph->visitItems(frameId, [this, &visited] (const ItemBase::Ptr item)
  {
    this->currentItems.addItem(item);
    visited = true;
  });
  
  //resize the table on the first time it has some content.
  //afterwards the user may change it and we not want to override the users
  //size choice
  if(firstTimeDisplayingItems && visited)
  {
    firstTimeDisplayingItems = false;
    tableViewItems->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
  }
  
}

void EnvireVisualizerWindow::internalFrameMoving(const QString& frame, const QVector3D& trans, const QQuaternion& rot,
                                     bool finished)
{
  const vertex_descriptor movedVertex = graph->getVertex(frame.toStdString());
  if(movedVertex != graph->null_vertex() && visualzier->getTree().vertexExists(movedVertex))
  {
    const vertex_descriptor parentVertex = visualzier->getTree().tree.at(movedVertex).parent;
    if(parentVertex != graph->null_vertex())
    {
      Transform tf = graph->getTransform(parentVertex, movedVertex);
      const base::Vector3d translation(trans.x(), trans.y(), trans.z());
      const base::Quaterniond rotation(rot.scalar(), rot.x(), rot.y(), rot.z());
      const base::Quaterniond delta = (tf.transform.orientation * rotation) * tf.transform.orientation.inverse();
      tf.transform.orientation = delta * tf.transform.orientation;
      tf.transform.translation = tf.transform.orientation * translation + tf.transform.translation;
      
      if(finished)
        graph->updateTransform(parentVertex, movedVertex, tf);
      
      updateDisplayedTransform(parentVertex, movedVertex, tf.transform);
    }
    else
    {
      LOG(ERROR) << "Tried to move the root vertex, this should not be possible";
    }
  }
  else
  {
    LOG(ERROR) << "Moved a frame that is not part of the graph: " << frame.toStdString();
  }  
}

void EnvireVisualizerWindow::frameMoved(const QString& frame, const QVector3D& trans, const QQuaternion& rot)
{
  internalFrameMoving(frame, trans, rot, true);
}

void EnvireVisualizerWindow::frameMoving(const QString& frame, const QVector3D& trans, const QQuaternion& rot)
{
  internalFrameMoving(frame, trans, rot, false);
}

std::shared_ptr<EnvireGraph> EnvireVisualizerWindow::getGraph() const
{
  return graph;
}

void EnvireVisualizerWindow::addItem()
{
  assert(!selectedFrame.isEmpty());
  addItemDialog->addItem(graph, selectedFrame);
}

void EnvireVisualizerWindow::itemClicked(const QModelIndex & index)
{
  ItemBase::Ptr item = currentItems.getItem(index);
 
  itemManipulator->itemSelected(item, graph, visualzier->getTree());
}

void EnvireVisualizerWindow::showStatistics()
{
    const auto now = std::chrono::system_clock::now();
    if(graph)
    {
        const double t = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastStatisticTime).count();
        
        if(t >= 1000)
        {
            const double updatesSec = double(numUpdates) / t * 1000.0;
            totalNumUpdates += numUpdates;
            statusBar->showMessage(QString("Frames: ") + QString::number(graph->num_vertices()) +
                                   QString(", Total Updates: ") + QString::number(totalNumUpdates) +
                                   QString(", Updates/Sec: ") + QString::number(updatesSec));
            numUpdates = 0; //should be locked but dont care because it is not important of we lose one or two updates
            lastStatisticTime = now;
        }
    }
    else
    {
        statusBar->showMessage("No Graph");
    }
}


}}