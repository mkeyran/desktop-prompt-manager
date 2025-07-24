#include "placeholderutils.h"
#include <QRegularExpressionMatchIterator>
#include <QSet>
#include <QDebug>

PlaceholderUtils::PlaceholderUtils(QObject *parent)
    : QObject(parent)
{
}

const QRegularExpression PlaceholderUtils::placeholderRegex()
{
    static QRegularExpression regex("\\{\\{([^}]+)\\}\\}");
    return regex;
}

QStringList PlaceholderUtils::extractPlaceholders(const QString &text)
{
    if (text.isEmpty()) {
        return QStringList();
    }
    
    QStringList orderedPlaceholders;
    QSet<QString> seenPlaceholders;
    QRegularExpressionMatchIterator iterator = placeholderRegex().globalMatch(text);
    
    while (iterator.hasNext()) {
        QRegularExpressionMatch match = iterator.next();
        QString placeholderContent = trimPlaceholderName(match.captured(1));
        QString placeholderName = extractPlaceholderName(placeholderContent);
        if (!placeholderName.isEmpty() && !seenPlaceholders.contains(placeholderName)) {
            orderedPlaceholders.append(placeholderName);
            seenPlaceholders.insert(placeholderName);
        }
    }
    
    return orderedPlaceholders;
}

QString PlaceholderUtils::replacePlaceholders(const QString &text, const QMap<QString, QString> &values)
{
    if (text.isEmpty()) {
        return text;
    }
    
    QString result = text;
    QRegularExpressionMatchIterator iterator = placeholderRegex().globalMatch(text);
    
    // Process matches in reverse order to avoid position shifts
    QList<QRegularExpressionMatch> matches;
    while (iterator.hasNext()) {
        matches.prepend(iterator.next());
    }
    
    for (const QRegularExpressionMatch &match : matches) {
        QString placeholderContent = trimPlaceholderName(match.captured(1));
        QString placeholderName = extractPlaceholderName(placeholderContent);
        QString defaultValue = extractDefaultValue(placeholderContent);
        
        QString replacement;
        if (values.contains(placeholderName) && !values[placeholderName].isEmpty()) {
            replacement = values[placeholderName];
        } else if (!defaultValue.isEmpty()) {
            replacement = defaultValue;
        } else {
            // Keep original placeholder if no value and no default
            continue;
        }
        
        result.replace(match.capturedStart(), match.capturedLength(), replacement);
    }
    
    return result;
}

bool PlaceholderUtils::hasPlaceholders(const QString &text)
{
    return placeholderRegex().match(text).hasMatch();
}

int PlaceholderUtils::placeholderCount(const QString &text)
{
    return extractPlaceholders(text).count();
}

QString PlaceholderUtils::generatePreview(const QString &text, const QMap<QString, QString> &values)
{
    if (text.isEmpty()) {
        return text;
    }
    
    QString result = text;
    QRegularExpressionMatchIterator iterator = placeholderRegex().globalMatch(text);
    
    // Process matches in reverse order to avoid position shifts
    QList<QRegularExpressionMatch> matches;
    while (iterator.hasNext()) {
        matches.prepend(iterator.next());
    }
    
    for (const QRegularExpressionMatch &match : matches) {
        QString placeholderContent = trimPlaceholderName(match.captured(1));
        QString placeholderName = extractPlaceholderName(placeholderContent);
        QString defaultValue = extractDefaultValue(placeholderContent);
        QString replacement;
        
        if (values.contains(placeholderName) && !values[placeholderName].isEmpty()) {
            replacement = values[placeholderName];
        } else if (!defaultValue.isEmpty()) {
            replacement = defaultValue;
        } else {
            replacement = QString("{{%1}}").arg(placeholderName); // Show unfilled placeholders with original syntax
        }
        
        result.replace(match.capturedStart(), match.capturedLength(), replacement);
    }
    
    return result;
}

bool PlaceholderUtils::isValidPlaceholder(const QString &placeholder)
{
    if (placeholder.isEmpty()) {
        return false;
    }
    
    QString pattern = QString("\\{\\{%1\\}\\}").arg(QRegularExpression::escape(placeholder.trimmed()));
    QRegularExpression regex(pattern);
    return regex.isValid();
}

QString PlaceholderUtils::cleanPlaceholderName(const QString &name)
{
    return trimPlaceholderName(name);
}

QString PlaceholderUtils::trimPlaceholderName(const QString &name)
{
    return name.trimmed();
}

QString PlaceholderUtils::extractPlaceholderName(const QString &placeholderContent)
{
    int pipeIndex = placeholderContent.indexOf('|');
    if (pipeIndex == -1) {
        return placeholderContent.trimmed();
    }
    return placeholderContent.left(pipeIndex).trimmed();
}

QString PlaceholderUtils::extractDefaultValue(const QString &placeholderContent)
{
    int pipeIndex = placeholderContent.indexOf('|');
    if (pipeIndex == -1) {
        return QString();
    }
    return placeholderContent.mid(pipeIndex + 1).trimmed();
}