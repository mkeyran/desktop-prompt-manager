#ifndef FOLDER_H
#define FOLDER_H

#include <QObject>
#include <QString>
#include <QDateTime>

class Folder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QDateTime createdAt READ createdAt WRITE setCreatedAt NOTIFY createdAtChanged)
    Q_PROPERTY(QDateTime updatedAt READ updatedAt WRITE setUpdatedAt NOTIFY updatedAtChanged)
    Q_PROPERTY(int promptCount READ promptCount WRITE setPromptCount NOTIFY promptCountChanged)

public:
    explicit Folder(QObject *parent = nullptr);
    Folder(int id, const QString &name, const QDateTime &createdAt, 
           const QDateTime &updatedAt, QObject *parent = nullptr);

    // Getters
    int id() const { return m_id; }
    QString name() const { return m_name; }
    QDateTime createdAt() const { return m_createdAt; }
    QDateTime updatedAt() const { return m_updatedAt; }
    int promptCount() const { return m_promptCount; }

    // Setters
    void setId(int id);
    void setName(const QString &name);
    void setCreatedAt(const QDateTime &createdAt);
    void setUpdatedAt(const QDateTime &updatedAt);
    void setPromptCount(int count);

    // Helper methods
    bool isValid() const { return m_id > 0; }
    void updateTimestamp() { setUpdatedAt(QDateTime::currentDateTime()); }

signals:
    void idChanged();
    void nameChanged();
    void createdAtChanged();
    void updatedAtChanged();
    void promptCountChanged();

private:
    int m_id;
    QString m_name;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
    int m_promptCount; // For display purposes
};

#endif // FOLDER_H