#include "mainwindow.h"

#include <QAbstractButton>
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

namespace
{
class VsWatermark final : public QWidget
{
public:
    explicit VsWatermark(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setFixedSize(320, 280);
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor("#e3e3e8"));

        QPainterPath rightRibbon;
        rightRibbon.moveTo(195, 24);
        rightRibbon.lineTo(281, 68);
        rightRibbon.lineTo(281, 232);
        rightRibbon.lineTo(195, 276);
        rightRibbon.lineTo(99, 184);
        rightRibbon.lineTo(151, 150);
        rightRibbon.lineTo(151, 124);
        rightRibbon.lineTo(99, 90);
        rightRibbon.closeSubpath();
        painter.drawPath(rightRibbon);

        painter.save();
        painter.translate(128, 130);
        painter.rotate(40.0);
        painter.drawRoundedRect(QRectF(-95, -16, 140, 32), 13, 13);
        painter.restore();

        painter.save();
        painter.translate(128, 170);
        painter.rotate(-40.0);
        painter.drawRoundedRect(QRectF(-95, -16, 140, 32), 13, 13);
        painter.restore();
    }
};

QToolButton *makeMenuButton(const QString &text)
{
    auto *button = new QToolButton();
    button->setText(text);
    button->setObjectName("menuButton");
    button->setCursor(Qt::PointingHandCursor);
    button->setAutoRaise(true);
    return button;
}

QToolButton *makeWindowButton(const QString &text, const QString &name)
{
    auto *button = new QToolButton();
    button->setText(text);
    button->setObjectName(name);
    button->setCursor(Qt::PointingHandCursor);
    button->setFixedSize(40, 28);
    return button;
}

QToolButton *makeActivityButton(const QString &text, bool active = false)
{
    auto *button = new QToolButton();
    button->setText(text);
    button->setObjectName("activityButton");
    button->setProperty("active", active);
    button->setCursor(Qt::PointingHandCursor);
    button->setFixedSize(38, 38);
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    return button;
}

QLabel *makeKeyCap(const QString &text)
{
    auto *label = new QLabel(text);
    label->setObjectName("keyCap");
    label->setAlignment(Qt::AlignCenter);
    return label;
}

void appendKeySequence(QHBoxLayout *layout, const QStringList &keys)
{
    for (int i = 0; i < keys.size(); ++i)
    {
        if (i > 0)
        {
            auto *plus = new QLabel("+");
            plus->setObjectName("plusLabel");
            layout->addWidget(plus);
        }

        layout->addWidget(makeKeyCap(keys.at(i)));
    }
}

QWidget *makeShortcutRow(const QString &title, const QStringList &first, const QStringList &second = {})
{
    auto *row = new QWidget();
    auto *rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(0, 0, 0, 0);
    rowLayout->setSpacing(8);

    auto *label = new QLabel(title);
    label->setObjectName("shortcutLabel");
    label->setMinimumWidth(165);
    rowLayout->addWidget(label);

    auto *keysContainer = new QWidget();
    auto *keysLayout = new QHBoxLayout(keysContainer);
    keysLayout->setContentsMargins(0, 0, 0, 0);
    keysLayout->setSpacing(6);

    appendKeySequence(keysLayout, first);
    if (!second.isEmpty())
    {
        keysLayout->addSpacing(12);
        appendKeySequence(keysLayout, second);
    }

    rowLayout->addWidget(keysContainer);
    rowLayout->addStretch();

    return row;
}
} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setMinimumSize(1180, 760);

    auto *root = new QWidget();
    root->setObjectName("root");
    setCentralWidget(root);

    auto *rootLayout = new QVBoxLayout(root);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    auto *topBar = new QWidget();
    topBar->setObjectName("topBar");
    topBar->setFixedHeight(40);
    m_dragRegion = topBar;
    m_dragRegion->installEventFilter(this);

    auto *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(10, 4, 8, 4);
    topLayout->setSpacing(10);

    auto *appIcon = new QLabel("<>");
    appIcon->setObjectName("appIcon");
    appIcon->setAlignment(Qt::AlignCenter);
    appIcon->setFixedSize(20, 20);
    topLayout->addWidget(appIcon);

    const QStringList menuItems = {"File", "Edit", "Selection", "View", "Go", "..."};
    for (const QString &menu : menuItems)
        topLayout->addWidget(makeMenuButton(menu));

    auto *backButton = makeMenuButton("<");
    backButton->setObjectName("navButton");
    backButton->setFixedWidth(24);
    topLayout->addWidget(backButton);

    auto *forwardButton = makeMenuButton(">");
    forwardButton->setObjectName("navButton");
    forwardButton->setFixedWidth(24);
    topLayout->addWidget(forwardButton);

    topLayout->addSpacing(8);

    auto *searchBox = new QLineEdit();
    searchBox->setObjectName("searchBox");
    searchBox->setPlaceholderText("Search");
    searchBox->setFixedHeight(28);
    searchBox->setFixedWidth(390);
    topLayout->addWidget(searchBox);

    topLayout->addStretch();

    auto *layoutButton = makeMenuButton("[]");
    layoutButton->setObjectName("viewModeButton");
    layoutButton->setFixedWidth(28);
    topLayout->addWidget(layoutButton);

    auto *splitButton = makeMenuButton("[]");
    splitButton->setObjectName("viewModeButton");
    splitButton->setFixedWidth(28);
    topLayout->addWidget(splitButton);

    auto *minButton = makeWindowButton("-", "windowButton");
    connect(minButton, &QToolButton::clicked, this, &MainWindow::showMinimized);
    topLayout->addWidget(minButton);

    m_maximizeButton = makeWindowButton("[]", "windowButton");
    connect(m_maximizeButton, &QToolButton::clicked, this, &MainWindow::toggleMaximizeRestore);
    topLayout->addWidget(m_maximizeButton);

    auto *closeButton = makeWindowButton("x", "closeButton");
    connect(closeButton, &QToolButton::clicked, this, &MainWindow::close);
    topLayout->addWidget(closeButton);

    rootLayout->addWidget(topBar);

    auto *content = new QWidget();
    auto *contentLayout = new QHBoxLayout(content);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    auto *activityBar = new QWidget();
    activityBar->setObjectName("activityBar");
    activityBar->setFixedWidth(52);

    auto *activityLayout = new QVBoxLayout(activityBar);
    activityLayout->setContentsMargins(6, 8, 6, 8);
    activityLayout->setSpacing(6);

    activityLayout->addWidget(makeActivityButton(QStringLiteral("\uE8A5"), true), 0, Qt::AlignHCenter);
    activityLayout->addWidget(makeActivityButton(QStringLiteral("\uE721")), 0, Qt::AlignHCenter);
    activityLayout->addWidget(makeActivityButton(QStringLiteral("\uE9D2")), 0, Qt::AlignHCenter);
    activityLayout->addWidget(makeActivityButton(QStringLiteral("\uE768")), 0, Qt::AlignHCenter);
    activityLayout->addWidget(makeActivityButton(QStringLiteral("\uE943")), 0, Qt::AlignHCenter);
    activityLayout->addWidget(makeActivityButton(QStringLiteral("\uE7AD")), 0, Qt::AlignHCenter);
    activityLayout->addWidget(makeActivityButton(QStringLiteral("\uEA86")), 0, Qt::AlignHCenter);
    activityLayout->addStretch();
    activityLayout->addWidget(makeActivityButton(QStringLiteral("\uE77B")), 0, Qt::AlignHCenter);
    activityLayout->addWidget(makeActivityButton(QStringLiteral("\uE713")), 0, Qt::AlignHCenter);

    contentLayout->addWidget(activityBar);

    auto *editorArea = new QWidget();
    editorArea->setObjectName("editorArea");

    auto *editorLayout = new QVBoxLayout(editorArea);
    editorLayout->setContentsMargins(0, 0, 0, 0);
    editorLayout->setSpacing(0);

    auto *welcome = new QWidget();
    auto *welcomeLayout = new QVBoxLayout(welcome);
    welcomeLayout->setContentsMargins(0, 0, 0, 40);
    welcomeLayout->setSpacing(12);
    welcomeLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    welcomeLayout->addWidget(new VsWatermark(), 0, Qt::AlignHCenter);

    auto *shortcuts = new QWidget();
    auto *shortcutsLayout = new QVBoxLayout(shortcuts);
    shortcutsLayout->setContentsMargins(0, 0, 0, 0);
    shortcutsLayout->setSpacing(10);

    shortcutsLayout->addWidget(makeShortcutRow("Open Chat", {"Ctrl", "Alt", "I"}));
    shortcutsLayout->addWidget(makeShortcutRow("Show All Commands", {"Ctrl", "Shift", "P"}));
    shortcutsLayout->addWidget(makeShortcutRow("Open File", {"Ctrl", "O"}));
    shortcutsLayout->addWidget(makeShortcutRow("Open Folder", {"Ctrl", "K"}, {"Ctrl", "O"}));
    shortcutsLayout->addWidget(makeShortcutRow("Open Recent", {"Ctrl", "R"}));

    welcomeLayout->addWidget(shortcuts, 0, Qt::AlignHCenter);

    editorLayout->addStretch();
    editorLayout->addWidget(welcome, 0, Qt::AlignHCenter);
    editorLayout->addStretch();

    contentLayout->addWidget(editorArea);

    rootLayout->addWidget(content, 1);

    auto *statusBarWidget = new QWidget();
    statusBarWidget->setObjectName("statusBarWidget");
    statusBarWidget->setFixedHeight(26);

    auto *statusLayout = new QHBoxLayout(statusBarWidget);
    statusLayout->setContentsMargins(8, 0, 10, 0);
    statusLayout->setSpacing(14);

    auto *leftStatus = new QLabel("<>   x 0   ! 0");
    leftStatus->setObjectName("statusText");
    statusLayout->addWidget(leftStatus);

    statusLayout->addStretch();

    auto *rightStatus = new QLabel("Autocomplete   |   Spaces: 4   |   UTF-8");
    rightStatus->setObjectName("statusText");
    statusLayout->addWidget(rightStatus);

    rootLayout->addWidget(statusBarWidget);

    setStyleSheet(R"(
        QMainWindow {
            background: #d9d9df;
        }

        #root {
            background: #f3f3f7;
            border: 1px solid #c6c6cf;
        }

        #topBar {
            background: #d8cae8;
            border-bottom: 1px solid #b9abc8;
        }

        #appIcon {
            background: #0178d4;
            color: #ffffff;
            border-radius: 4px;
            font: 700 7.5pt "Segoe UI";
        }

        QToolButton#menuButton {
            border: none;
            background: transparent;
            color: #3f3f49;
            font: 500 10pt "Segoe UI";
            padding: 4px 6px;
        }

        QToolButton#menuButton:hover {
            background: #c6b7d7;
            border-radius: 4px;
        }

        QToolButton#viewModeButton {
            border: none;
            background: transparent;
            color: #4d425c;
            font: 600 9pt "Consolas";
            padding: 4px 3px;
            min-width: 22px;
        }

        QToolButton#viewModeButton:hover {
            background: #c6b7d7;
            border-radius: 4px;
        }

        QToolButton#navButton {
            border: none;
            background: transparent;
            color: #5f516e;
            font: 700 9pt "Segoe UI";
            padding: 4px 0;
            min-width: 20px;
        }

        QToolButton#navButton:hover {
            background: #c6b7d7;
            border-radius: 4px;
        }

        #searchBox {
            background: #cdbedf;
            border: 1px solid #b4a5c7;
            border-radius: 6px;
            padding: 0 10px;
            color: #3d3349;
            font: 10pt "Segoe UI";
        }

        #searchBox::placeholder {
            color: #6e5f82;
        }

        QToolButton#windowButton {
            border: none;
            background: transparent;
            color: #3f3f49;
            font: 700 10pt "Consolas";
        }

        QToolButton#windowButton:hover {
            background: #c5b6d5;
        }

        QToolButton#closeButton {
            border: none;
            background: transparent;
            color: #3f3f49;
            font: 700 10pt "Consolas";
        }

        QToolButton#closeButton:hover {
            background: #d95e5e;
            color: white;
        }

        #activityBar {
            background: #ececf3;
            border-right: 1px solid #dddde7;
        }

        QToolButton#activityButton {
            border: none;
            background: transparent;
            color: #8a8da1;
            font: 11pt "Segoe MDL2 Assets";
            border-radius: 8px;
        }

        QToolButton#activityButton:hover {
            background: #ddddea;
            color: #5d5f73;
        }

        QToolButton#activityButton[active="true"] {
            background: #d8d7e7;
            color: #6151a5;
            border-left: 3px solid #6151a5;
            padding-left: 5px;
        }

        #editorArea {
            background: #f4f4f8;
        }

        QLabel#shortcutLabel {
            color: #5a5f69;
            font: 10.5pt "Segoe UI";
        }

        QLabel#keyCap {
            background: #eceff3;
            border: 1px solid #d4d8de;
            border-radius: 4px;
            color: #677180;
            font: 600 8.5pt "Segoe UI";
            padding: 1px 8px;
            min-width: 14px;
        }

        QLabel#plusLabel {
            color: #818694;
            font: 600 8.5pt "Segoe UI";
        }

        #statusBarWidget {
            background: #6b4ca4;
            border-top: 1px solid #5b3f8f;
        }

        QLabel#statusText {
            color: #ffffff;
            font: 600 8.5pt "Segoe UI";
        }
    )");
}

MainWindow::~MainWindow() {}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_dragRegion)
    {
        switch (event->type())
        {
        case QEvent::MouseButtonPress:
        {
            auto *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                QWidget *child = m_dragRegion->childAt(mouseEvent->position().toPoint());
                if (!qobject_cast<QAbstractButton *>(child) && !qobject_cast<QLineEdit *>(child))
                {
                    m_dragActive = true;
                    m_dragOffset = mouseEvent->globalPosition().toPoint() - frameGeometry().topLeft();
                    return true;
                }
            }
            break;
        }
        case QEvent::MouseMove:
        {
            auto *mouseEvent = static_cast<QMouseEvent *>(event);
            if (m_dragActive && !(windowState() & Qt::WindowMaximized))
            {
                move(mouseEvent->globalPosition().toPoint() - m_dragOffset);
                return true;
            }
            break;
        }
        case QEvent::MouseButtonRelease:
            m_dragActive = false;
            break;
        case QEvent::MouseButtonDblClick:
        {
            auto *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                toggleMaximizeRestore();
                return true;
            }
            break;
        }
        default:
            break;
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::toggleMaximizeRestore()
{
    if (windowState() & Qt::WindowMaximized)
    {
        showNormal();
        if (m_maximizeButton)
            m_maximizeButton->setText("[]");
    }
    else
    {
        showMaximized();
        if (m_maximizeButton)
            m_maximizeButton->setText("o");
    }
}
