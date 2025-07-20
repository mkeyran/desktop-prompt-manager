#ifndef PROMPT_H
#define PROMPT_H

#include <QObject>
#include <QString>
#include <QDateTime>

class Prompt : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)
    Q_PROPERTY(int folderId READ folderId WRITE setFolderId NOTIFY folderIdChanged)
    Q_PROPERTY(QDateTime createdAt READ createdAt WRITE setCreatedAt NOTIFY createdAtChanged)
    Q_PROPERTY(QDateTime updatedAt READ updatedAt WRITE setUpdatedAt NOTIFY updatedAtChanged)

public:
    explicit Prompt(QObject *parent = nullptr);
    Prompt(int id, const QString &title, const QString &content, 
           int folderId, const QDateTime &createdAt, const QDateTime &updatedAt,
           QObject *parent = nullptr);

    // Getters
    int id() const { return m_id; }
    QString title() const { return m_title; }
    QString content() const { return m_content; }
    int folderId() const { return m_folderId; }
    QDateTime createdAt() const { return m_createdAt; }
    QDateTime updatedAt() const { return m_updatedAt; }

    // Setters
    void setId(int id);
    void setTitle(const QString &title);
    void setContent(const QString &content);
    void setFolderId(int folderId);
    void setCreatedAt(const QDateTime &createdAt);
    void setUpdatedAt(const QDateTime &updatedAt);

    // Helper methods
    bool isValid() const { return m_id > 0; }
    void updateTimestamp() { setUpdatedAt(QDateTime::currentDateTime()); }

signals:
    void idChanged();
    void titleChanged();
    void contentChanged();
    void folderIdChanged();
    void createdAtChanged();
    void updatedAtChanged();

private:
    int m_id;
    QString m_title;
    QString m_content;
    int m_folderId; // -1 means no folder
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
};

#endif // PROMPT_H