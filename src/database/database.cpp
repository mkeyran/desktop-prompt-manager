#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

Database* Database::m_instance = nullptr;

Database::Database(QObject *parent)
    : QObject(parent)
{
}

Database::~Database()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

Database* Database::instance()
{
    if (!m_instance) {
        m_instance = new Database();
    }
    return m_instance;
}

bool Database::initialize(const QString &databasePath)
{
    // Create directory if it doesn't exist
    QFileInfo fileInfo(databasePath);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            m_lastError = "Could not create database directory";
            return false;
        }
    }
    
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(databasePath);
    
    if (!m_database.open()) {
        m_lastError = m_database.lastError().text();
        qCritical() << "Database error:" << m_lastError;
        return false;
    }
    
    // Enable foreign keys
    QSqlQuery pragmaQuery(m_database);
    if (!pragmaQuery.exec("PRAGMA foreign_keys = ON")) {
        qWarning() << "Could not enable foreign keys:" << pragmaQuery.lastError().text();
    }
    
    return createTables();
}

bool Database::createTables()
{
    return createFoldersTable() && createPromptsTable();
}

bool Database::createFoldersTable()
{
    QSqlQuery query(m_database);
    QString createFoldersTable = R"(
        CREATE TABLE IF NOT EXISTS folders (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE,
            created_at INTEGER NOT NULL,
            updated_at INTEGER NOT NULL
        )
    )";
    
    if (!query.exec(createFoldersTable)) {
        m_lastError = "Failed to create folders table: " + query.lastError().text();
        qCritical() << m_lastError;
        return false;
    }
    
    return true;
}

bool Database::createPromptsTable()
{
    QSqlQuery query(m_database);
    QString createPromptsTable = R"(
        CREATE TABLE IF NOT EXISTS prompts (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            content TEXT NOT NULL,
            folder_id INTEGER,
            created_at INTEGER NOT NULL,
            updated_at INTEGER NOT NULL,
            FOREIGN KEY (folder_id) REFERENCES folders(id) ON DELETE SET NULL
        )
    )";
    
    if (!query.exec(createPromptsTable)) {
        m_lastError = "Failed to create prompts table: " + query.lastError().text();
        qCritical() << m_lastError;
        return false;
    }
    
    // Create index for search performance
    QString createSearchIndex = R"(
        CREATE INDEX IF NOT EXISTS idx_prompts_search 
        ON prompts(title, content)
    )";
    
    if (!query.exec(createSearchIndex)) {
        qWarning() << "Could not create search index:" << query.lastError().text();
    }
    
    return true;
}

bool Database::isValid() const
{
    return m_database.isOpen() && m_database.isValid();
}

QString Database::lastError() const
{
    return m_lastError;
}