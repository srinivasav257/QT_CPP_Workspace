#include "CustomDockComponentsFactory.h"
#include <DockAreaWidget.h>
#include <DockWidget.h>

namespace DockManager {

ads::CDockAreaTitleBar* CustomDockComponentsFactory::createDockAreaTitleBar(
    ads::CDockAreaWidget* dockArea) const
{
    // Use default implementation - override in derived class to customize
    return new ads::CDockAreaTitleBar(dockArea);
}

ads::CDockWidgetTab* CustomDockComponentsFactory::createDockWidgetTab(
    ads::CDockWidget* dockWidget) const
{
    // Use default implementation - override in derived class to customize
    return new ads::CDockWidgetTab(dockWidget);
}

ads::CDockAreaTabBar* CustomDockComponentsFactory::createDockAreaTabBar(
    ads::CDockAreaWidget* dockArea) const
{
    // Use default implementation - override in derived class to customize
    return new ads::CDockAreaTabBar(dockArea);
}

ads::CAutoHideTab* CustomDockComponentsFactory::createDockWidgetSideTab(
    ads::CDockWidget* dockWidget) const
{
    // Use default implementation - override in derived class to customize
    return new ads::CAutoHideTab(dockWidget);
}

} // namespace DockManager
