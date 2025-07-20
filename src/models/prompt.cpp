#include "prompt.h"

Prompt::Prompt(QObject *parent)
    : QObject(parent), m_id(-1), m_folderId(-1)
{
    QDateTime now = QDateTime::currentDateTime();
    m_createdAt = now;
    m_updatedAt = now;
}

Prompt::Prompt(int id, const QString &title, const QString &content,
               int folderId, const QDateTime &createdAt, const QDateTime &updatedAt,
               QObject *parent)
    : QObject(parent), m_id(id), m_title(title), m_content(content),
      m_folderId(folderId), m_createdAt(createdAt), m_updatedAt(updatedAt)
{
}

void Prompt::setId(int id)
{
    if (m_id != id) {
        m_id = id;
        emit idChanged();
    }
}

void Prompt::setTitle(const QString &title)
{
    if (m_title != title) {
        m_title = title;
        emit titleChanged();
    }
}

void Prompt::setContent(const QString &content)
{
    if (m_content != content) {
        m_content = content;
        emit contentChanged();
    }
}

void Prompt::setFolderId(int folderId)
{
    if (m_folderId != folderId) {
        m_folderId = folderId;
        emit folderIdChanged();
    }
}

void Prompt::setCreatedAt(const QDateTime &createdAt)
{
    if (m_createdAt != createdAt) {
        m_createdAt = createdAt;
        emit createdAtChanged();
    }
}

void Prompt::setUpdatedAt(const QDateTime &updatedAt)
{
    if (m_updatedAt != updatedAt) {
        m_updatedAt = updatedAt;
        emit updatedAtChanged();
    }
}