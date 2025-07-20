#include "promptdao.h"
#include "database.h"
#include "../models/folder.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>

PromptDao::PromptDao(Database *database, QObject *parent)
    : QObject(parent), m_database(database)
{
}

bool PromptDao::insertPrompt(Prompt *prompt)
{
    if (!prompt || !m_database->isValid()) {
        return false;
    }
    
    QSqlQuery query(m_database->database());
    query.prepare(R"(
        INSERT INTO prompts (title, content, folder_id, created_at, updated_at)
        VALUES (:title, :content, :folder_id, :created_at, :updated_at)
    )");
    
    bindPromptToQuery(query, prompt);
    
    if (!query.exec()) {
        qCritical() << "Failed to insert prompt:" << query.lastError().text();
        return false;
    }
    
    prompt->setId(query.lastInsertId().toInt());
    return true;
}

bool PromptDao::updatePrompt(Prompt *prompt)
{
    if (!prompt || !prompt->isValid() || !m_database->isValid()) {
        return false;
    }
    
    prompt->updateTimestamp();
    
    QSqlQuery query(m_database->database());
    query.prepare(R"(
        UPDATE prompts 
        SET title = :title, content = :content, folder_id = :folder_id, 
            updated_at = :updated_at
        WHERE id = :id
    )");
    
    bindPromptToQuery(query, prompt);
    query.bindValue(":id", prompt->id());
    
    if (!query.exec()) {
        qCritical() << "Failed to update prompt:" << query.lastError().text();
        return false;
    }
    
    return query.numRowsAffected() > 0;
}

bool PromptDao::deletePrompt(int promptId)
{
    if (promptId <= 0 || !m_database->isValid()) {
        return false;
    }
    
    QSqlQuery query(m_database->database());
    query.prepare("DELETE FROM prompts WHERE id = :id");
    query.bindValue(":id", promptId);
    
    if (!query.exec()) {
        qCritical() << "Failed to delete prompt:" << query.lastError().text();
        return false;
    }
    
    return query.numRowsAffected() > 0;
}

Prompt* PromptDao::getPromptById(int promptId)
{
    if (promptId <= 0 || !m_database->isValid()) {
        return nullptr;
    }
    
    QSqlQuery query(m_database->database());
    query.prepare(R"(
        SELECT id, title, content, folder_id, created_at, updated_at
        FROM prompts WHERE id = :id
    )");
    query.bindValue(":id", promptId);
    
    if (!query.exec() || !query.next()) {
        return nullptr;
    }
    
    return createPromptFromQuery(query);
}

QList<Prompt*> PromptDao::getAllPrompts()
{
    QList<Prompt*> prompts;
    
    if (!m_database->isValid()) {
        return prompts;
    }
    
    QSqlQuery query(m_database->database());
    query.prepare(R"(
        SELECT id, title, content, folder_id, created_at, updated_at
        FROM prompts ORDER BY updated_at DESC
    )");
    
    if (!query.exec()) {
        qCritical() << "Failed to get all prompts:" << query.lastError().text();
        return prompts;
    }
    
    while (query.next()) {
        Prompt *prompt = createPromptFromQuery(query);
        if (prompt) {
            prompts.append(prompt);
        }
    }
    
    return prompts;
}

QList<Prompt*> PromptDao::getPromptsByFolder(int folderId)
{
    QList<Prompt*> prompts;
    
    if (!m_database->isValid()) {
        return prompts;
    }
    
    QSqlQuery query(m_database->database());
    query.prepare(R"(
        SELECT id, title, content, folder_id, created_at, updated_at
        FROM prompts WHERE folder_id = :folder_id ORDER BY updated_at DESC
    )");
    query.bindValue(":folder_id", folderId);
    
    if (!query.exec()) {
        qCritical() << "Failed to get prompts by folder:" << query.lastError().text();
        return prompts;
    }
    
    while (query.next()) {
        Prompt *prompt = createPromptFromQuery(query);
        if (prompt) {
            prompts.append(prompt);
        }
    }
    
    return prompts;
}

QList<Prompt*> PromptDao::getPromptsWithoutFolder()
{
    QList<Prompt*> prompts;
    
    if (!m_database->isValid()) {
        return prompts;
    }
    
    QSqlQuery query(m_database->database());
    query.prepare(R"(
        SELECT id, title, content, folder_id, created_at, updated_at
        FROM prompts WHERE folder_id IS NULL ORDER BY updated_at DESC
    )");
    
    if (!query.exec()) {
        qCritical() << "Failed to get prompts without folder:" << query.lastError().text();
        return prompts;
    }
    
    while (query.next()) {
        Prompt *prompt = createPromptFromQuery(query);
        if (prompt) {
            prompts.append(prompt);
        }
    }
    
    return prompts;
}

QList<Prompt*> PromptDao::searchPrompts(const QString &searchText)
{
    QList<Prompt*> prompts;
    
    if (searchText.isEmpty() || !m_database->isValid()) {
        return prompts;
    }
    
    QSqlQuery query(m_database->database());
    query.prepare(R"(
        SELECT id, title, content, folder_id, created_at, updated_at
        FROM prompts 
        WHERE title LIKE :search OR content LIKE :search
        ORDER BY updated_at DESC
    )");
    
    QString searchPattern = "%" + searchText + "%";
    query.bindValue(":search", searchPattern);
    
    if (!query.exec()) {
        qCritical() << "Failed to search prompts:" << query.lastError().text();
        return prompts;
    }
    
    while (query.next()) {
        Prompt *prompt = createPromptFromQuery(query);
        if (prompt) {
            prompts.append(prompt);
        }
    }
    
    return prompts;
}

QList<Prompt*> PromptDao::searchPromptsInFolder(const QString &searchText, int folderId)
{
    QList<Prompt*> prompts;
    
    if (searchText.isEmpty() || !m_database->isValid()) {
        return prompts;
    }
    
    QSqlQuery query(m_database->database());
    query.prepare(R"(
        SELECT id, title, content, folder_id, created_at, updated_at
        FROM prompts 
        WHERE (title LIKE :search OR content LIKE :search) AND folder_id = :folder_id
        ORDER BY updated_at DESC
    )");
    
    QString searchPattern = "%" + searchText + "%";
    query.bindValue(":search", searchPattern);
    query.bindValue(":folder_id", folderId);
    
    if (!query.exec()) {
        qCritical() << "Failed to search prompts in folder:" << query.lastError().text();
        return prompts;
    }
    
    while (query.next()) {
        Prompt *prompt = createPromptFromQuery(query);
        if (prompt) {
            prompts.append(prompt);
        }
    }
    
    return prompts;
}

QList<PromptWithFolder*> PromptDao::getPromptsWithFolders()
{
    QList<PromptWithFolder*> promptsWithFolders;
    
    if (!m_database->isValid()) {
        return promptsWithFolders;
    }
    
    QSqlQuery query(m_database->database());
    query.prepare(R"(
        SELECT p.id, p.title, p.content, p.folder_id, p.created_at, p.updated_at,
               f.name as folder_name, f.created_at as folder_created_at, f.updated_at as folder_updated_at
        FROM prompts p
        LEFT JOIN folders f ON p.folder_id = f.id
        ORDER BY p.updated_at DESC
    )");
    
    if (!query.exec()) {
        qCritical() << "Failed to get prompts with folders:" << query.lastError().text();
        return promptsWithFolders;
    }
    
    while (query.next()) {
        Prompt *prompt = createPromptFromQuery(query);
        if (!prompt) continue;
        
        Folder *folder = nullptr;
        if (!query.value("folder_name").isNull()) {
            int folderId = query.value("folder_id").toInt();
            QString folderName = query.value("folder_name").toString();
            QDateTime folderCreatedAt = QDateTime::fromSecsSinceEpoch(query.value("folder_created_at").toLongLong());
            QDateTime folderUpdatedAt = QDateTime::fromSecsSinceEpoch(query.value("folder_updated_at").toLongLong());
            
            folder = new Folder(folderId, folderName, folderCreatedAt, folderUpdatedAt, this);
        }
        
        PromptWithFolder *promptWithFolder = new PromptWithFolder(prompt, folder, this);
        promptsWithFolders.append(promptWithFolder);
    }
    
    return promptsWithFolders;
}

bool PromptDao::duplicatePrompt(int promptId)
{
    Prompt *original = getPromptById(promptId);
    if (!original) {
        return false;
    }
    
    Prompt *copy = new Prompt(this);
    copy->setTitle(original->title() + " (Copy)");
    copy->setContent(original->content());
    copy->setFolderId(original->folderId());
    
    bool success = insertPrompt(copy);
    if (!success) {
        delete copy;
    }
    
    delete original;
    return success;
}

int PromptDao::getPromptCount()
{
    if (!m_database->isValid()) {
        return 0;
    }
    
    QSqlQuery query(m_database->database());
    query.prepare("SELECT COUNT(*) FROM prompts");
    
    if (!query.exec() || !query.next()) {
        return 0;
    }
    
    return query.value(0).toInt();
}

int PromptDao::getPromptCountByFolder(int folderId)
{
    if (!m_database->isValid()) {
        return 0;
    }
    
    QSqlQuery query(m_database->database());
    query.prepare("SELECT COUNT(*) FROM prompts WHERE folder_id = :folder_id");
    query.bindValue(":folder_id", folderId);
    
    if (!query.exec() || !query.next()) {
        return 0;
    }
    
    return query.value(0).toInt();
}

Prompt* PromptDao::createPromptFromQuery(QSqlQuery &query)
{
    int id = query.value("id").toInt();
    QString title = query.value("title").toString();
    QString content = query.value("content").toString();
    int folderId = query.value("folder_id").isNull() ? -1 : query.value("folder_id").toInt();
    QDateTime createdAt = QDateTime::fromSecsSinceEpoch(query.value("created_at").toLongLong());
    QDateTime updatedAt = QDateTime::fromSecsSinceEpoch(query.value("updated_at").toLongLong());
    
    return new Prompt(id, title, content, folderId, createdAt, updatedAt, this);
}

void PromptDao::bindPromptToQuery(QSqlQuery &query, Prompt *prompt)
{
    query.bindValue(":title", prompt->title());
    query.bindValue(":content", prompt->content());
    
    if (prompt->folderId() > 0) {
        query.bindValue(":folder_id", prompt->folderId());
    } else {
        query.bindValue(":folder_id", QVariant());
    }
    
    query.bindValue(":created_at", prompt->createdAt().toSecsSinceEpoch());
    query.bindValue(":updated_at", prompt->updatedAt().toSecsSinceEpoch());
}