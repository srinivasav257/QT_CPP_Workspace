#pragma once

// Include QtADS headers - explicitly include the ADS library's factory
#include <DockComponentsFactory.h>
#include <DockAreaTitleBar.h>
#include <DockWidgetTab.h>
#include <AutoHideTab.h>
#include <DockAreaTabBar.h>

namespace ads {
    class CDockAreaWidget;
    class CDockWidget;
}

namespace DockManager {

/**
 * @brief Custom dock components factory for UI customization.
 *
 * This factory allows customization of dock UI components like
 * title bars, tabs, and buttons. Derive from this class and override
 * the create methods to add custom widgets.
 *
 * Usage:
 * @code
 * class MyFactory : public DockManager::CustomDockComponentsFactory {
 * public:
 *     ads::CDockAreaTitleBar* createDockAreaTitleBar(
 *         ads::CDockAreaWidget* area) const override
 *     {
 *         auto* titleBar = CustomDockComponentsFactory::createDockAreaTitleBar(area);
 *         // Add custom button
 *         auto* btn = new QToolButton(area);
 *         btn->setText("?");
 *         titleBar->insertWidget(0, btn);
 *         return titleBar;
 *     }
 * };
 *
 * // In main() before creating DockMainWindow:
 * ads::CDockComponentsFactory::setFactory(new MyFactory());
 * @endcode
 */
class CustomDockComponentsFactory : public ads::CDockComponentsFactory
{
public:
    CustomDockComponentsFactory() = default;
    ~CustomDockComponentsFactory() override = default;

    /**
     * @brief Create a dock area title bar
     *
     * Override this to customize the title bar (add buttons, change style, etc.)
     *
     * @param dockArea The dock area widget
     * @return The title bar widget
     */
    ads::CDockAreaTitleBar* createDockAreaTitleBar(
        ads::CDockAreaWidget* dockArea) const override;

    /**
     * @brief Create a dock widget tab
     *
     * Override this to customize tab appearance or behavior.
     *
     * @param dockWidget The dock widget
     * @return The tab widget
     */
    ads::CDockWidgetTab* createDockWidgetTab(
        ads::CDockWidget* dockWidget) const override;

    /**
     * @brief Create a dock area tab bar
     *
     * Override this to customize tab bar appearance or behavior.
     *
     * @param dockArea The dock area widget
     * @return The tab bar widget
     */
    ads::CDockAreaTabBar* createDockAreaTabBar(
        ads::CDockAreaWidget* dockArea) const override;

    /**
     * @brief Create an auto-hide side tab
     *
     * Override this to customize auto-hide tab appearance.
     *
     * @param dockWidget The dock widget for the tab
     * @return The auto-hide tab widget
     */
    ads::CAutoHideTab* createDockWidgetSideTab(
        ads::CDockWidget* dockWidget) const override;
};

} // namespace DockManager
