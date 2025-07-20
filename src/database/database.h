#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>

class Database : public QObject
{
    Q_OBJECT

public:
    static Database* instance();
    
    bool initialize(const QString &databasePath);
    QSqlDatabase& database() { return m_database; }
    
    bool isValid() const;
    QString lastError() const;

private:
    explicit Database(QObject *parent = nullptr);
    ~Database();
    
    bool createTables();
    bool createPromptsTable();
    bool createFoldersTable();
    
    static Database* m_instance;
    QSqlDatabase m_database;
    QString m_lastError;
};

#endif // DATABASE_H