#include "folder.h"

Folder::Folder(QObject *parent)
    : QObject(parent), m_id(-1), m_promptCount(0)
{
    QDateTime now = QDateTime::currentDateTime();
    m_createdAt = now;
    m_updatedAt = now;
}

Folder::Folder(int id, const QString &name, const QDateTime &createdAt,
               const QDateTime &updatedAt, QObject *parent)
    : QObject(parent), m_id(id), m_name(name), m_createdAt(createdAt),
      m_updatedAt(updatedAt), m_promptCount(0)
{
}

void Folder::setId(int id)
{
    if (m_id != id) {
        m_id = id;
        emit idChanged();
    }
}

void Folder::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}

void Folder::setCreatedAt(const QDateTime &createdAt)
{
    if (m_createdAt != createdAt) {
        m_createdAt = createdAt;
        emit createdAtChanged();
    }
}

void Folder::setUpdatedAt(const QDateTime &updatedAt)
{
    if (m_updatedAt != updatedAt) {
        m_updatedAt = updatedAt;
        emit updatedAtChanged();
    }
}

void Folder::setPromptCount(int count)
{
    if (m_promptCount != count) {
        m_promptCount = count;
        emit promptCountChanged();
    }
}