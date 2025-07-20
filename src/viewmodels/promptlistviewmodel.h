#ifndef PROMPTLISTVIEWMODEL_H
#define PROMPTLISTVIEWMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QTimer>
#include "../models/prompt.h"
#include "../models/folder.h"

class PromptRepository;

class PromptListViewModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)
    Q_PROPERTY(int selectedFolderId READ selectedFolderId WRITE setSelectedFolderId NOTIFY selectedFolderIdChanged)
    Q_PROPERTY(QList<QObject*> folders READ folders NOTIFY foldersChanged)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    enum PromptRoles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        ContentRole,
        FolderIdRole,
        FolderNameRole,
        CreatedAtRole,
        UpdatedAtRole,
        PromptObjectRole
    };

    explicit PromptListViewModel(PromptRepository *repository, QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Properties
    QString searchText() const { return m_searchText; }
    void setSearchText(const QString &searchText);
    
    int selectedFolderId() const { return m_selectedFolderId; }
    void setSelectedFolderId(int folderId);
    
    QList<QObject*> folders() const;
    bool isLoading() const { return m_isLoading; }
    QString errorMessage() const { return m_errorMessage; }

    // Public methods
    Q_INVOKABLE void refreshData();
    Q_INVOKABLE void deletePrompt(int promptId);
    Q_INVOKABLE void duplicatePrompt(int promptId);
    Q_INVOKABLE Prompt* getPromptById(int promptId);

signals:
    void searchTextChanged();
    void selectedFolderIdChanged();
    void foldersChanged();
    void isLoadingChanged();
    void errorMessageChanged();
    void promptDeleted(int promptId);
    void promptDuplicated();

private slots:
    void onSearchTimerTimeout();
    void onDataChanged();

private:
    void loadPrompts();
    void loadFolders();
    void setIsLoading(bool loading);
    void setErrorMessage(const QString &message);
    
    PromptRepository *m_repository;
    QList<Prompt*> m_prompts;
    QList<Folder*> m_folders;
    QString m_searchText;
    int m_selectedFolderId;
    bool m_isLoading;
    QString m_errorMessage;
    QTimer *m_searchTimer;
};

#endif // PROMPTLISTVIEWMODEL_H