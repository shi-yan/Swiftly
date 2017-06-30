#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QVariant>

class SettingsManager
{
private:
    QSettings m_settings;
    SettingsManager();

public:
    static SettingsManager & getSingleton()
    {
        static SettingsManager obj;
        return obj;
    }

    void init();
    bool has(const QString &key);
    QVariant get(const QString &key);
    void set(const QString &key, const QVariant &value);
};

#endif // SETTINGSMANAGER_H
