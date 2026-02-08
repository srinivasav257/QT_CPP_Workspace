# IDE Shell Library

Reusable VS-like Qt Widgets shell (frameless top bar, left activity bar, center host area, bottom status bar).

## What to copy

Copy this folder to any Qt6 Widgets project:

- `lib/ide_shell/`

## CMake usage

```cmake
add_subdirectory(lib/ide_shell)

target_link_libraries(YourAppTarget PRIVATE IdeShell::IdeShell)
```

## Basic integration

```cpp
#include <IdeShell/IdeShellWindow.h>

class MainWindow : public ide_shell::IdeShellWindow
{
public:
    MainWindow()
    {
        // Replace default welcome panel with your own workspace widget
        auto *customCenter = new QWidget();
        setWorkspaceWidget(customCenter);
    }
};
```

## Useful API

- `QWidget* workspaceHost() const`
- `void setWorkspaceWidget(QWidget *widget)`
- `void setStatusText(const QString &leftText, const QString &rightText)`
- `QWidget* createWelcomePanel() const` (protected)
