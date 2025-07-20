#include "promptrepository.h"
#include "../database/database.h"
#include "../database/promptdao.h"
#include "../database/folderdao.h"

PromptRepository::PromptRepository(Database *database, QObject *parent)
    : QObject(parent), m_database(database)
{
    m_promptDao = new PromptDao(database, this);
    m_folderDao = new FolderDao(database, this);
}

PromptRepository::~PromptRepository()
{
}

// Prompt operations
bool PromptRepository::savePrompt(Prompt *prompt)
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

bool PromptRepository::deletePrompt(int promptId)
{
    bool success = m_promptDao->deletePrompt(promptId);
    if (success) {
        emit promptDeleted(promptId);
        emit dataChanged();
    }
    return success;
}

Prompt* PromptRepository::getPromptById(int promptId)
{
    return m_promptDao->getPromptById(promptId);
}

QList<Prompt*> PromptRepository::getAllPrompts()
{
    return m_promptDao->getAllPrompts();
}

QList<Prompt*> PromptRepository::getPromptsByFolder(int folderId)
{
    return m_promptDao->getPromptsByFolder(folderId);
}

QList<Prompt*> PromptRepository::getPromptsWithoutFolder()
{
    return m_promptDao->getPromptsWithoutFolder();
}

bool PromptRepository::duplicatePrompt(int promptId)
{
    bool success = m_promptDao->duplicatePrompt(promptId);
    if (success) {
        emit dataChanged();
    }
    return success;
}

// Folder operations
bool PromptRepository::saveFolder(Folder *folder)
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

bool PromptRepository::deleteFolder(int folderId)
{
    bool success = m_folderDao->deleteFolder(folderId);
    if (success) {
        emit folderDeleted(folderId);
        emit dataChanged();
    }
    return success;
}

Folder* PromptRepository::getFolderById(int folderId)
{
    return m_folderDao->getFolderById(folderId);
}

QList<Folder*> PromptRepository::getAllFolders()
{
    return m_folderDao->getAllFolders();
}

QList<Folder*> PromptRepository::getFoldersWithCounts()
{
    return m_folderDao->getFoldersWithCounts();
}

bool PromptRepository::folderNameExists(const QString &name, int excludeId)
{
    return m_folderDao->folderNameExists(name, excludeId);
}

// Search operations
QList<Prompt*> PromptRepository::searchPrompts(const QString &searchText)
{
    return m_promptDao->searchPrompts(searchText);
}

QList<Prompt*> PromptRepository::searchPromptsInFolder(const QString &searchText, int folderId)
{
    return m_promptDao->searchPromptsInFolder(searchText, folderId);
}

// Combined operations
QList<PromptWithFolder*> PromptRepository::getPromptsWithFolders()
{
    return m_promptDao->getPromptsWithFolders();
}

// Statistics
int PromptRepository::getPromptCount()
{
    return m_promptDao->getPromptCount();
}

int PromptRepository::getFolderCount()
{
    return m_folderDao->getFolderCount();
}

int PromptRepository::getPromptCountByFolder(int folderId)
{
    return m_promptDao->getPromptCountByFolder(folderId);
}