#include "searchfilter.h"

SearchFilter::SearchFilter(QObject *parent)
    : QObject(parent)
{
}

QList<Prompt*> SearchFilter::filterPrompts(const QList<Prompt*> &prompts, const QString &searchText)
{
    if (searchText.isEmpty()) {
        return prompts;
    }

    QList<Prompt*> filtered;
    for (Prompt *prompt : prompts) {
        if (matchesSearch(prompt, searchText)) {
            filtered.append(prompt);
        }
    }
    return filtered;
}

QList<Prompt*> SearchFilter::filterByFolder(const QList<Prompt*> &prompts, int folderId)
{
    QList<Prompt*> filtered;
    for (Prompt *prompt : prompts) {
        if (matchesFolder(prompt, folderId)) {
            filtered.append(prompt);
        }
    }
    return filtered;
}

QList<Prompt*> SearchFilter::applyFilters(const QList<Prompt*> &prompts, const QString &searchText, int folderId)
{
    QList<Prompt*> result = prompts;
    
    if (folderId >= 0) {
        result = filterByFolder(result, folderId);
    }
    
    if (!searchText.isEmpty()) {
        result = filterPrompts(result, searchText);
    }
    
    return result;
}

bool SearchFilter::matchesSearch(Prompt *prompt, const QString &searchText)
{
    if (!prompt || searchText.isEmpty()) {
        return true;
    }

    QString lowerSearchText = searchText.toLower();
    return prompt->title().toLower().contains(lowerSearchText) ||
           prompt->content().toLower().contains(lowerSearchText);
}

bool SearchFilter::matchesFolder(Prompt *prompt, int folderId)
{
    if (!prompt) {
        return false;
    }

    if (folderId == 0) {
        // "Uncategorized" - prompts without folder
        return prompt->folderId() <= 0;
    } else if (folderId > 0) {
        // Specific folder
        return prompt->folderId() == folderId;
    } else {
        // All prompts (folderId == -1)
        return true;
    }
}