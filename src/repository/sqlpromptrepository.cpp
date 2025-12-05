#include "sqlpromptrepository.h"
#include "../database/database.h"
#include "../database/promptdao.h"
#include "../database/folderdao.h"

SqlPromptRepository::SqlPromptRepository(Database *database, QObject *parent)
    : PromptRepository(parent), m_database(database)
{
    m_promptDao = new PromptDao(database, this);
    m_folderDao = new FolderDao(database, this);
}

SqlPromptRepository::~SqlPromptRepository()
{
}

// Prompt operations
bool SqlPromptRepository::savePrompt(Prompt *prompt)
{
    if (!prompt) {
        return false;
    }
    
    bool success;
    bool isNew = !prompt->isValid();
    
    if (isNew) {
        success = m_promptDao->insertPrompt(prompt);
        if (success) {
            emit promptAdded(prompt);
        }
    } else {
        success = m_promptDao->updatePrompt(prompt);
        if (success) {
            emit promptUpdated(prompt);
        }
    }
    
    if (success) {
        emit dataChanged();
    }
    
    return success;
}

bool SqlPromptRepository::deletePrompt(int promptId)
{
    bool success = m_promptDao->deletePrompt(promptId);
    if (success) {
        emit promptDeleted(promptId);
        emit dataChanged();
    }
    return success;
}

Prompt* SqlPromptRepository::getPromptById(int promptId)
{
    return m_promptDao->getPromptById(promptId);
}

QList<Prompt*> SqlPromptRepository::getAllPrompts()
{
    return m_promptDao->getAllPrompts();
}

QList<Prompt*> SqlPromptRepository::getPromptsByFolder(int folderId)
{
    return m_promptDao->getPromptsByFolder(folderId);
}

QList<Prompt*> SqlPromptRepository::getPromptsWithoutFolder()
{
    return m_promptDao->getPromptsWithoutFolder();
}

bool SqlPromptRepository::duplicatePrompt(int promptId)
{
    bool success = m_promptDao->duplicatePrompt(promptId);
    if (success) {
        emit dataChanged();
    }
    return success;
}

// Folder operations
bool SqlPromptRepository::saveFolder(Folder *folder)
{
    if (!folder) {
        return false;
    }
    
    bool success;
    bool isNew = !folder->isValid();
    
    if (isNew) {
        success = m_folderDao->insertFolder(folder);
        if (success) {
            emit folderAdded(folder);
        }
    } else {
        success = m_folderDao->updateFolder(folder);
        if (success) {
            emit folderUpdated(folder);
        }
    }
    
    if (success) {
        emit dataChanged();
    }
    
    return success;
}

bool SqlPromptRepository::deleteFolder(int folderId)
{
    bool success = m_folderDao->deleteFolder(folderId);
    if (success) {
        emit folderDeleted(folderId);
        emit dataChanged();
    }
    return success;
}

Folder* SqlPromptRepository::getFolderById(int folderId)
{
    return m_folderDao->getFolderById(folderId);
}

QList<Folder*> SqlPromptRepository::getAllFolders()
{
    return m_folderDao->getAllFolders();
}

QList<Folder*> SqlPromptRepository::getFoldersWithCounts()
{
    return m_folderDao->getFoldersWithCounts();
}

bool SqlPromptRepository::folderNameExists(const QString &name, int excludeId)
{
    return m_folderDao->folderNameExists(name, excludeId);
}

// Search operations
QList<Prompt*> SqlPromptRepository::searchPrompts(const QString &searchText)
{
    return m_promptDao->searchPrompts(searchText);
}

QList<Prompt*> SqlPromptRepository::searchPromptsInFolder(const QString &searchText, int folderId)
{
    return m_promptDao->searchPromptsInFolder(searchText, folderId);
}

// Combined operations
QList<PromptWithFolder*> SqlPromptRepository::getPromptsWithFolders()
{
    return m_promptDao->getPromptsWithFolders();
}

// Statistics
int SqlPromptRepository::getPromptCount()
{
    return m_promptDao->getPromptCount();
}

int SqlPromptRepository::getFolderCount()
{
    return m_folderDao->getFolderCount();
}

int SqlPromptRepository::getPromptCountByFolder(int folderId)
{
    return m_promptDao->getPromptCountByFolder(folderId);
}
