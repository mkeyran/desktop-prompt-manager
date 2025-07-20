#ifndef PROMPTEDITVIEWMODEL_H
#define PROMPTEDITVIEWMODEL_H

#include <QObject>
#include "../models/prompt.h"
#include "../models/folder.h"

class PromptRepository;

class PromptEditViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)
    Q_PROPERTY(int selectedFolderId READ selectedFolderId WRITE setSelectedFolderId NOTIFY selectedFolderIdChanged)
    Q_PROPERTY(QList<QObject*> folders READ folders NOTIFY foldersChanged)
    Q_PROPERTY(bool isEditing READ isEditing NOTIFY isEditingChanged)
    Q_PROPERTY(bool hasPlaceholders READ hasPlaceholders NOTIFY hasPlaceholdersChanged)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(bool canSave READ canSave NOTIFY canSaveChanged)

public:
    explicit PromptEditViewModel(PromptRepository *repository, QObject *parent = nullptr);

    // Properties
    QString title() const { return m_title; }
    void setTitle(const QString &title);
    
    QString content() const { return m_content; }
    void setContent(const QString &content);
    
    int selectedFolderId() const { return m_selectedFolderId; }
    void setSelectedFolderId(int folderId);
    
    QList<QObject*> folders() const;
    bool isEditing() const { return m_isEditing; }
    bool hasPlaceholders() const;
    bool isLoading() const { return m_isLoading; }
    QString errorMessage() const { return m_errorMessage; }

    // Public methods
    Q_INVOKABLE void loadPrompt(int promptId);
    Q_INVOKABLE void createNewPrompt();
    Q_INVOKABLE bool savePrompt();
    Q_INVOKABLE void resetForm();
    Q_INVOKABLE bool canSave() const;
    
    // Folder management
    Q_INVOKABLE bool createFolder(const QString &name);
    Q_INVOKABLE void refreshFolders();

signals:
    void titleChanged();
    void contentChanged();
    void selectedFolderIdChanged();
    void foldersChanged();
    void isEditingChanged();
    void hasPlaceholdersChanged();
    void isLoadingChanged();
    void errorMessageChanged();
    void canSaveChanged();
    void promptSaved(int promptId);
    void folderCreated();

private:
    void setIsEditing(bool editing);
    void setIsLoading(bool loading);
    void setErrorMessage(const QString &message);
    void loadFolders();
    
    PromptRepository *m_repository;
    Prompt *m_currentPrompt;
    QList<Folder*> m_folders;
    QString m_title;
    QString m_content;
    int m_selectedFolderId;
    bool m_isEditing;
    bool m_isLoading;
    QString m_errorMessage;
};

#endif // PROMPTEDITVIEWMODEL_H