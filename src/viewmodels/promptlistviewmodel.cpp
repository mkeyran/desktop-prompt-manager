#include "promptlistviewmodel.h"
#include "../repository/promptrepository.h"
#include <QDebug>

PromptListViewModel::PromptListViewModel(PromptRepository *repository, QObject *parent)
    : QAbstractListModel(parent), m_repository(repository), m_selectedFolderId(-1),
      m_isLoading(false)
{
    m_searchTimer = new QTimer(this);
    m_searchTimer->setSingleShot(true);
    m_searchTimer->setInterval(300); // 300ms debounce
    connect(m_searchTimer, &QTimer::timeout, this, &PromptListViewModel::onSearchTimerTimeout);
    
    // Connect to repository signals
    connect(m_repository, &PromptRepository::dataChanged, this, &PromptListViewModel::onDataChanged);
    
    // Load initial data
    refreshData();
}

int PromptListViewModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_prompts.size();
}

QVariant PromptListViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_prompts.size()) {
        return QVariant();
    }
    
    Prompt *prompt = m_prompts.at(index.row());
    
    switch (role) {
    case IdRole:
        return prompt->id();
    case TitleRole:
        return prompt->title();
    case ContentRole:
        return prompt->content();
    case FolderIdRole:
        return prompt->folderId();
    case FolderNameRole: {
        if (prompt->folderId() > 0) {
            for (Folder *folder : m_folders) {
                if (folder->id() == prompt->folderId()) {
                    return folder->name();
                }
            }
        }
        return QString();
    }
    case CreatedAtRole:
        return prompt->createdAt();
    case UpdatedAtRole:
        return prompt->updatedAt();
    case PromptObjectRole:
        return QVariant::fromValue(prompt);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> PromptListViewModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[TitleRole] = "title";
    roles[ContentRole] = "content";
    roles[FolderIdRole] = "folderId";
    roles[FolderNameRole] = "folderName";
    roles[CreatedAtRole] = "createdAt";
    roles[UpdatedAtRole] = "updatedAt";
    roles[PromptObjectRole] = "promptObject";
    return roles;
}

void PromptListViewModel::setSearchText(const QString &searchText)
{
    if (m_searchText != searchText) {
        m_searchText = searchText;
        emit searchTextChanged();
        
        // Restart the search timer
        m_searchTimer->stop();
        m_searchTimer->start();
    }
}

void PromptListViewModel::setSelectedFolderId(int folderId)
{
    if (m_selectedFolderId != folderId) {
        m_selectedFolderId = folderId;
        emit selectedFolderIdChanged();
        loadPrompts();
    }
}

QList<QObject*> PromptListViewModel::folders() const
{
    QList<QObject*> result;
    for (Folder *folder : m_folders) {
        result.append(folder);
    }
    return result;
}

void PromptListViewModel::refreshData()
{
    loadFolders();
    loadPrompts();
}

void PromptListViewModel::deletePrompt(int promptId)
{
    setIsLoading(true);
    
    if (m_repository->deletePrompt(promptId)) {
        loadPrompts();
        emit promptDeleted(promptId);
    } else {
        setErrorMessage("Failed to delete prompt");
    }
    
    setIsLoading(false);
}

void PromptListViewModel::duplicatePrompt(int promptId)
{
    setIsLoading(true);
    
    if (m_repository->duplicatePrompt(promptId)) {
        loadPrompts();
        emit promptDuplicated();
    } else {
        setErrorMessage("Failed to duplicate prompt");
    }
    
    setIsLoading(false);
}

Prompt* PromptListViewModel::getPromptById(int promptId)
{
    for (Prompt *prompt : m_prompts) {
        if (prompt->id() == promptId) {
            return prompt;
        }
    }
    return nullptr;
}

void PromptListViewModel::onSearchTimerTimeout()
{
    loadPrompts();
}

void PromptListViewModel::onDataChanged()
{
    refreshData();
}

void PromptListViewModel::loadPrompts()
{
    setIsLoading(true);
    setErrorMessage("");
    
    beginResetModel();
    
    // Clear existing prompts
    qDeleteAll(m_prompts);
    m_prompts.clear();
    
    try {
        if (!m_searchText.isEmpty()) {
            // Search with optional folder filter
            if (m_selectedFolderId > 0) {
                m_prompts = m_repository->searchPromptsInFolder(m_searchText, m_selectedFolderId);
            } else {
                m_prompts = m_repository->searchPrompts(m_searchText);
            }
        } else {
            // Load by folder or all
            if (m_selectedFolderId > 0) {
                m_prompts = m_repository->getPromptsByFolder(m_selectedFolderId);
            } else if (m_selectedFolderId == 0) {
                m_prompts = m_repository->getPromptsWithoutFolder();
            } else {
                m_prompts = m_repository->getAllPrompts();
            }
        }
    } catch (const std::exception &e) {
        setErrorMessage(QString("Failed to load prompts: %1").arg(e.what()));
    }
    
    endResetModel();
    setIsLoading(false);
}

void PromptListViewModel::loadFolders()
{
    // Clear existing folders
    qDeleteAll(m_folders);
    m_folders.clear();
    
    try {
        m_folders = m_repository->getFoldersWithCounts();
        emit foldersChanged();
    } catch (const std::exception &e) {
        setErrorMessage(QString("Failed to load folders: %1").arg(e.what()));
    }
}

void PromptListViewModel::setIsLoading(bool loading)
{
    if (m_isLoading != loading) {
        m_isLoading = loading;
        emit isLoadingChanged();
    }
}

void PromptListViewModel::setErrorMessage(const QString &message)
{
    if (m_errorMessage != message) {
        m_errorMessage = message;
        emit errorMessageChanged();
    }
}