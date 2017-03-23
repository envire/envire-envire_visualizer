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

#include <QObject> //included at the top to fix an issue with the boost::foreach macro shadowing qt foreach (see http://stackoverflow.com/questions/15191361/boostq-foreach-has-not-been-declared)
#include "PclItemFactory.hpp"
#include <envire_core/items/Item.hpp>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <pcl/io/pcd_io.h>
#include <envire_core/items/Item.hpp>
namespace envire { namespace viz 
{

PclItemFactory::PclItemFactory() : widget(new PclWidget())
{
  envire::core::Item<pcl::PCLPointCloud2> item;
  supportedTypes.emplace_back(*(item.getTypeInfo()));
}

QWidget* PclItemFactory::getConfigurationWidget(const std::type_index& type)
{
  return widget;
}

void PclItemFactory::addItem(const std::type_index& type, std::shared_ptr<envire::core::EnvireGraph> graph,
                             const envire::core::FrameId& frame, const QWidget* configWidget)
{
  if(type == supportedTypes[0])
  {
    envire::core::Item<pcl::PCLPointCloud2>::Ptr item(new envire::core::Item<pcl::PCLPointCloud2>());
    const QString pclFile = widget->getPclPath();
    ::pcl::PCDReader reader;
    if(QFile::exists(pclFile))
    {
      reader.read(pclFile.toStdString(), item->getData());
    }
    else
    {
      throw std::runtime_error("File does not exist: " + pclFile.toStdString());
    }
    graph->addItemToFrame(frame, item);
  }
}

const std::vector< std::type_index >& PclItemFactory::getSupportedTypes()
{
  return supportedTypes;
}

}}
