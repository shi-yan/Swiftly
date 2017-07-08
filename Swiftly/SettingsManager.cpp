#include "SettingsManager.h"
#include <QDebug>
#include <QCoreApplication>
#include <QFileInfo>

SettingsManager::SettingsManager()
    :m_settings( QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName().replace(' ','_'))
{

}

void SettingsManager::init()
{
    qDebug() << m_settings.fileName();

    m_settings.setValue("UserManager/test", "gaga");
}

QVariant SettingsManager::get(const QString &key, const QVariant &defaultValue)
{
    return m_settings.value(key, defaultValue);
}

bool SettingsManager::has(const QString &key)
{
    return m_settings.contains(key);
}

void SettingsManager::set(const QString &key, const QVariant &value)
{
    m_settings.setValue(key, value);
}
