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
#include <vizkit3d/Vizkit3DWidget.hpp>
#include <QMultiHash>

namespace envire { namespace viz
{

/**Contains information about updateData methods of all available Vizkit3D plugins*/
class Vizkit3dPluginInformation
{
public:
  
  /**Contains everything needed to invoke a certain update method */
  struct UpdateMethod
  {
    QMetaMethod method; //meta infos about the method
    QString libName; //name of the library (give to loadPlugin() to get an instance)
  };
  
  Vizkit3dPluginInformation(vizkit3d::Vizkit3DWidget* mainWidget);
  
  using TypeToUpdateMapping = QMultiHash<QString, UpdateMethod>;
  const TypeToUpdateMapping& getTypeToUpdateMethodMapping() const;
  
private:
  
  /**Populate ::typeToPlugin by loading all plugins and parsing their meta data */
  void loadData();
  
  /** Add meta data of a specific plugin to ::typeToPlugin
   * @param libName Name of the library that @p plugin comes from*/
  void loadPluginData(const QObject* plugin, const QString& libName);
  
  /**Widget that is used to load the plugins*/
  vizkit3d::Vizkit3DWidget* mainWidget;
  /**Contains a mapping from parameter type name to update method.
   * @note there might be more than one updateMethod per type.
   *       */
  TypeToUpdateMapping typeToPlugin;
};

}}