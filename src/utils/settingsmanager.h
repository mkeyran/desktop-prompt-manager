#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QDir>
#include <QDebug>

class SettingsManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString promptsPath READ promptsPath WRITE setPromptsPath NOTIFY promptsPathChanged)

public:
    explicit SettingsManager(QObject *parent = nullptr) : QObject(parent) {
        // Load settings
        m_promptsPath = m_settings.value("promptsPath", QDir::homePath() + "/Obsidian/PromptManager/").toString();
    }

    QString promptsPath() const {
        return m_promptsPath;
    }

    void setPromptsPath(const QString &path) {
        if (m_promptsPath != path) {
            m_promptsPath = path;
            m_settings.setValue("promptsPath", m_promptsPath);
            emit promptsPathChanged(m_promptsPath);
        }
    }

signals:
    void promptsPathChanged(const QString &newPath);

private:
    QSettings m_settings;
    QString m_promptsPath;
};

#endif // SETTINGSMANAGER_H
