#include "placeholderviewmodel.h"
#include "../utils/placeholderutils.h"
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>

PlaceholderViewModel::PlaceholderViewModel(QObject *parent)
    : QObject(parent), m_currentIndex(-1)
{
}

void PlaceholderViewModel::setOriginalContent(const QString &content)
{
    if (m_originalContent != content) {
        m_originalContent = content;
        emit originalContentChanged();
        initialize(content);
    }
}

void PlaceholderViewModel::setCurrentIndex(int index)
{
    if (index >= 0 && index < m_placeholders.size() && m_currentIndex != index) {
        saveCurrentValue(); // Save current value before changing index
        m_currentIndex = index;
        updateCurrentValue();
        emit currentIndexChanged();
        emit currentPlaceholderChanged();
        emitNavigationSignals();
    }
}

QString PlaceholderViewModel::currentPlaceholder() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_placeholders.size()) {
        return m_placeholders.at(m_currentIndex);
    }
    return QString();
}

void PlaceholderViewModel::setCurrentValue(const QString &value)
{
    if (m_currentValue != value) {
        m_currentValue = value;
        emit currentValueChanged();
        updateProcessedContent();
    }
}

QString PlaceholderViewModel::processedContent() const
{
    return m_processedContent;
}

bool PlaceholderViewModel::isComplete() const
{
    if (m_placeholders.isEmpty()) {
        return true;  // No placeholders means it's complete
    }
    
    for (const QString &placeholder : m_placeholders) {
        // Check if user provided a value
        bool hasUserValue = m_values.contains(placeholder) && !m_values[placeholder].isEmpty();
        
        if (!hasUserValue) {
            // Check if placeholder has a default value by searching in original content
            QRegularExpression regex("\\{\\{([^}]+)\\}\\}");
            QRegularExpressionMatchIterator iterator = regex.globalMatch(m_originalContent);
            bool hasDefault = false;
            
            while (iterator.hasNext()) {
                QRegularExpressionMatch match = iterator.next();
                QString placeholderContent = match.captured(1).trimmed();
                QString placeholderName = PlaceholderUtils::extractPlaceholderName(placeholderContent);
                
                if (placeholderName == placeholder) {
                    QString defaultValue = PlaceholderUtils::extractDefaultValue(placeholderContent);
                    if (!defaultValue.isEmpty()) {
                        hasDefault = true;
                        break;
                    }
                }
            }
            
            // If no user value and no default, it's incomplete
            if (!hasDefault) {
                return false;
            }
        }
    }
    return true;
}

bool PlaceholderViewModel::canGoNext() const
{
    return m_currentIndex < m_placeholders.size() - 1;
}

bool PlaceholderViewModel::canGoPrevious() const
{
    return m_currentIndex > 0;
}

void PlaceholderViewModel::initialize(const QString &content)
{
    m_originalContent = content;  // Set the original content first!
    m_values.clear();
    m_placeholders = PlaceholderUtils::extractPlaceholders(content);
    m_currentIndex = m_placeholders.isEmpty() ? -1 : 0;
    m_currentValue = "";
    
    updateCurrentValue();
    updateProcessedContent();
    
    emit originalContentChanged();
    emit placeholdersChanged();
    emit currentIndexChanged();
    emit currentPlaceholderChanged();
    emit currentValueChanged();
    emit isCompleteChanged();
    emitNavigationSignals();
}

void PlaceholderViewModel::reset()
{
    m_values.clear();
    m_currentIndex = m_placeholders.isEmpty() ? -1 : 0;
    m_currentValue = "";
    
    updateCurrentValue();
    updateProcessedContent();
    
    emit currentIndexChanged();
    emit currentPlaceholderChanged();
    emit currentValueChanged();
    emit isCompleteChanged();
    emitNavigationSignals();
}

bool PlaceholderViewModel::goNext()
{
    if (canGoNext()) {
        saveCurrentValue();
        setCurrentIndex(m_currentIndex + 1);
        return true;
    }
    return false;
}

bool PlaceholderViewModel::goPrevious()
{
    if (canGoPrevious()) {
        saveCurrentValue();
        setCurrentIndex(m_currentIndex - 1);
        return true;
    }
    return false;
}

void PlaceholderViewModel::saveCurrentValue()
{
    if (m_currentIndex >= 0 && m_currentIndex < m_placeholders.size()) {
        QString placeholder = m_placeholders.at(m_currentIndex);
        m_values[placeholder] = m_currentValue;
        emit placeholderCompleted(placeholder, m_currentValue);
        updateProcessedContent();
        emit isCompleteChanged();
        
        if (isComplete()) {
            emit allPlaceholdersCompleted();
        }
    }
}

QString PlaceholderViewModel::getPreview() const
{
    // Create a temporary map including current value
    QMap<QString, QString> tempValues = m_values;
    if (m_currentIndex >= 0 && m_currentIndex < m_placeholders.size()) {
        QString currentPlaceholder = m_placeholders.at(m_currentIndex);
        tempValues[currentPlaceholder] = m_currentValue;
    }
    
    return PlaceholderUtils::generatePreview(m_originalContent, tempValues);
}

void PlaceholderViewModel::updateCurrentValue()
{
    if (m_currentIndex >= 0 && m_currentIndex < m_placeholders.size()) {
        QString placeholder = m_placeholders.at(m_currentIndex);
        m_currentValue = m_values.value(placeholder, "");
    } else {
        m_currentValue = "";
    }
    emit currentValueChanged();
}

void PlaceholderViewModel::updateProcessedContent()
{
    // Use the stored values for generating preview
    QString newProcessedContent = PlaceholderUtils::generatePreview(m_originalContent, m_values);
    if (m_processedContent != newProcessedContent) {
        m_processedContent = newProcessedContent;
        emit processedContentChanged();
    }
}

void PlaceholderViewModel::emitNavigationSignals()
{
    emit canGoNextChanged();
    emit canGoPreviousChanged();
}

bool PlaceholderViewModel::hasDefaultValue(const QString &placeholder) const
{
    QRegularExpression regex("\\{\\{([^}]+)\\}\\}");
    QRegularExpressionMatchIterator iterator = regex.globalMatch(m_originalContent);
    
    while (iterator.hasNext()) {
        QRegularExpressionMatch match = iterator.next();
        QString placeholderContent = match.captured(1).trimmed();
        QString placeholderName = PlaceholderUtils::extractPlaceholderName(placeholderContent);
        
        if (placeholderName == placeholder) {
            QString defaultValue = PlaceholderUtils::extractDefaultValue(placeholderContent);
            return !defaultValue.isEmpty();
        }
    }
    
    return false;
}

QString PlaceholderViewModel::getDefaultValue(const QString &placeholder) const
{
    QRegularExpression regex("\\{\\{([^}]+)\\}\\}");
    QRegularExpressionMatchIterator iterator = regex.globalMatch(m_originalContent);
    
    while (iterator.hasNext()) {
        QRegularExpressionMatch match = iterator.next();
        QString placeholderContent = match.captured(1).trimmed();
        QString placeholderName = PlaceholderUtils::extractPlaceholderName(placeholderContent);
        
        if (placeholderName == placeholder) {
            return PlaceholderUtils::extractDefaultValue(placeholderContent);
        }
    }
    
    return QString();
}