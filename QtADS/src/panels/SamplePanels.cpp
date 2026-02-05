#include "SamplePanels.h"
#include <PanelRegistry.h>

#include <QLabel>
#include <QTextEdit>
#include <QTreeWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPlainTextEdit>
#include <QFont>
#include <QFontDatabase>

// ---------------------------------------------------------------------------
// Helper: creates a QTreeWidget with sample items to represent tree-based panels
// ---------------------------------------------------------------------------
static QWidget *makeTreePanel(QWidget *parent, const QString &title,
                              const QStringList &topItems,
                              const QStringList &childItems)
{
    auto *tree = new QTreeWidget(parent);
    tree->setHeaderLabel(title);
    for (const auto &top : topItems)
    {
        auto *item = new QTreeWidgetItem(tree, {top});
        for (const auto &child : childItems)
            new QTreeWidgetItem(item, {child});
        item->setExpanded(true);
    }
    return tree;
}

// ---------------------------------------------------------------------------
// Helper: creates a QTableWidget with sample columns
// ---------------------------------------------------------------------------
static QWidget *makeTablePanel(QWidget *parent, const QStringList &headers, int rows)
{
    auto *table = new QTableWidget(rows, headers.size(), parent);
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setStretchLastSection(true);
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < headers.size(); ++c)
            table->setItem(r, c, new QTableWidgetItem(QString("--")));
    }
    return table;
}

// ---------------------------------------------------------------------------
// Helper: creates a read-only QPlainTextEdit for output-style panels
// ---------------------------------------------------------------------------
static QWidget *makeOutputPanel(QWidget *parent, const QString &placeholder)
{
    auto *edit = new QPlainTextEdit(parent);
    edit->setReadOnly(true);
    edit->setPlaceholderText(placeholder);
    return edit;
}

// ---------------------------------------------------------------------------
// Helper: creates a QListWidget with sample items
// ---------------------------------------------------------------------------
static QWidget *makeListPanel(QWidget *parent, const QStringList &items)
{
    auto *list = new QListWidget(parent);
    list->addItems(items);
    return list;
}

// ---------------------------------------------------------------------------
// Registration
// ---------------------------------------------------------------------------
void registerSamplePanels()
{
    auto &reg = DockManager::PanelRegistry::instance();

    // ===== Explorer =====
    reg.registerPanel({"project_explorer", "Project Explorer", "Explorer",
                       ads::LeftDockWidgetArea,
                       [](QWidget *p)
                       {
                           return makeTreePanel(p, "Project",
                                                {"src", "include", "tests", "resources"},
                                                {"main.cpp", "widget.cpp", "utils.h"});
                       }});

    reg.registerPanel({"file_browser", "File Browser", "Explorer",
                       ads::LeftDockWidgetArea,
                       [](QWidget *p)
                       {
                           return makeTreePanel(p, "File System",
                                                {"C:/", "Documents", "Projects"},
                                                {"folder_a", "folder_b", "file.txt"});
                       }});

    reg.registerPanel({"class_view", "Class View", "Explorer",
                       ads::LeftDockWidgetArea,
                       [](QWidget *p)
                       {
                           return makeTreePanel(p, "Classes",
                                                {"MainWindow", "PanelRegistry", "DockManager"},
                                                {"method()", "signal()", "slot()"});
                       }});

    // ===== Editor =====
    reg.registerPanel({"code_editor", "Code Editor", "Editor",
                       ads::CenterDockWidgetArea,
                       [](QWidget *p)
                       {
                           auto *edit = new QTextEdit(p);
                           edit->setPlaceholderText("// Write your code here...\n#include <iostream>\n\nint main() {\n    return 0;\n}");
                           edit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
                           return static_cast<QWidget *>(edit);
                       }});

    reg.registerPanel({"text_editor", "Text Editor", "Editor",
                       ads::CenterDockWidgetArea,
                       [](QWidget *p)
                       {
                           auto *edit = new QTextEdit(p);
                           edit->setPlaceholderText("Plain text editor...");
                           return static_cast<QWidget *>(edit);
                       }});

    reg.registerPanel({"hex_editor", "Hex Editor", "Editor",
                       ads::CenterDockWidgetArea,
                       [](QWidget *p)
                       {
                           return makeTablePanel(p,
                                                 {"Offset", "00", "01", "02", "03", "04", "05", "06", "07",
                                                  "08", "09", "0A", "0B", "0C", "0D", "0E", "0F", "ASCII"},
                                                 16);
                       }});

    // ===== Output =====
    reg.registerPanel({"console_output", "Console", "Output",
                       ads::BottomDockWidgetArea,
                       [](QWidget *p)
                       { return makeOutputPanel(p, "Application console output..."); }});

    reg.registerPanel({"build_output", "Build Output", "Output",
                       ads::BottomDockWidgetArea,
                       [](QWidget *p)
                       { return makeOutputPanel(p, "Build output will appear here..."); }});

    reg.registerPanel({"debug_output", "Debug Output", "Output",
                       ads::BottomDockWidgetArea,
                       [](QWidget *p)
                       { return makeOutputPanel(p, "Debug messages..."); }});

    reg.registerPanel({"log_viewer", "Log Viewer", "Output",
                       ads::BottomDockWidgetArea,
                       [](QWidget *p)
                       { return makeOutputPanel(p, "Log entries..."); }});

    reg.registerPanel({"terminal", "Terminal", "Output",
                       ads::BottomDockWidgetArea,
                       [](QWidget *p)
                       {
                           auto *edit = new QPlainTextEdit(p);
                           edit->setPlaceholderText("$ ");
                           edit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
                           return static_cast<QWidget *>(edit);
                       }});

    // ===== Properties =====
    reg.registerPanel({"properties", "Properties", "Properties",
                       ads::RightDockWidgetArea,
                       [](QWidget *p)
                       {
                           return makeTablePanel(p, {"Property", "Value"}, 10);
                       }});

    reg.registerPanel({"inspector", "Inspector", "Properties",
                       ads::RightDockWidgetArea,
                       [](QWidget *p)
                       {
                           return makeTreePanel(p, "Object Inspector",
                                                {"QMainWindow", "CDockManager", "QMenuBar"},
                                                {"objectName", "geometry", "visible"});
                       }});

    reg.registerPanel({"settings_panel", "Settings", "Properties",
                       ads::RightDockWidgetArea,
                       [](QWidget *p)
                       {
                           return makeTablePanel(p, {"Setting", "Value", "Default"}, 8);
                       }});

    reg.registerPanel({"style_editor", "Style Editor", "Properties",
                       ads::RightDockWidgetArea,
                       [](QWidget *p)
                       {
                           auto *edit = new QTextEdit(p);
                           edit->setPlaceholderText("QWidget {\n    background: #2b2b2b;\n    color: #a9b7c6;\n}");
                           edit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
                           return static_cast<QWidget *>(edit);
                       }});

    // ===== Debug =====
    reg.registerPanel({"watch", "Watch", "Debug",
                       ads::RightDockWidgetArea,
                       [](QWidget *p)
                       {
                           return makeTablePanel(p, {"Expression", "Value", "Type"}, 5);
                       }});

    reg.registerPanel({"call_stack", "Call Stack", "Debug",
                       ads::BottomDockWidgetArea,
                       [](QWidget *p)
                       {
                           return makeTablePanel(p, {"#", "Function", "File", "Line"}, 8);
                       }});

    reg.registerPanel({"breakpoints", "Breakpoints", "Debug",
                       ads::BottomDockWidgetArea,
                       [](QWidget *p)
                       {
                           return makeTablePanel(p, {"Enabled", "File", "Line", "Condition"}, 5);
                       }});

    reg.registerPanel({"locals", "Locals", "Debug",
                       ads::RightDockWidgetArea,
                       [](QWidget *p)
                       {
                           return makeTreePanel(p, "Local Variables",
                                                {"this", "argc", "argv", "app"},
                                                {"value", "address", "type"});
                       }});

    reg.registerPanel({"threads", "Threads", "Debug",
                       ads::BottomDockWidgetArea,
                       [](QWidget *p)
                       {
                           return makeTablePanel(p, {"ID", "Name", "State", "Location"}, 4);
                       }});

    reg.registerPanel({"memory", "Memory", "Debug",
                       ads::BottomDockWidgetArea,
                       [](QWidget *p)
                       {
                           return makeTablePanel(p,
                                                 {"Address", "00", "01", "02", "03", "04", "05", "06", "07",
                                                  "08", "09", "0A", "0B", "0C", "0D", "0E", "0F"},
                                                 16);
                       }});

    reg.registerPanel({"registers", "Registers", "Debug",
                       ads::RightDockWidgetArea,
                       [](QWidget *p)
                       {
                           return makeTablePanel(p, {"Register", "Hex", "Decimal", "Binary"}, 16);
                       }});

    // ===== Tools =====
    reg.registerPanel({"search_results", "Search Results", "Tools",
                       ads::BottomDockWidgetArea,
                       [](QWidget *p)
                       {
                           return makeTreePanel(p, "Search Results",
                                                {"main.cpp (3 hits)", "widget.cpp (1 hit)"},
                                                {"Line 12: match", "Line 45: match"});
                       }});

    reg.registerPanel({"bookmarks", "Bookmarks", "Tools",
                       ads::BottomDockWidgetArea,
                       [](QWidget *p)
                       {
                           return makeListPanel(p, {"main.cpp:10 - Entry point",
                                                    "widget.cpp:55 - Event handler",
                                                    "utils.h:3 - Helper macro"});
                       }});

    reg.registerPanel({"todo_list", "TODO List", "Tools",
                       ads::BottomDockWidgetArea,
                       [](QWidget *p)
                       {
                           return makeTablePanel(p, {"File", "Line", "TODO Comment"}, 5);
                       }});
}
