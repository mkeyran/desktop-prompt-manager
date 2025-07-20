#ifndef CLIPBOARDUTILS_H
#define CLIPBOARDUTILS_H

#include <QObject>
#include <QClipboard>
#include <QGuiApplication>

class ClipboardUtils : public QObject
{
    Q_OBJECT

public:
    explicit ClipboardUtils(QObject *parent = nullptr);

    Q_INVOKABLE void copyToClipboard(const QString &text);
    Q_INVOKABLE QString getFromClipboard();
    Q_INVOKABLE bool hasClipboardText();

signals:
    void clipboardChanged();

private slots:
    void onClipboardDataChanged();

private:
    QClipboard *m_clipboard;
};

#endif // CLIPBOARDUTILS_H