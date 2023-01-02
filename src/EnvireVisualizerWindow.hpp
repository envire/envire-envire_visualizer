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

#pragma once
#include "TransformModel.hpp"
#include "ItemTableModel.hpp"
#include <QMainWindow>
#include <QListWidgetItem>
#include <memory>
#if QT_VERSION >= 0x050000 || !defined(Q_MOC_RUN)
#include <envire_core/events/GraphEventDispatcher.hpp>
#include <envire_core/graph/GraphTypes.hpp>
#endif
#include <unordered_map>
#include <mutex>
#include <chrono>

class QTableView;
class QTreeView;
class QListWidget;

namespace Ui
{
  class MainWindow;
}

namespace vizkit3d
{
    class Vizkit3DWidget;
    class Vizkit3dPluginInformation;
}


namespace envire { namespace core
{
  class EnvireGraph;
  class EdgeModifiedEvent;
}}

namespace envire { namespace viz
{

class EnvireGraphVisualizer;
class EnvireGraph2DStructurWidget;
class AddItemDialog;
class ItemManipulatorWidget;

class EnvireVisualizerWindow : public QMainWindow, public envire::core::GraphEventDispatcher
{
    Q_OBJECT
public:

    /**Create an unitialized envire visualizer that displays nothing.*/
    EnvireVisualizerWindow();

    /**called whenever some edge in the graph changes */
    virtual void edgeModified(const envire::core::EdgeModifiedEvent& e);

    /**Display the specified graph */
    void displayGraph(std::shared_ptr<envire::core::EnvireGraph> graph, const QString& rootNode);
    /**Load a graph and ask the user for the root node*/
    void displayGraph(const QString& filePath);

    /**Returns a shared_ptr to the currently displayed graph */
    std::shared_ptr<envire::core::EnvireGraph> getGraph() const;

    vizkit3d::Vizkit3DWidget* getWidget() { return vizkit3dWidget; }

    std::shared_ptr<EnvireGraphVisualizer> getVisualizer(){
        return visualzier;
    }


public slots:
    void addFrame();
    /**Remove the currently selected frame (if any) */
    void removeFrame();

    void frameNameAdded(const QString& name);
    void frameNameRemoved(const QString& name);

    /**Display a file picker dialog and load a graph from the selected file */
    void loadGraph();

    /**Display a file picker dialog and save the graph to the selected file */
    void storeGraph();

    /**Is called whenever the user wants to move a frame in the ui using the dragger */
    void frameMoved(const QString& frame, const QVector3D& trans, const QQuaternion& rot);
    void frameMoving(const QString& frame, const QVector3D& trans, const QQuaternion& rot);

    /**Redraw the graph structure. This has to be called after the graph structure has changed.
    * @note Only call if you know that the graph is not modifed at the same time.
    * @note Is thread safe in a sense that no two threads can call redraw at the same time*/
    void redraw();

private slots:
    void framePicked(const QString&);
    /**shows the add item dialog for the selected frame */
    void addItem();
    void listWidgetItemChanged(QListWidgetItem * current, QListWidgetItem * previous);
    /**Invoked if the user changes a transformation */
    void transformChanged(const envire::core::Transform& newValue);

    void displayGraphInternal(std::shared_ptr<envire::core::EnvireGraph> graph, const QString& rootNode);

    /**invoked by edgeModified() for thread switch */
    void edgeModifiedInternal(const QString& originFrame, const QString& tagetFrame);

    /**Display the transform from @p parent to @p selected in the transform display */
    void updateDisplayedTransform(const envire::core::GraphTraits::vertex_descriptor parent,
                                const envire::core::GraphTraits::vertex_descriptor selected,
                                const base::TransformWithCovariance& tf);

    /**Display the items of @p frame in the itemListWidget */
    void displayItems(const QString& frame);

    /**Invoked whenever the user clicks on an item */
    void itemClicked(const QModelIndex & index);

    void showStatistics();

signals:
    /**emitted when the widget is closed */
    void widgetClosed();

protected:
    void closeEvent(QCloseEvent *event);
private:

    /** @param finished if false, the movement is still ongoing, if true the movement is done */
    void internalFrameMoving(const QString& frame, const QVector3D& trans, const QQuaternion& rot,
                           bool finished);

    /**selects the frame named @p name in the 2d listview and 3d window */
    void selectFrame(const QString& name);


    std::shared_ptr<Ui::MainWindow> window;
    std::shared_ptr<envire::core::EnvireGraph> graph;
    std::shared_ptr<EnvireGraphVisualizer> visualzier;//is ptr for lazy instanziation
    std::shared_ptr<vizkit3d::Vizkit3dPluginInformation> pluginInfos;//is ptr for lazy instanziation
    QString selectedFrame;//currently selected frame, empty if none
    QString rootFrame;//the root frame of the displayed tree
    TransformModel currentTransform;//model of the currently selected transform
    ItemTableModel currentItems; //model of the  items of the current frame
    bool ignoreEdgeModifiedEvent;
    bool firstTimeDisplayingItems; //true if no items have been displayed, yet
    EnvireGraph2DStructurWidget* view2D; //widget inside the "2D View" tab
    AddItemDialog* addItemDialog;
    envire::core::ItemBase::Ptr selectedItem;
    ItemManipulatorWidget* itemManipulator;
    vizkit3d::Vizkit3DWidget* vizkit3dWidget;
    QTableView* tableViewItems;
    QTreeView* treeViewSelectedFrame;
    QListWidget* listWidgetFrames;
    QStatusBar* statusBar;

    std::mutex redrawMutex;

    std::chrono::system_clock::time_point lastStatisticTime;
    size_t numUpdates;
    size_t totalNumUpdates;

};


}}
