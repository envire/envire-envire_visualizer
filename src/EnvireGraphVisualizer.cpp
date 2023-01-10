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

#include "EnvireGraphVisualizer.hpp"
#include <envire_core/graph/EnvireGraph.hpp>
#include <envire_core/events/EdgeEvents.hpp>
#include <envire_core/items/ItemMetadata.hpp>
#include "Helpers.hpp"
#include <string>
#include <glog/logging.h>
#include <chrono>

using namespace envire::core;
using namespace vizkit3d;

namespace envire
{
    namespace viz
    {
        using TypeToUpdateMapping = vizkit3d::Vizkit3dPluginInformation::TypeToUpdateMapping;

        EnvireGraphVisualizer::EnvireGraphVisualizer() : initialized(false)
        {
            pluginInfos.reset(new vizkit3d::Vizkit3dPluginInformation(this));
        }

        void EnvireGraphVisualizer::init(std::shared_ptr<EnvireGraph> graph, const FrameId &rootNode)
        {
            this->clear();
            this->setWorldName(QString(rootNode.c_str()));
            this->setRootFrame(QString(rootNode.c_str()));

            if (initialized)
            {
                // clear old stuff before re-initializing
                tree.unsubscribe();
                tree.clear();
                clearFrames();
                clearItemVisuals();
            }
            else
            {
                // only do this upon first initialization
                auto edgeAdded = std::bind(&EnvireGraphVisualizer::edgeAddedToTree, this,
                                           std::placeholders::_1, std::placeholders::_2);
                tree.edgeAdded.connect(edgeAdded);

                auto edgeRemoved = std::bind(&EnvireGraphVisualizer::edgeRemovedFromTree, this,
                                             std::placeholders::_1, std::placeholders::_2);
                tree.edgeRemoved.connect(edgeRemoved);
            }

            rootId = rootNode;
            addFrameName(QString::fromStdString(rootNode));

            this->graph = graph;
            this->graph->subscribe(this);
            // will cause edgeAdded events which will be handled by EnvireGraphVisualizer::edgeAddedToTree
            graph->getTree(rootNode, true, &tree);

            initialized = true;
        }

        void EnvireGraphVisualizer::edgeAddedToTree(vertex_descriptor origin, vertex_descriptor target)
        {
            setTransformation(origin, target);
            // Since we are drawing a tree structure every "origin", has been a "target" in
            // a previous call (except the root node). Thus we only need to load items
            // from the origin if it is the root.
            if (tree.isRoot(origin))
            {
                loadItems(origin);
                addFrameName(QString::fromStdString(graph->getFrameId(origin)));
            }
            loadItems(target);
            addFrameName(QString::fromStdString(graph->getFrameId(target)));

            LOG(INFO) << "Added edge " << graph->getFrameId(origin) << " -- " << graph->getFrameId(target);
            redraw();
        }

        void EnvireGraphVisualizer::edgeRemovedFromTree(const vertex_descriptor origin, const vertex_descriptor target)
        {
            const QString targetId = QString::fromStdString(graph->getFrameId(target));
            Qt::ConnectionType conType = Helpers::determineBlockingConnectionType(this);
            QMetaObject::invokeMethod(this, "removeFrame", conType, Q_ARG(QString, targetId));

            removeFrameName(QString::fromStdString(graph->getFrameId(target)));

            LOG(INFO) << "Removed edge " << graph->getFrameId(origin) << " -- " << graph->getFrameId(target);
        }

        void EnvireGraphVisualizer::itemAdded(const envire::core::ItemAddedEvent &e)
        {
            std::cout << "[EnvireGraphVisualizer::itemAdded] " << e.frame << std::endl;
            const GraphTraits::vertex_descriptor vd = graph->getVertex(e.frame);
            // if the vertex that the item was added to is part of the current tree
            if (tree.vertexExists(vd))
            {
                loadItem(e.item);
            }
        }

        void EnvireGraphVisualizer::itemRemoved(const envire::core::ItemRemovedEvent &e)
        {
            const GraphTraits::vertex_descriptor vd = graph->getVertex(e.frame);
            if (tree.vertexExists(vd))
            {
                if (itemVisuals.find(e.item->getID()) == itemVisuals.end())
                {
                    LOG(ERROR) << "No item visual for id: " << e.item->getIDString();
                    return;
                }
                VizPluginBase *itemViz = itemVisuals.at(e.item->getID()); // may throw
                itemVisuals.erase(e.item->getID());
                removeItemPlugin(itemViz);
                LOG(INFO) << "Removed item " << e.item->getIDString();
            }
        }

        void EnvireGraphVisualizer::clearItemVisuals()
        {
            while (itemVisuals.size() > 0)
            {
                removeItemPlugin(itemVisuals.begin()->second);
                itemVisuals.erase(itemVisuals.begin());
            }
        }

        void EnvireGraphVisualizer::removeItemPlugin(VizPluginBase *itemViz)
        {
            ASSERT_NOT_NULL(itemViz);
            Qt::ConnectionType conType = Helpers::determineBlockingConnectionType(this);
            QMetaObject::invokeMethod(this, "removePlugin", conType, Q_ARG(QObject *, itemViz));
        }

        std::pair<QQuaternion, QVector3D> EnvireGraphVisualizer::convertTransform(const Transform &tf) const
        {
            // normalizing is important, otherwise the transfirmation in osg will break when switching the root.
            const base::Quaterniond &rot = tf.transform.orientation.normalized();
            const base::Position &pos = tf.transform.translation;
            const QQuaternion orientation(rot.w(), rot.x(), rot.y(), rot.z());
            const QVector3D translation(pos.x(), pos.y(), pos.z());
            return std::make_pair(orientation, translation);
        }

        void EnvireGraphVisualizer::loadItems(const vertex_descriptor vertex)
        {
            const FrameId frame = graph->getFrameId(vertex);

            graph->visitItems(frame, [this](const ItemBase::Ptr item)
                              { loadItem(item); });
        }

        void EnvireGraphVisualizer::loadItem(const envire::core::ItemBase::Ptr item)
        {
            LOG(INFO) << "Loading Visualizer for item: " << item->getIDString() << " of type " << demangledTypeName(*item);
            if (itemVisuals.find(item->getID()) != itemVisuals.end())
            {
                LOG(ERROR) << "Ignoring item " << item->getIDString() << ". It already has a visual.";
                return;
            }

            const bool hasMetadata = ItemMetadataMapping::containsMetadata(*(item->getTypeInfo()));
            if (!hasMetadata)
            {
                LOG(ERROR) << "Ignoring item " << item->getIDString() << ". No metadata available. This usually means that the type was not loaded using envire plugins";
                return;
            }

            updateVisual(item);
        }

        void EnvireGraphVisualizer::edgeModified(const EdgeModifiedEvent &e)
        {
            // only update if this is an edge of the current tree and not a cross-edge
            const vertex_descriptor origin = graph->getVertex(e.origin);
            const vertex_descriptor target = graph->getVertex(e.target);

            if (tree.edgeExists(origin, target))
            {
                // transformation changes are buffered
                // so the transformation changes will be not applied to visualization
                // you are required to call redraw to see the changes
                setTransformation(origin, target);
                // TOFO: temporarly call redraw to work with mars2
                redraw();
            }
        }

        void EnvireGraphVisualizer::setTransformation(const vertex_descriptor origin,
                                                      const vertex_descriptor target)
        {
            setTransformation(graph->getFrameId(origin), graph->getFrameId(target));
        }

        void EnvireGraphVisualizer::setTransformation(const FrameId &origin, const FrameId &target)
        {
            std::lock_guard<std::mutex> lock(transformationsToUpdateMutex);
            // transformation changes are buffered and applied when redraw() is called
            // this is done because some users make up to 22k transformation changes and
            // direct redrawing is simply not possible with those numbers
            transformationsToUpdate[std::make_pair(origin, target)] = graph->getTransform(origin, target);
        }

        void EnvireGraphVisualizer::redraw()
        {
            // copy set to keep lock time as little as possible
            transformationsToUpdateMutex.lock();
            TransformToUpdateMap updateMap(transformationsToUpdate);
            transformationsToUpdate.clear();
            transformationsToUpdateMutex.unlock();

            for (const auto updatePair : updateMap)
            {
                const FrameId &origin = updatePair.first.first;
                const FrameId &target = updatePair.first.second;

                const Transform &tf = updatePair.second;
                QQuaternion rot;
                QVector3D trans;
                std::tie(rot, trans) = convertTransform(tf);
                const QString qSrc = QString::fromStdString(origin);
                const QString qTarget = QString::fromStdString(target);

                // needs to be invoked because we might have been called from the non-gui thread
                QMetaObject::invokeMethod(this, "setTransformation", Qt::QueuedConnection,
                                          Q_ARG(QString, qSrc), Q_ARG(QString, qTarget),
                                          Q_ARG(QVector3D, trans), Q_ARG(QQuaternion, rot));
            }
        }

        void EnvireGraphVisualizer::updateVisual(envire::core::ItemBase::Ptr item)
        {

            const std::string parameterType = ItemMetadataMapping::getMetadata(*item->getTypeInfo()).embeddedTypename;
            const QString qParameterType = QString::fromStdString(parameterType);
            const TypeToUpdateMapping &typeToUpdateMethod = pluginInfos->getTypeToUpdateMethodMapping();
            const Qt::ConnectionType conType = Helpers::determineBlockingConnectionType(this);

            TypeToUpdateMapping::ConstIterator it = typeToUpdateMethod.find(qParameterType);

            if (typeToUpdateMethod.count(qParameterType) > 1)
            {
                LOG(WARNING) << "Multiple update methods registered for type "
                             << parameterType << ". Using the most recently added one from: "
                             << it->libName.toStdString();
            }

            if (it != typeToUpdateMethod.end())
            {
                const Vizkit3dPluginInformation::UpdateMethod &info = it.value();

                VizPluginBase *vizPlugin = itemVisuals[item->getID()];
                // check if plugin already loaded, if not ty to
                if (!vizPlugin)
                {
                    QObject *plugin = nullptr;

                    QMetaObject::invokeMethod(this, "loadPlugin", conType,
                                              Q_RETURN_ARG(QObject *, plugin),
                                              Q_ARG(QString, info.libName), Q_ARG(QString, ""));
                    ASSERT_NOT_NULL(plugin); // loading should never fail (has been loaded successfully before)
                    vizPlugin = dynamic_cast<VizPluginBase *>(plugin);
                    ASSERT_NOT_NULL(vizPlugin); // everything loaded with vizkit should inherit from VizPluginBase

                    const QString qFrame = QString::fromStdString(item->getFrame());

                    // needs to be invoked because setting the data frame while rendering crashes vizkit3d
                    QMetaObject::invokeMethod(vizPlugin, "setVisualizationFrame", conType,
                                              Q_ARG(QString, qFrame));

                    if (qParameterType.toStdString() == "smurf::Collidable")
                        vizPlugin->setPluginEnabled(false);

                    itemVisuals[item->getID()] = vizPlugin;
                    LOG(INFO) << "Added item " << item->getIDString() << " using vizkit plugin " << info.libName.toStdString();
                }
                else
                {
                }
                // call the updateData method
                // NOTE cannot use non blocking calls because qt does not know how to handle the raw datatypes
                // std::cout << "try updating item: " << demangledTypeName(item) << " " << parameterType.c_str() << std::endl;
                const void *raw = item->getRawData();

                if (raw != nullptr)
                {
                    // std::cout << "updating item: " << demangledTypeName(item) << " " << parameterType.c_str() << " p:" << raw << std::endl;
                    it->method.invoke(vizPlugin, QGenericArgument(parameterType.c_str(), raw));
                }
                else
                {
                    LOG(WARNING) << "updating item visual failed: NULL item: " << demangledTypeName(*item) << " " << parameterType.c_str() << " p:" << raw << std::endl;
                }
            }
            else
            {
                LOG(WARNING) << "No visualizer found for item type " << parameterType;
            }
        }

        const QSet<QString> &EnvireGraphVisualizer::getFrameNames() const
        {
            return frameNames;
        }

        void EnvireGraphVisualizer::addFrameName(const QString &name)
        {
            if (frameNames.contains(name))
            {
                LOG(INFO) << "Ignoring frame name " << name.toStdString() << ". Has already been added";
                return;
            }
            frameNames.insert(name);
            emit frameAdded(name);
        }

        void EnvireGraphVisualizer::removeFrameName(const QString &name)
        {
            frameNames.remove(name);
            emit frameRemoved(name);
        }

        const TreeView &EnvireGraphVisualizer::getTree() const
        {
            return tree;
        }

        void EnvireGraphVisualizer::clearFrames()
        {
            while (frameNames.size() > 0)
            {
                QMetaObject::invokeMethod(this, "removeFrame", Qt::QueuedConnection,
                                          Q_ARG(QString, *frameNames.begin()));
                removeFrameName(*frameNames.begin());
            }
        }

        EnvireGraphVisualizer::~EnvireGraphVisualizer()
        {
            // unsubscribe manually because we might destroy the graph before
            // destroying base. In that case the automatic unsubscribe from base would crash.
            tree.unsubscribe();
            unsubscribe();
            itemVisuals.clear();
            graph.reset();
        }

    }
}