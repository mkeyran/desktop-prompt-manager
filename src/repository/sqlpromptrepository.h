#ifndef SQLPROMPTREPOSITORY_H
#define SQLPROMPTREPOSITORY_H

#include "promptrepository.h"

class Database;
class PromptDao;
class FolderDao;

class SqlPromptRepository : public PromptRepository
{
    Q_OBJECT

public:
    explicit SqlPromptRepository(Database *database, QObject *parent = nullptr);
    ~SqlPromptRepository() override;

    // Prompt operations
    bool savePrompt(Prompt *prompt) override;
    bool deletePrompt(int promptId) override;
    Prompt* getPromptById(int promptId) override;
    QList<Prompt*> getAllPrompts() override;
    QList<Prompt*> getPromptsByFolder(int folderId) override;
    QList<Prompt*> getPromptsWithoutFolder() override;
    bool duplicatePrompt(int promptId) override;
    
    // Folder operations
    bool saveFolder(Folder *folder) override;
    bool deleteFolder(int folderId) override;
    Folder* getFolderById(int folderId) override;
    QList<Folder*> getAllFolders() override;
    QList<Folder*> getFoldersWithCounts() override;
    bool folderNameExists(const QString &name, int excludeId = -1) override;
    
    // Search operations
    QList<Prompt*> searchPrompts(const QString &searchText) override;
    QList<Prompt*> searchPromptsInFolder(const QString &searchText, int folderId) override;
    
    // Combined operations
    QList<PromptWithFolder*> getPromptsWithFolders() override;
    
    // Statistics
    int getPromptCount() override;
    int getFolderCount() override;
    int getPromptCountByFolder(int folderId) override;

private:
    Database *m_database;
    PromptDao *m_promptDao;
    FolderDao *m_folderDao;
};

#endif // SQLPROMPTREPOSITORY_H
