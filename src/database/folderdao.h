#ifndef FOLDERDAO_H
#define FOLDERDAO_H

#include <QObject>
#include <QList>
#include "../models/folder.h"

class Database;
class QSqlQuery;

class FolderDao : public QObject
{
    Q_OBJECT

public:
    explicit FolderDao(Database *database, QObject *parent = nullptr);

    // CRUD operations
    bool insertFolder(Folder *folder);
    bool updateFolder(Folder *folder);
    bool deleteFolder(int folderId);
    Folder* getFolderById(int folderId);
    
    // Query operations
    QList<Folder*> getAllFolders();
    QList<Folder*> getFoldersWithCounts();
    
    // Utility operations
    int getFolderCount();
    bool folderNameExists(const QString &name, int excludeId = -1);

private:
    Folder* createFolderFromQuery(QSqlQuery &query);
    void bindFolderToQuery(QSqlQuery &query, Folder *folder);
    
    Database *m_database;
};

#endif // FOLDERDAO_H