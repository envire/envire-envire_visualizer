#pragma once

class QWidget;

namespace envire { namespace viz
{

  
class ItemManipulatorInterface 
{
public:
  virtual QWidget* show() = 0;
  virtual void hide() = 0;
};

}}