#ifndef PROMPTWITHFOLDER_H
#define PROMPTWITHFOLDER_H

#include <QObject>
#include "prompt.h"
#include "folder.h"

class PromptWithFolder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Prompt* prompt READ prompt NOTIFY promptChanged)
    Q_PROPERTY(Folder* folder READ folder NOTIFY folderChanged)
    Q_PROPERTY(QString folderName READ folderName NOTIFY folderNameChanged)

public:
    explicit PromptWithFolder(QObject *parent = nullptr);
    PromptWithFolder(Prompt *prompt, Folder *folder = nullptr, QObject *parent = nullptr);

    // Getters
    Prompt* prompt() const { return m_prompt; }
    Folder* folder() const { return m_folder; }
    QString folderName() const;

    // Setters
    void setPrompt(Prompt *prompt);
    void setFolder(Folder *folder);

    // Helper methods
    bool hasFolder() const { return m_folder != nullptr; }

signals:
    void promptChanged();
    void folderChanged();
    void folderNameChanged();

private:
    Prompt *m_prompt;
    Folder *m_folder;
};

#endif // PROMPTWITHFOLDER_H