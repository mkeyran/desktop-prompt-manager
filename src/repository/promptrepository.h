#ifndef PROMPTREPOSITORY_H
#define PROMPTREPOSITORY_H

#include <QObject>
#include <QList>
#include "../models/prompt.h"
#include "../models/folder.h"
#include "../models/promptwithfolder.h"

class Database;
class PromptDao;
class FolderDao;

class PromptRepository : public QObject
{
    Q_OBJECT

public:
    explicit PromptRepository(Database *database, QObject *parent = nullptr);
    ~PromptRepository();

    // Prompt operations
    bool savePrompt(Prompt *prompt);
    bool deletePrompt(int promptId);
    Prompt* getPromptById(int promptId);
    QList<Prompt*> getAllPrompts();
    QList<Prompt*> getPromptsByFolder(int folderId);
    QList<Prompt*> getPromptsWithoutFolder();
    bool duplicatePrompt(int promptId);
    
    // Folder operations
    bool saveFolder(Folder *folder);
    bool deleteFolder(int folderId);
    Folder* getFolderById(int folderId);
    QList<Folder*> getAllFolders();
    QList<Folder*> getFoldersWithCounts();
    bool folderNameExists(const QString &name, int excludeId = -1);
    
    // Search operations
    QList<Prompt*> searchPrompts(const QString &searchText);
    QList<Prompt*> searchPromptsInFolder(const QString &searchText, int folderId);
    
    // Combined operations
    QList<PromptWithFolder*> getPromptsWithFolders();
    
    // Statistics
    int getPromptCount();
    int getFolderCount();
    int getPromptCountByFolder(int folderId);

signals:
    void promptAdded(Prompt *prompt);
    void promptUpdated(Prompt *prompt);
    void promptDeleted(int promptId);
    void folderAdded(Folder *folder);
    void folderUpdated(Folder *folder);
    void folderDeleted(int folderId);
    void dataChanged();

private:
    Database *m_database;
    PromptDao *m_promptDao;
    FolderDao *m_folderDao;
};

#endif // PROMPTREPOSITORY_H