#include "prompteditviewmodel.h"
#include "../repository/promptrepository.h"
#include "../utils/placeholderutils.h"
#include <QDebug>

PromptEditViewModel::PromptEditViewModel(PromptRepository *repository, QObject *parent)
    : QObject(parent), m_repository(repository), m_currentPrompt(nullptr),
      m_selectedFolderId(-1), m_isEditing(false), m_isLoading(false)
{
    loadFolders();
}

void PromptEditViewModel::setTitle(const QString &title)
{
    if (m_title != title) {
        m_title = title;
        emit titleChanged();
        emit canSaveChanged();
    }
}

void PromptEditViewModel::setContent(const QString &content)
{
    if (m_content != content) {
        m_content = content;
        emit contentChanged();
        emit hasPlaceholdersChanged();
        emit canSaveChanged();
    }
}

void PromptEditViewModel::setSelectedFolderId(int folderId)
{
    if (m_selectedFolderId != folderId) {
        m_selectedFolderId = folderId;
        emit selectedFolderIdChanged();
    }
}

QList<QObject*> PromptEditViewModel::folders() const
{
    QList<QObject*> result;
    for (Folder *folder : m_folders) {
        result.append(folder);
    }
    return result;
}

bool PromptEditViewModel::hasPlaceholders() const
{
    return PlaceholderUtils::hasPlaceholders(m_content);
}

void PromptEditViewModel::loadPrompt(int promptId)
{
    setIsLoading(true);
    setErrorMessage("");
    
    if (m_currentPrompt) {
        delete m_currentPrompt;
        m_currentPrompt = nullptr;
    }
    
    try {
        m_currentPrompt = m_repository->getPromptById(promptId);
        if (m_currentPrompt) {
            setTitle(m_currentPrompt->title());
            setContent(m_currentPrompt->content());
            setSelectedFolderId(m_currentPrompt->folderId());
            setIsEditing(true);
        } else {
            setErrorMessage("Prompt not found");
        }
    } catch (const std::exception &e) {
        setErrorMessage(QString("Failed to load prompt: %1").arg(e.what()));
    }
    
    setIsLoading(false);
}

void PromptEditViewModel::createNewPrompt()
{
    if (m_currentPrompt) {
        delete m_currentPrompt;
    }
    
    m_currentPrompt = new Prompt(this);
    setTitle("");
    setContent("");
    setSelectedFolderId(-1);
    setIsEditing(false);
    setErrorMessage("");
}

bool PromptEditViewModel::savePrompt()
{
    if (!canSave()) {
        setErrorMessage("Please fill in all required fields");
        return false;
    }
    
    setIsLoading(true);
    setErrorMessage("");
    
    try {
        if (!m_currentPrompt) {
            m_currentPrompt = new Prompt(this);
        }
        
        m_currentPrompt->setTitle(m_title);
        m_currentPrompt->setContent(m_content);
        m_currentPrompt->setFolderId(m_selectedFolderId > 0 ? m_selectedFolderId : -1);
        
        bool success = m_repository->savePrompt(m_currentPrompt);
        if (success) {
            setIsEditing(true);
            emit promptSaved(m_currentPrompt->id());
        } else {
            setErrorMessage("Failed to save prompt");
        }
        
        setIsLoading(false);
        return success;
    } catch (const std::exception &e) {
        setErrorMessage(QString("Failed to save prompt: %1").arg(e.what()));
        setIsLoading(false);
        return false;
    }
}

void PromptEditViewModel::resetForm()
{
    createNewPrompt();
}

bool PromptEditViewModel::canSave() const
{
    return !m_title.trimmed().isEmpty() && !m_content.trimmed().isEmpty();
}

bool PromptEditViewModel::createFolder(const QString &name)
{
    if (name.trimmed().isEmpty()) {
        setErrorMessage("Folder name cannot be empty");
        return false;
    }
    
    if (m_repository->folderNameExists(name.trimmed())) {
        setErrorMessage("Folder name already exists");
        return false;
    }
    
    setIsLoading(true);
    setErrorMessage("");
    
    try {
        Folder *folder = new Folder(this);
        folder->setName(name.trimmed());
        
        bool success = m_repository->saveFolder(folder);
        if (success) {
            loadFolders();
            setSelectedFolderId(folder->id());
            emit folderCreated();
        } else {
            setErrorMessage("Failed to create folder");
            delete folder;
        }
        
        setIsLoading(false);
        return success;
    } catch (const std::exception &e) {
        setErrorMessage(QString("Failed to create folder: %1").arg(e.what()));
        setIsLoading(false);
        return false;
    }
}

void PromptEditViewModel::refreshFolders()
{
    loadFolders();
}

void PromptEditViewModel::setIsEditing(bool editing)
{
    if (m_isEditing != editing) {
        m_isEditing = editing;
        emit isEditingChanged();
    }
}

void PromptEditViewModel::setIsLoading(bool loading)
{
    if (m_isLoading != loading) {
        m_isLoading = loading;
        emit isLoadingChanged();
    }
}

void PromptEditViewModel::setErrorMessage(const QString &message)
{
    if (m_errorMessage != message) {
        m_errorMessage = message;
        emit errorMessageChanged();
    }
}

void PromptEditViewModel::loadFolders()
{
    qDeleteAll(m_folders);
    m_folders.clear();
    
    try {
        m_folders = m_repository->getAllFolders();
        emit foldersChanged();
    } catch (const std::exception &e) {
        setErrorMessage(QString("Failed to load folders: %1").arg(e.what()));
    }
}