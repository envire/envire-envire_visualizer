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
#include <envire_core/graph/EnvireGraph.hpp>
#include <vector>
#include <type_traits>

class QWidget;

namespace envire { namespace viz
{

class ItemFactoryInterface 
{
public:
  
  
  /**Returns a widget used for configurating the item. The widget will be passed
   * to the createItem() method later on*/
  virtual QWidget* getConfigurationWidget(const std::type_index& type) = 0;
  
  /**Create a new item of @p type and add it to @p frame in @p graph.
   * This method is called in the gui thread.
   * When this method is called a QApplication exists.
   * */
  virtual void addItem(const std::type_index& type, std::shared_ptr<envire::core::EnvireGraph> graph,
                       const envire::core::FrameId& frame, const QWidget* configWidget) = 0;
  
  /**Returns a vector of all types that this factory can create.
   * @note This should be the item types, not the types of the embedded data.
   *       E.g. if you want to support envire::pcl::Pointcloud this list should
   *       contain the result of envire::pcl::PointCloud::getTypeInfo() */
  virtual const std::vector<std::type_index>& getSupportedTypes() = 0;
  
  virtual ~ItemFactoryInterface(){};
};

}}