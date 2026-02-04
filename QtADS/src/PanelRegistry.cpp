#include "PanelRegistry.h"
#include <QSet>
#include <QDebug>

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
    m_idToIndex.insert(def.id, m_panelList.size());
    m_panelList.append(def);
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
