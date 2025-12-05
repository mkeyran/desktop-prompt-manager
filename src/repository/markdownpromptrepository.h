#ifndef MARKDOWNPROMPTREPOSITORY_H
#define MARKDOWNPROMPTREPOSITORY_H

#include "promptrepository.h"
#include <QDir>
#include <QHash>
#include <QMap>

class MarkdownPromptRepository : public PromptRepository
{
    Q_OBJECT

public:
    explicit MarkdownPromptRepository(const QString &rootPath, QObject *parent = nullptr);
    ~MarkdownPromptRepository() override;
    
    void setRootPath(const QString &rootPath);

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
    void reload();
    void scanDirectory(const QDir &dir, int parentFolderId);
    Prompt* parsePromptFile(const QString &filePath, int id, int folderId);
    void writePromptFile(Prompt *prompt, const QString &filePath);
    QString extractFrontMatter(const QString &content, QMap<QString, QString> &frontMatter);
    QString generateFrontMatter(const QMap<QString, QString> &frontMatter);

    QString m_rootPath;
    
    // In-memory cache
    QList<Prompt*> m_prompts;
    QList<Folder*> m_folders;
    
    // ID management
    int m_nextPromptId = 1;
    int m_nextFolderId = 1;
    
    // Mappings to persist connections between IDs and Files across reloads (if possible)
    // For this version, IDs are ephemeral and reset on restart/reload
};

#endif // MARKDOWNPROMPTREPOSITORY_H
