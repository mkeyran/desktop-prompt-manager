#include "promptwithfolder.h"

PromptWithFolder::PromptWithFolder(QObject *parent)
    : QObject(parent), m_prompt(nullptr), m_folder(nullptr)
{
}

PromptWithFolder::PromptWithFolder(Prompt *prompt, Folder *folder, QObject *parent)
    : QObject(parent), m_prompt(prompt), m_folder(folder)
{
}

QString PromptWithFolder::folderName() const
{
    return m_folder ? m_folder->name() : QString();
}

void PromptWithFolder::setPrompt(Prompt *prompt)
{
    if (m_prompt != prompt) {
        m_prompt = prompt;
        emit promptChanged();
    }
}

void PromptWithFolder::setFolder(Folder *folder)
{
    if (m_folder != folder) {
        m_folder = folder;
        emit folderChanged();
        emit folderNameChanged();
    }
}