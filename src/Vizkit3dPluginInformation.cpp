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

#include "Vizkit3dPluginInformation.hpp"
#include "Helpers.hpp"
#include <glog/logging.h>

namespace envire { namespace viz
{
Vizkit3dPluginInformation::Vizkit3dPluginInformation(vizkit3d::Vizkit3DWidget* mainWidget) :
    mainWidget(mainWidget)
{
  ASSERT_NOT_NULL(mainWidget);
  loadData();
}

void Vizkit3dPluginInformation::loadData()
{
  QStringList* availablePlugins = mainWidget->getAvailablePlugins();
  
  Qt::ConnectionType conType = Helpers::determineConnectionType(mainWidget);
  for(const QString& libName : *availablePlugins)
  {
    QObject* plugin = nullptr;
    QMetaObject::invokeMethod(mainWidget, "createPlugin", conType,
                              Q_RETURN_ARG(QObject*, plugin),
                              Q_ARG(QString, libName), Q_ARG(QString, ""));
    if(nullptr != plugin)
    {
      loadPluginData(plugin, libName);
      delete plugin;
    }
  }
  
  delete availablePlugins;
}

void Vizkit3dPluginInformation::loadPluginData(const QObject* plugin,
                                               const QString& libName)
{
  const QMetaObject* metaObj = plugin->metaObject();
  ASSERT_NOT_NULL(plugin);
  
  for(int i = 0; i < metaObj->methodCount(); ++i)
  {
    const QMetaMethod method = metaObj->method(i);
    QString signature = QString::fromAscii(method.signature());
    QStringList elements = signature.split("(");     
    const QString methodName = elements[0];
    if(methodName.contains("updateData"))
    {
      const QString typeName = method.typeName();
      QList<QByteArray> paramTypes = method.parameterTypes();
      if(paramTypes.size() == 1 && typeName.isEmpty()) //isEmpty means "void"
      { 
        typeToPlugin.insert(QString(paramTypes.at(0)), {method, libName});
        LOG(INFO) << "found method: " << method.signature() << 
                     " for type: " << QString(paramTypes[0]).toStdString();
      }
    }
  }
}

const QMultiHash<QString, Vizkit3dPluginInformation::UpdateMethod>&
Vizkit3dPluginInformation::getTypeToUpdateMethodMapping() const
{
  return typeToPlugin;
}
  
}}