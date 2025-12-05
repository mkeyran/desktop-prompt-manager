#ifndef PROMPTREPOSITORY_H
#define PROMPTREPOSITORY_H

#include <QObject>
#include <QList>
#include "../models/prompt.h"
#include "../models/folder.h"
#include "../models/promptwithfolder.h"

class PromptRepository : public QObject
{
    Q_OBJECT

public:
    explicit PromptRepository(QObject *parent = nullptr);
    virtual ~PromptRepository();

    // Prompt operations
    virtual bool savePrompt(Prompt *prompt) = 0;
    virtual bool deletePrompt(int promptId) = 0;
    virtual Prompt* getPromptById(int promptId) = 0;
    virtual QList<Prompt*> getAllPrompts() = 0;
    virtual QList<Prompt*> getPromptsByFolder(int folderId) = 0;
    virtual QList<Prompt*> getPromptsWithoutFolder() = 0;
    virtual bool duplicatePrompt(int promptId) = 0;
    
    // Folder operations
    virtual bool saveFolder(Folder *folder) = 0;
    virtual bool deleteFolder(int folderId) = 0;
    virtual Folder* getFolderById(int folderId) = 0;
    virtual QList<Folder*> getAllFolders() = 0;
    virtual QList<Folder*> getFoldersWithCounts() = 0;
    virtual bool folderNameExists(const QString &name, int excludeId = -1) = 0;
    
    // Search operations
    virtual QList<Prompt*> searchPrompts(const QString &searchText) = 0;
    virtual QList<Prompt*> searchPromptsInFolder(const QString &searchText, int folderId) = 0;
    
    // Combined operations
    virtual QList<PromptWithFolder*> getPromptsWithFolders() = 0;
    
    // Statistics
    virtual int getPromptCount() = 0;
    virtual int getFolderCount() = 0;
    virtual int getPromptCountByFolder(int folderId) = 0;

signals:
    void promptAdded(Prompt *prompt);
    void promptUpdated(Prompt *prompt);
    void promptDeleted(int promptId);
    void folderAdded(Folder *folder);
    void folderUpdated(Folder *folder);
    void folderDeleted(int folderId);
    void dataChanged();
};

#endif // PROMPTREPOSITORY_H