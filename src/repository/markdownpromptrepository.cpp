#include "markdownpromptrepository.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QFileInfo>
#include <QRegularExpression>
#include <QDebug>

MarkdownPromptRepository::MarkdownPromptRepository(const QString &rootPath, QObject *parent)
    : PromptRepository(parent), m_rootPath(rootPath)
{
    QDir dir(m_rootPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    reload();
}

void MarkdownPromptRepository::setRootPath(const QString &rootPath)
{
    if (m_rootPath == rootPath) return;

    m_rootPath = rootPath;
    QDir dir(m_rootPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    reload();
}

MarkdownPromptRepository::~MarkdownPromptRepository()
{
    qDeleteAll(m_prompts);
    qDeleteAll(m_folders);
}

void MarkdownPromptRepository::reload()
{
    qDeleteAll(m_prompts);
    qDeleteAll(m_folders);
    m_prompts.clear();
    m_folders.clear();
    m_nextPromptId = 1;
    m_nextFolderId = 1;

    scanDirectory(QDir(m_rootPath), -1);
    emit dataChanged();
}

void MarkdownPromptRepository::scanDirectory(const QDir &dir, int parentFolderId)
{
    // List Directories (Folders)
    QFileInfoList subdirList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const QFileInfo &subdirInfo : subdirList) {
        int folderId = m_nextFolderId++;
        Folder *folder = new Folder(folderId, subdirInfo.fileName(), subdirInfo.birthTime(), subdirInfo.lastModified(), this);
        // Store path in object? No, Folder model doesn't have path. 
        // We implicitly assume: root + folderName = path. 
        // Nested folders not fully supported by Folder model (flat list?), but we scan recursively.
        // Existing app seems to assume flat folders based on database schema usually.
        // Let's assume 1 level of folders for now as per likely app logic.
        m_folders.append(folder);
        
        // Scan for prompts inside this folder
        QDir subDir(subdirInfo.absoluteFilePath());
        QFileInfoList fileList = subDir.entryInfoList(QStringList() << "*.md", QDir::Files, QDir::Name);
        for (const QFileInfo &fileInfo : fileList) {
            int promptId = m_nextPromptId++;
            Prompt *prompt = parsePromptFile(fileInfo.absoluteFilePath(), promptId, folderId);
            if (prompt) {
                m_prompts.append(prompt);
            }
        }
    }

    // List Files in Root (Prompts without folder)
    if (parentFolderId == -1) {
        QFileInfoList fileList = dir.entryInfoList(QStringList() << "*.md", QDir::Files, QDir::Name);
        for (const QFileInfo &fileInfo : fileList) {
            int promptId = m_nextPromptId++;
            Prompt *prompt = parsePromptFile(fileInfo.absoluteFilePath(), promptId, -1);
            if (prompt) {
                m_prompts.append(prompt);
            }
        }
    }
}

Prompt* MarkdownPromptRepository::parsePromptFile(const QString &filePath, int id, int folderId)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return nullptr;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    QMap<QString, QString> frontMatter;
    QString body = extractFrontMatter(content, frontMatter);
    
    QString title = frontMatter.value("title");
    if (title.isEmpty()) {
        title = QFileInfo(filePath).baseName();
    }
    
    QDateTime createdAt = QDateTime::fromString(frontMatter.value("createdAt"), Qt::ISODate);
    if (!createdAt.isValid()) createdAt = QFileInfo(filePath).birthTime();
    
    QDateTime updatedAt = QDateTime::fromString(frontMatter.value("updatedAt"), Qt::ISODate);
    if (!updatedAt.isValid()) updatedAt = QFileInfo(filePath).lastModified();

    return new Prompt(id, title, body, folderId, createdAt, updatedAt, this);
}

QString MarkdownPromptRepository::extractFrontMatter(const QString &content, QMap<QString, QString> &frontMatter)
{
    if (!content.startsWith("---\n")) {
        return content;
    }

    int end = content.indexOf("\n---\n", 4);
    if (end == -1) {
        return content;
    }

    QString frontMatterStr = content.mid(4, end - 4);
    QString body = content.mid(end + 5);

    QStringList lines = frontMatterStr.split('\n');
    for (const QString &line : lines) {
        int colon = line.indexOf(':');
        if (colon != -1) {
            QString key = line.left(colon).trimmed();
            QString value = line.mid(colon + 1).trimmed();
            frontMatter.insert(key, value);
        }
    }

    return body;
}

QString MarkdownPromptRepository::generateFrontMatter(const QMap<QString, QString> &frontMatter)
{
    QString fm = "---\n";
    QMapIterator<QString, QString> i(frontMatter);
    while (i.hasNext()) {
        i.next();
        fm += i.key() + ": " + i.value() + "\n";
    }
    fm += "---\n";
    return fm;
}

// Prompt operations
bool MarkdownPromptRepository::savePrompt(Prompt *prompt)
{
    if (!prompt) return false;

    // Determine target folder path (Use internal lookup)
    QString folderPath = m_rootPath;
    if (prompt->folderId() != -1) {
        Folder *f = nullptr;
        for (Folder *folder : m_folders) {
            if (folder->id() == prompt->folderId()) {
                f = folder;
                break;
            }
        }
        
        if (f) {
            folderPath = QDir(m_rootPath).filePath(f->name());
        }
    }
    
    QDir dir(folderPath);
    if (!dir.exists()) dir.mkpath(".");

    // Sanitize title for filename
    QString safeTitle = prompt->title();
    safeTitle.replace(QRegularExpression("[^a-zA-Z0-9_\\-\\s]"), "");
    safeTitle = safeTitle.trimmed();
    if (safeTitle.isEmpty()) safeTitle = "Untitled";
    
    QString fileName = safeTitle + ".md";
    QString filePath = dir.filePath(fileName);

    // If ID is new, assign it
    if (!prompt->isValid()) {
        prompt->setId(m_nextPromptId++);
        prompt->setCreatedAt(QDateTime::currentDateTime());
        prompt->setUpdatedAt(QDateTime::currentDateTime());
        
        // Add COPY to cache so we own it
        Prompt* cacheCopy = new Prompt(prompt->id(), prompt->title(), prompt->content(), 
                                      prompt->folderId(), prompt->createdAt(), prompt->updatedAt(), this);
        m_prompts.append(cacheCopy);
        
        emit promptAdded(prompt); // Optimistic add to UI
    } else {
        // Internal lookup to find existing cache object
        Prompt* existing = nullptr;
        for (Prompt *p : m_prompts) {
            if (p->id() == prompt->id()) {
                existing = p;
                break;
            }
        }
        
        if (existing) {
             // Handle file operations (delete old if needed)
             QString oldFolderPath = m_rootPath;
             if (existing->folderId() != -1) {
                 Folder *oldF = nullptr;
                 for (Folder *f : m_folders) {
                     if (f->id() == existing->folderId()) {
                         oldF = f;
                         break;
                     }
                 }
                 if (oldF) oldFolderPath = QDir(m_rootPath).filePath(oldF->name());
             }
             
             QString oldSafeTitle = existing->title();
             oldSafeTitle.replace(QRegularExpression("[^a-zA-Z0-9_\\-\\s]"), "");
             oldSafeTitle = oldSafeTitle.trimmed();
             if (oldSafeTitle.isEmpty()) oldSafeTitle = "Untitled";
             QString oldFilePath = QDir(oldFolderPath).filePath(oldSafeTitle + ".md");
             
             if (oldFilePath != filePath && QFile::exists(oldFilePath)) {
                 QFile::remove(oldFilePath);
             }
             
             // Update cache object
             existing->setTitle(prompt->title());
             existing->setContent(prompt->content());
             existing->setFolderId(prompt->folderId());
             existing->setUpdatedAt(QDateTime::currentDateTime());
        }
        
        prompt->setUpdatedAt(QDateTime::currentDateTime());
        emit promptUpdated(prompt);
    }
    
    writePromptFile(prompt, filePath);
    emit dataChanged();
    return true;
}

void MarkdownPromptRepository::writePromptFile(Prompt *prompt, const QString &filePath)
{
    QMap<QString, QString> frontMatter;
    frontMatter.insert("title", prompt->title());
    frontMatter.insert("createdAt", prompt->createdAt().toString(Qt::ISODate));
    frontMatter.insert("updatedAt", prompt->updatedAt().toString(Qt::ISODate));
    
    QString content = generateFrontMatter(frontMatter) + prompt->content();
    
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << content;
        file.close();
    }
}

bool MarkdownPromptRepository::deletePrompt(int promptId)
{
    Prompt *p = nullptr;
    for (Prompt *prompt : m_prompts) {
        if (prompt->id() == promptId) {
            p = prompt;
            break;
        }
    }
    
    if (!p) return false;
    
    QString folderPath = m_rootPath;
    if (p->folderId() != -1) {
        Folder *f = nullptr;
        for (Folder *folder : m_folders) {
            if (folder->id() == p->folderId()) {
                f = folder;
                break;
            }
        }
        if (f) folderPath = QDir(m_rootPath).filePath(f->name());
    }
    
    QString safeTitle = p->title();
    safeTitle.replace(QRegularExpression("[^a-zA-Z0-9_\\-\\s]"), "");
    safeTitle = safeTitle.trimmed();
    if (safeTitle.isEmpty()) safeTitle = "Untitled";
    QString filePath = QDir(folderPath).filePath(safeTitle + ".md");
    
    bool success = QFile::remove(filePath);
    if (success) {
        m_prompts.removeAll(p);
        delete p;
        emit promptDeleted(promptId);
        emit dataChanged();
    }
    return success;
}

Prompt* MarkdownPromptRepository::getPromptById(int promptId)
{
    for (Prompt *p : m_prompts) {
        if (p->id() == promptId) {
            // Return copy
            return new Prompt(p->id(), p->title(), p->content(), p->folderId(), p->createdAt(), p->updatedAt());
        }
    }
    return nullptr;
}

QList<Prompt*> MarkdownPromptRepository::getAllPrompts()
{
    QList<Prompt*> result;
    for (Prompt *p : m_prompts) {
        result.append(new Prompt(p->id(), p->title(), p->content(), p->folderId(), p->createdAt(), p->updatedAt()));
    }
    return result;
}

QList<Prompt*> MarkdownPromptRepository::getPromptsByFolder(int folderId)
{
    QList<Prompt*> result;
    for (Prompt *p : m_prompts) {
        if (p->folderId() == folderId) {
            result.append(new Prompt(p->id(), p->title(), p->content(), p->folderId(), p->createdAt(), p->updatedAt()));
        }
    }
    return result;
}

QList<Prompt*> MarkdownPromptRepository::getPromptsWithoutFolder()
{
    QList<Prompt*> result;
    for (Prompt *p : m_prompts) {
        if (p->folderId() == -1) {
            result.append(new Prompt(p->id(), p->title(), p->content(), p->folderId(), p->createdAt(), p->updatedAt()));
        }
    }
    return result;
}

bool MarkdownPromptRepository::duplicatePrompt(int promptId)
{
    // Implementation uses getPromptById which now returns a copy, so this is safe/unchanged
    Prompt *p = getPromptById(promptId);
    if (!p) return false;
    
    Prompt *newPrompt = new Prompt(this);
    newPrompt->setTitle(p->title() + " (Copy)");
    newPrompt->setContent(p->content());
    newPrompt->setFolderId(p->folderId());
    
    bool result = savePrompt(newPrompt);
    delete p; // Clean up the copy we got
    return result;
}

// Folder operations
bool MarkdownPromptRepository::saveFolder(Folder *folder)
{
    if (!folder) return false;
    
    QString folderName = folder->name();
    // Sanitize
    folderName.replace(QRegularExpression("[^a-zA-Z0-9_\\-\\s]"), "");
    folderName = folderName.trimmed();
    
    QString folderPath = QDir(m_rootPath).filePath(folderName);
    QDir dir(folderPath);
    
    if (folder->isValid()) {
        // Internal lookup
        Folder *existing = nullptr;
        for (Folder *f : m_folders) {
            if (f->id() == folder->id()) {
                existing = f;
                break;
            }
        }
        
        if (existing && existing->name() != folderName) {
            QString oldPath = QDir(m_rootPath).filePath(existing->name());
            QDir().rename(oldPath, folderPath);
            existing->setName(folderName);
            emit folderUpdated(folder);
        }
    } else {
        // New folder
        if (dir.mkpath(".")) {
            folder->setId(m_nextFolderId++);
            // Create internal copy
            Folder* cacheCopy = new Folder(folder->id(), folder->name(), folder->createdAt(), folder->updatedAt(), this);
            m_folders.append(cacheCopy);
            
            emit folderAdded(folder);
        }
    }
    
    emit dataChanged();
    return true;
}

bool MarkdownPromptRepository::deleteFolder(int folderId)
{
    Folder *f = nullptr;
    for (Folder *folder : m_folders) {
        if (folder->id() == folderId) {
            f = folder;
            break;
        }
    }
    
    if (!f) return false;
    
    QString folderPath = QDir(m_rootPath).filePath(f->name());
    QDir dir(folderPath);
    
    // removeRecursively
    if (dir.removeRecursively()) {
        // Also remove all contained prompts from memory
        QList<Prompt*> promptsToRemove;
        for (Prompt *p : m_prompts) {
            if (p->folderId() == folderId) promptsToRemove.append(p);
        }
        for (Prompt *p : promptsToRemove) {
            m_prompts.removeAll(p);
            delete p;
        }
        
        m_folders.removeAll(f);
        delete f;
        emit folderDeleted(folderId);
        emit dataChanged();
        return true;
    }
    return false;
}

Folder* MarkdownPromptRepository::getFolderById(int folderId)
{
    for (Folder *f : m_folders) {
        if (f->id() == folderId) {
            return new Folder(f->id(), f->name(), f->createdAt(), f->updatedAt());
        }
    }
    return nullptr;
}

QList<Folder*> MarkdownPromptRepository::getAllFolders()
{
    QList<Folder*> result;
    for (Folder *f : m_folders) {
        result.append(new Folder(f->id(), f->name(), f->createdAt(), f->updatedAt()));
    }
    return result;
}

QList<Folder*> MarkdownPromptRepository::getFoldersWithCounts()
{
    QList<Folder*> result;
    for (Folder *f : m_folders) {
        // Count internal prompts
        int count = 0;
        for (Prompt *p : m_prompts) {
            if (p->folderId() == f->id()) count++;
        }
        
        // Return copy with count set
        Folder *copy = new Folder(f->id(), f->name(), f->createdAt(), f->updatedAt());
        copy->setPromptCount(count);
        result.append(copy);
    }
    return result;
}

bool MarkdownPromptRepository::folderNameExists(const QString &name, int excludeId)
{
    for (Folder *f : m_folders) {
        if (f->id() == excludeId) continue;
        if (f->name().compare(name, Qt::CaseInsensitive) == 0) return true;
    }
    return false;
}

// Search operations
QList<Prompt*> MarkdownPromptRepository::searchPrompts(const QString &searchText)
{
    QList<Prompt*> result;
    for (Prompt *p : m_prompts) {
        if (p->title().contains(searchText, Qt::CaseInsensitive) || 
            p->content().contains(searchText, Qt::CaseInsensitive)) {
            result.append(new Prompt(p->id(), p->title(), p->content(), p->folderId(), p->createdAt(), p->updatedAt()));
        }
    }
    return result;
}

QList<Prompt*> MarkdownPromptRepository::searchPromptsInFolder(const QString &searchText, int folderId)
{
    QList<Prompt*> result;
    for (Prompt *p : m_prompts) {
        if (p->folderId() == folderId) {
            if (p->title().contains(searchText, Qt::CaseInsensitive) || 
                p->content().contains(searchText, Qt::CaseInsensitive)) {
                result.append(new Prompt(p->id(), p->title(), p->content(), p->folderId(), p->createdAt(), p->updatedAt()));
            }
        }
    }
    return result;
}

// Combined operations
QList<PromptWithFolder*> MarkdownPromptRepository::getPromptsWithFolders()
{
    QList<PromptWithFolder*> result;
    for (Prompt *p : m_prompts) {
        Folder *folderCopy = nullptr;
        // Internal loop used to find folder to clone
        for (Folder *f : m_folders) {
            if (f->id() == p->folderId()) {
                folderCopy = new Folder(f->id(), f->name(), f->createdAt(), f->updatedAt());
                break;
            }
        }
        
        Prompt *promptCopy = new Prompt(p->id(), p->title(), p->content(), p->folderId(), p->createdAt(), p->updatedAt());
        result.append(new PromptWithFolder(promptCopy, folderCopy));
    }
    return result;
}

// Statistics
int MarkdownPromptRepository::getPromptCount()
{
    return m_prompts.count();
}

int MarkdownPromptRepository::getFolderCount()
{
    return m_folders.count();
}

int MarkdownPromptRepository::getPromptCountByFolder(int folderId)
{
    int count = 0;
    for (Prompt *p : m_prompts) {
        if (p->folderId() == folderId) count++;
    }
    return count;
}
