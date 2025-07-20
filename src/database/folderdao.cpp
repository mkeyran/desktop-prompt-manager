#include "folderdao.h"
#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>

FolderDao::FolderDao(Database *database, QObject *parent)
    : QObject(parent), m_database(database)
{
}

bool FolderDao::insertFolder(Folder *folder)
{
    if (!folder || !m_database->isValid()) {
        return false;
    }
    
    // Check if name already exists
    if (folderNameExists(folder->name())) {
        qWarning() << "Folder name already exists:" << folder->name();
        return false;
    }
    
    QSqlQuery query(m_database->database());
    query.prepare(R"(
        INSERT INTO folders (name, created_at, updated_at)
        VALUES (:name, :created_at, :updated_at)
    )");
    
    bindFolderToQuery(query, folder);
    
    if (!query.exec()) {
        qCritical() << "Failed to insert folder:" << query.lastError().text();
        return false;
    }
    
    folder->setId(query.lastInsertId().toInt());
    return true;
}

bool FolderDao::updateFolder(Folder *folder)
{
    if (!folder || !folder->isValid() || !m_database->isValid()) {
        return false;
    }
    
    // Check if name already exists (excluding current folder)
    if (folderNameExists(folder->name(), folder->id())) {
        qWarning() << "Folder name already exists:" << folder->name();
        return false;
    }
    
    folder->updateTimestamp();
    
    QSqlQuery query(m_database->database());
    query.prepare(R"(
        UPDATE folders 
        SET name = :name, updated_at = :updated_at
        WHERE id = :id
    )");
    
    bindFolderToQuery(query, folder);
    query.bindValue(":id", folder->id());
    
    if (!query.exec()) {
        qCritical() << "Failed to update folder:" << query.lastError().text();
        return false;
    }
    
    return query.numRowsAffected() > 0;
}

bool FolderDao::deleteFolder(int folderId)
{
    if (folderId <= 0 || !m_database->isValid()) {
        return false;
    }
    
    QSqlQuery query(m_database->database());
    query.prepare("DELETE FROM folders WHERE id = :id");
    query.bindValue(":id", folderId);
    
    if (!query.exec()) {
        qCritical() << "Failed to delete folder:" << query.lastError().text();
        return false;
    }
    
    return query.numRowsAffected() > 0;
}

Folder* FolderDao::getFolderById(int folderId)
{
    if (folderId <= 0 || !m_database->isValid()) {
        return nullptr;
    }
    
    QSqlQuery query(m_database->database());
    query.prepare(R"(
        SELECT id, name, created_at, updated_at
        FROM folders WHERE id = :id
    )");
    query.bindValue(":id", folderId);
    
    if (!query.exec() || !query.next()) {
        return nullptr;
    }
    
    return createFolderFromQuery(query);
}

QList<Folder*> FolderDao::getAllFolders()
{
    QList<Folder*> folders;
    
    if (!m_database->isValid()) {
        return folders;
    }
    
    QSqlQuery query(m_database->database());
    query.prepare(R"(
        SELECT id, name, created_at, updated_at
        FROM folders ORDER BY name ASC
    )");
    
    if (!query.exec()) {
        qCritical() << "Failed to get all folders:" << query.lastError().text();
        return folders;
    }
    
    while (query.next()) {
        Folder *folder = createFolderFromQuery(query);
        if (folder) {
            folders.append(folder);
        }
    }
    
    return folders;
}

QList<Folder*> FolderDao::getFoldersWithCounts()
{
    QList<Folder*> folders;
    
    if (!m_database->isValid()) {
        return folders;
    }
    
    QSqlQuery query(m_database->database());
    query.prepare(R"(
        SELECT f.id, f.name, f.created_at, f.updated_at, 
               COUNT(p.id) as prompt_count
        FROM folders f
        LEFT JOIN prompts p ON f.id = p.folder_id
        GROUP BY f.id, f.name, f.created_at, f.updated_at
        ORDER BY f.name ASC
    )");
    
    if (!query.exec()) {
        qCritical() << "Failed to get folders with counts:" << query.lastError().text();
        return folders;
    }
    
    while (query.next()) {
        Folder *folder = createFolderFromQuery(query);
        if (folder) {
            folder->setPromptCount(query.value("prompt_count").toInt());
            folders.append(folder);
        }
    }
    
    return folders;
}

int FolderDao::getFolderCount()
{
    if (!m_database->isValid()) {
        return 0;
    }
    
    QSqlQuery query(m_database->database());
    query.prepare("SELECT COUNT(*) FROM folders");
    
    if (!query.exec() || !query.next()) {
        return 0;
    }
    
    return query.value(0).toInt();
}

bool FolderDao::folderNameExists(const QString &name, int excludeId)
{
    if (name.isEmpty() || !m_database->isValid()) {
        return false;
    }
    
    QSqlQuery query(m_database->database());
    
    if (excludeId > 0) {
        query.prepare("SELECT COUNT(*) FROM folders WHERE name = :name AND id != :exclude_id");
        query.bindValue(":exclude_id", excludeId);
    } else {
        query.prepare("SELECT COUNT(*) FROM folders WHERE name = :name");
    }
    
    query.bindValue(":name", name);
    
    if (!query.exec() || !query.next()) {
        return false;
    }
    
    return query.value(0).toInt() > 0;
}

Folder* FolderDao::createFolderFromQuery(QSqlQuery &query)
{
    int id = query.value("id").toInt();
    QString name = query.value("name").toString();
    QDateTime createdAt = QDateTime::fromSecsSinceEpoch(query.value("created_at").toLongLong());
    QDateTime updatedAt = QDateTime::fromSecsSinceEpoch(query.value("updated_at").toLongLong());
    
    return new Folder(id, name, createdAt, updatedAt, this);
}

void FolderDao::bindFolderToQuery(QSqlQuery &query, Folder *folder)
{
    query.bindValue(":name", folder->name());
    query.bindValue(":created_at", folder->createdAt().toSecsSinceEpoch());
    query.bindValue(":updated_at", folder->updatedAt().toSecsSinceEpoch());
}