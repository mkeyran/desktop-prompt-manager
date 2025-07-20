#ifndef SEARCHFILTER_H
#define SEARCHFILTER_H

#include <QObject>
#include <QString>
#include <QList>
#include "../models/prompt.h"

class SearchFilter : public QObject
{
    Q_OBJECT

public:
    explicit SearchFilter(QObject *parent = nullptr);

    // Static filter methods
    static QList<Prompt*> filterPrompts(const QList<Prompt*> &prompts, const QString &searchText);
    static QList<Prompt*> filterByFolder(const QList<Prompt*> &prompts, int folderId);
    static QList<Prompt*> applyFilters(const QList<Prompt*> &prompts, const QString &searchText, int folderId);

private:
    static bool matchesSearch(Prompt *prompt, const QString &searchText);
    static bool matchesFolder(Prompt *prompt, int folderId);
};

#endif // SEARCHFILTER_H