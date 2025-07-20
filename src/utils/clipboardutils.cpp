#include "clipboardutils.h"
#include <QDebug>

ClipboardUtils::ClipboardUtils(QObject *parent)
    : QObject(parent)
{
    m_clipboard = QGuiApplication::clipboard();
    
    // Connect to clipboard changes to emit signal
    connect(m_clipboard, &QClipboard::dataChanged, 
            this, &ClipboardUtils::onClipboardDataChanged);
}

void ClipboardUtils::copyToClipboard(const QString &text)
{
    if (!m_clipboard) {
        qWarning() << "Clipboard not available";
        return;
    }
    
    m_clipboard->setText(text, QClipboard::Clipboard);
    
    // Also copy to selection buffer on X11 systems
    if (m_clipboard->supportsSelection()) {
        m_clipboard->setText(text, QClipboard::Selection);
    }
    
    qDebug() << "Copied to clipboard:" << text.length() << "characters";
}

QString ClipboardUtils::getFromClipboard()
{
    if (!m_clipboard) {
        qWarning() << "Clipboard not available";
        return QString();
    }
    
    return m_clipboard->text(QClipboard::Clipboard);
}

bool ClipboardUtils::hasClipboardText()
{
    if (!m_clipboard) {
        return false;
    }
    
    return !m_clipboard->text(QClipboard::Clipboard).isEmpty();
}

void ClipboardUtils::onClipboardDataChanged()
{
    emit clipboardChanged();
}