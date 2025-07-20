#ifndef PROMPTDAO_H
#define PROMPTDAO_H

#include <QObject>
#include <QList>
#include "../models/prompt.h"
#include "../models/promptwithfolder.h"

class Database;
class QSqlQuery;

class PromptDao : public QObject
{
    Q_OBJECT

public:
    explicit PromptDao(Database *database, QObject *parent = nullptr);

    // CRUD operations
    bool insertPrompt(Prompt *prompt);
    bool updatePrompt(Prompt *prompt);
    bool deletePrompt(int promptId);
    Prompt* getPromptById(int promptId);
    
    // Query operations
    QList<Prompt*> getAllPrompts();
    QList<Prompt*> getPromptsByFolder(int folderId);
    QList<Prompt*> getPromptsWithoutFolder();
    QList<PromptWithFolder*> getPromptsWithFolders();
    
    // Search operations
    QList<Prompt*> searchPrompts(const QString &searchText);
    QList<Prompt*> searchPromptsInFolder(const QString &searchText, int folderId);
    
    // Utility operations
    int getPromptCount();
    int getPromptCountByFolder(int folderId);
    bool duplicatePrompt(int promptId);

private:
    Prompt* createPromptFromQuery(QSqlQuery &query);
    void bindPromptToQuery(QSqlQuery &query, Prompt *prompt);
    
    Database *m_database;
};

#endif // PROMPTDAO_H