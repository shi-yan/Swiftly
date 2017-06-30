#include "SettingsManager.h"
#include <QDebug>

SettingsManager::SettingsManager()
    :m_settings()
{

}

void SettingsManager::init()
{
    qDebug() << m_settings.fileName();
}

QVariant SettingsManager::get(const QString &key)
{
    return m_settings.value(key);
}

bool SettingsManager::has(const QString &key)
{
    return m_settings.contains(key);
}

void SettingsManager::set(const QString &key, const QVariant &value)
{
    m_settings.setValue(key, value);
}
