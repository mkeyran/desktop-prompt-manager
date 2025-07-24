#ifndef PLACEHOLDERUTILS_H
#define PLACEHOLDERUTILS_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QRegularExpression>

class PlaceholderUtils : public QObject
{
    Q_OBJECT

public:
    explicit PlaceholderUtils(QObject *parent = nullptr);
    
    // Static utility methods
    Q_INVOKABLE static QStringList extractPlaceholders(const QString &text);
    Q_INVOKABLE static QString replacePlaceholders(const QString &text, const QMap<QString, QString> &values);
    Q_INVOKABLE static bool hasPlaceholders(const QString &text);
    Q_INVOKABLE static int placeholderCount(const QString &text);
    
    // Default value support
    Q_INVOKABLE static QString extractDefaultValue(const QString &placeholderContent);
    Q_INVOKABLE static QString extractPlaceholderName(const QString &placeholderContent);
    
    // Preview functionality
    Q_INVOKABLE static QString generatePreview(const QString &text, const QMap<QString, QString> &values);
    
    // Validation
    Q_INVOKABLE static bool isValidPlaceholder(const QString &placeholder);
    Q_INVOKABLE static QString cleanPlaceholderName(const QString &name);

private:
    static const QRegularExpression placeholderRegex();
    static QString trimPlaceholderName(const QString &name);
};

#endif // PLACEHOLDERUTILS_H