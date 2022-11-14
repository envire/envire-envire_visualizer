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
#include <unordered_map>
#include <typeindex>
#if QT_VERSION >= 0x050000 || !defined(Q_MOC_RUN)
#include <vizkit3d/Vizkit3DPlugin.hpp>
#include <vizkit3d/Vizkit3DWidget.hpp>
#include <envire_core/graph/GraphTypes.hpp>
#include <envire_core/graph/TreeView.hpp>
#include <envire_core/events/GraphEventDispatcher.hpp>
#include <envire_core/items/ItemBase.hpp>
#include <envire_core/items/Transform.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>
#endif
#include <memory>
#include <unordered_map>
#include <mutex>
#if QT_VERSION >= 0x050000 || !defined(Q_MOC_RUN)
#include <vizkit3d_plugin_information/Vizkit3dPluginInformation.hpp>
#endif

namespace envire { namespace core 
{
  class EnvireGraph;
}}

namespace envire { namespace viz
{

/**Draws an envire graph into a Vizkit3DWidget */
class EnvireGraphVisualizer : public QObject, public  envire::core::GraphEventDispatcher
{
  Q_OBJECT
  using vertex_descriptor = envire::core::GraphTraits::vertex_descriptor;
  using edge_descriptor = envire::core::GraphTraits::edge_descriptor;
  using FrameId = envire::core::FrameId;
public:
  
  /**
   * @param rootNode the name of the frame that should be placed in the world origin*/
  EnvireGraphVisualizer(std::shared_ptr<envire::core::EnvireGraph> graph,
                        vizkit3d::Vizkit3DWidget* widget, 
                        const envire::core::FrameId& rootNode,
                        std::shared_ptr<vizkit3d::Vizkit3dPluginInformation> pluginInfos);
  
    /**Allows for lazy initialization */
    EnvireGraphVisualizer(vizkit3d::Vizkit3DWidget* widget,
                          std::shared_ptr<vizkit3d::Vizkit3dPluginInformation> pluginInfos);
    
    /** (re)-initializes the visualizer. Use this method to change the displayed
     *  graph*/
    void init(std::shared_ptr<envire::core::EnvireGraph> graph,
              const envire::core::FrameId& rootNode);
  
  ~EnvireGraphVisualizer();
  
  const QSet<QString>& getFrameNames() const;
  
  /**Returns a reference to the TreeView that is currently visualized. */
  const envire::core::TreeView& getTree() const;


  vizkit3d::VizPluginBase* getVizPluginForItem(const boost::uuids::uuid &uuid) {
    return itemVisuals[uuid];
  }

  
public slots:
    /**Get all transformation changes from the graph and redraw the graph.
     * @note expensive, don't call too often.
     * @note This method is not thread safe. Do **not** modify the graph while
     *       redrawing.
     */ 
    void redraw();

    void updateVisual(envire::core::ItemBase::Ptr item);
  
protected:
  /**Is invoked whenever a transform changes in the graph */
  virtual void edgeModified(const envire::core::EdgeModifiedEvent& e);
  
signals:
  void frameAdded(const QString& frame);
  void frameRemoved(const QString& frame);
  
private:
  void loadPlugins();
  
  /**Is invoked whenever a new edge is added to the current tree*/
  void edgeAddedToTree(vertex_descriptor origin, vertex_descriptor target);
  /**Is invoked whenever an edge is removed from the current tree */
  void edgeRemovedFromTree(const vertex_descriptor origin, const vertex_descriptor target);
  /**Is invoked whenever a new item is added to the graph*/
  virtual void itemAdded(const envire::core::ItemAddedEvent& e);
  /**Is invoked whenever an item is deleted from the graph */
  virtual void itemRemoved(const envire::core::ItemRemovedEvent& e);
  
  /**Display all items that are in @p vertex */
  void loadItems(const vertex_descriptor vertex);
  /**Display @p item */
  void loadItem(const envire::core::ItemBase::Ptr item);
  
  /**Adds @p name to frameNames and emits frameAdded*/
  void addFrameName(const QString& name);
  /**Removes @p name from frameNames and emits frameRemoved */
  void removeFrameName(const QString& name);
  
  /**removes all frame names from frameNames.
   * Removes the frames from the widget aswell.
   * Emits frameRemoved for each name */
  void clearFrames();
  
  /**Remove @p item from the Vizkit3dWidget */
  void removeItemPlugin(vizkit3d::VizPluginBase* item);
  /**Removes all item visuals from itemVisuals and from the widget */
  void clearItemVisuals();
  
  
  /**Gets the current transformation between @p origin and @p target from the
   * graph and sets it in the widget*/
  void setTransformation(const FrameId& origin, const FrameId& target);
  void setTransformation(const vertex_descriptor origin, const vertex_descriptor target);
  
  /**Converts a transformation from envire format to vizkit3d format */
  std::pair<QQuaternion, QVector3D> convertTransform(const envire::core::Transform& tf) const;
  
  using ItemVisualMap = std::unordered_map<boost::uuids::uuid, vizkit3d::VizPluginBase*, boost::hash<boost::uuids::uuid>>;

  std::shared_ptr<envire::core::EnvireGraph> graph; /**< the graph that is visualized*/
  envire::core::TreeView tree;
  vizkit3d::Vizkit3DWidget* widget; /**< Is used to display the graph */
  std::shared_ptr<vizkit3d::Vizkit3dPluginInformation> pluginInfos; /**< meta-data needed to figure out which plugins to load*/
  ItemVisualMap itemVisuals; /**<Map of all items that are currently visualized and the plugin visualizing them*/
  QSet<QString> frameNames; //contains the names of all frames in the current tree
  bool initialized;
  envire::core::FrameId rootId;
  
  std::mutex transformationsToUpdateMutex;
  /**Buffer that stores all transformations that need to be updated since the last redraw */
  using TransformToUpdateMap = std::unordered_map<std::pair<std::string, std::string>, envire::core::Transform,  boost::hash <std::pair <std::string, std::string>>>;
  TransformToUpdateMap transformationsToUpdate;
  
};

}}
