#ifndef PLACEHOLDERVIEWMODEL_H
#define PLACEHOLDERVIEWMODEL_H

#include <QObject>
#include <QStringList>
#include <QMap>

class PlaceholderViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString originalContent READ originalContent WRITE setOriginalContent NOTIFY originalContentChanged)
    Q_PROPERTY(QStringList placeholders READ placeholders NOTIFY placeholdersChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QString currentPlaceholder READ currentPlaceholder NOTIFY currentPlaceholderChanged)
    Q_PROPERTY(QString currentValue READ currentValue WRITE setCurrentValue NOTIFY currentValueChanged)
    Q_PROPERTY(QString processedContent READ processedContent NOTIFY processedContentChanged)
    Q_PROPERTY(bool isComplete READ isComplete NOTIFY isCompleteChanged)
    Q_PROPERTY(bool canGoNext READ canGoNext NOTIFY canGoNextChanged)
    Q_PROPERTY(bool canGoPrevious READ canGoPrevious NOTIFY canGoPreviousChanged)

public:
    explicit PlaceholderViewModel(QObject *parent = nullptr);

    // Properties
    QString originalContent() const { return m_originalContent; }
    void setOriginalContent(const QString &content);
    
    QStringList placeholders() const { return m_placeholders; }
    
    int currentIndex() const { return m_currentIndex; }
    void setCurrentIndex(int index);
    
    QString currentPlaceholder() const;
    
    QString currentValue() const { return m_currentValue; }
    void setCurrentValue(const QString &value);
    
    QString processedContent() const;
    bool isComplete() const;
    bool canGoNext() const;
    bool canGoPrevious() const;

    // Public methods
    Q_INVOKABLE void initialize(const QString &content);
    Q_INVOKABLE void reset();
    Q_INVOKABLE bool goNext();
    Q_INVOKABLE bool goPrevious();
    Q_INVOKABLE void saveCurrentValue();
    Q_INVOKABLE QString getPreview() const;
    Q_INVOKABLE QMap<QString, QString> getAllValues() const { return m_values; }
    Q_INVOKABLE bool hasDefaultValue(const QString &placeholder) const;
    Q_INVOKABLE QString getDefaultValue(const QString &placeholder) const;

signals:
    void originalContentChanged();
    void placeholdersChanged();
    void currentIndexChanged();
    void currentPlaceholderChanged();
    void currentValueChanged();
    void processedContentChanged();
    void isCompleteChanged();
    void canGoNextChanged();
    void canGoPreviousChanged();
    void placeholderCompleted(const QString &placeholder, const QString &value);
    void allPlaceholdersCompleted();

private:
    void updateCurrentValue();
    void updateProcessedContent();
    void emitNavigationSignals();
    
    QString m_originalContent;
    QStringList m_placeholders;
    QMap<QString, QString> m_values;
    int m_currentIndex;
    QString m_currentValue;
    QString m_processedContent;
};

#endif // PLACEHOLDERVIEWMODEL_H