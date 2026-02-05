#include "PanelRegistry.h"
#include <QSet>
#include <QDebug>

namespace DockManager {

PanelRegistry& PanelRegistry::instance()
{
    static PanelRegistry reg;
    return reg;
}

bool PanelRegistry::registerPanel(const PanelDefinition& def)
{
    if (m_idToIndex.contains(def.id)) {
        qWarning() << "PanelRegistry: duplicate panel ID ignored:" << def.id;
        return false;
    }

    if (def.id.isEmpty()) {
        qWarning() << "PanelRegistry: panel ID cannot be empty";
        return false;
    }

    if (!def.factory) {
        qWarning() << "PanelRegistry: panel" << def.id << "has no factory function";
        return false;
    }

    m_idToIndex.insert(def.id, m_panelList.size());
    m_panelList.append(def);
    return true;
}

bool PanelRegistry::registerPanel(PanelDefinition&& def)
{
    if (m_idToIndex.contains(def.id)) {
        qWarning() << "PanelRegistry: duplicate panel ID ignored:" << def.id;
        return false;
    }

    if (def.id.isEmpty()) {
        qWarning() << "PanelRegistry: panel ID cannot be empty";
        return false;
    }

    if (!def.factory) {
        qWarning() << "PanelRegistry: panel" << def.id << "has no factory function";
        return false;
    }

    m_idToIndex.insert(def.id, m_panelList.size());
    m_panelList.append(std::move(def));
    return true;
}

const PanelDefinition* PanelRegistry::panel(const QString& id) const
{
    auto it = m_idToIndex.constFind(id);
    if (it != m_idToIndex.constEnd())
        return &m_panelList.at(it.value());
    return nullptr;
}

const QList<PanelDefinition>& PanelRegistry::panels() const
{
    return m_panelList;
}

QStringList PanelRegistry::categories() const
{
    QSet<QString> cats;
    for (const auto& p : m_panelList)
        cats.insert(p.category);

    QStringList sorted(cats.begin(), cats.end());
    sorted.sort();
    return sorted;
}

QList<PanelDefinition> PanelRegistry::panelsInCategory(const QString& category) const
{
    QList<PanelDefinition> result;
    for (const auto& p : m_panelList) {
        if (p.category == category)
            result.append(p);
    }
    return result;
}

bool PanelRegistry::contains(const QString& id) const
{
    return m_idToIndex.contains(id);
}

int PanelRegistry::count() const
{
    return m_panelList.size();
}

void PanelRegistry::clear()
{
    m_panelList.clear();
    m_idToIndex.clear();
}

} // namespace DockManager
