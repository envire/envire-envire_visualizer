#include "EnvireVisualizerPlugin.hpp"
#include <envire_visualizer/EnvireVisualizerWindow.hpp>
#include <QtPlugin>

EnvireVisualizerPlugin::EnvireVisualizerPlugin(QObject *parent) : QObject(parent)
{
    initialized = false;
}

EnvireVisualizerPlugin::~EnvireVisualizerPlugin()
{
}

bool EnvireVisualizerPlugin::isContainer() const
{
    return false;
}

bool EnvireVisualizerPlugin::isInitialized() const
{
    return initialized;
}

QIcon EnvireVisualizerPlugin::icon() const
{
    return QIcon("");
}

QString EnvireVisualizerPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
            " <widget class=\"EnvireVisualizerWindow\" name=\"EnvireVisualizer\">\n"
            "  <property name=\"geometry\">\n"
            "   <rect>\n"
            "    <x>0</x>\n"
            "    <y>0</y>\n"
            "     <width>640</width>\n"
            "     <height>480</height>\n"
            "   </rect>\n"
            "  </property>\n"
            "  <property name=\"toolTip\" >\n"
            "   <string>EnvireVisualizer</string>\n"
            "  </property>\n"
            "  <property name=\"whatsThis\" >\n"
            "   <string>EnvireVisualizer</string>\n"
            "  </property>\n"
            " </widget>\n"
            "</ui>\n";
}

QString EnvireVisualizerPlugin::group() const {
    return "EnviRe";
}

QString EnvireVisualizerPlugin::includeFile() const {
    return "envire_visualizer/EnvireVisualizerWindow.hpp";
}

QString EnvireVisualizerPlugin::name() const {
    return "EnvireVisualizerWindow";
}

QString EnvireVisualizerPlugin::toolTip() const {
    return whatsThis();
}

QString EnvireVisualizerPlugin::whatsThis() const
{
    return "";
}

QWidget* EnvireVisualizerPlugin::createWidget(QWidget *parent)
{
    return new envire::viz::EnvireVisualizerWindow(parent, Qt::Widget);
}

void EnvireVisualizerPlugin::initialize(QDesignerFormEditorInterface *core)
{
     if (initialized)
         return;
     initialized = true;
}

Q_EXPORT_PLUGIN2(EnvireVisualizer, EnvireVisualizerPlugin)