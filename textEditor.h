#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QMainWindow>
#include <QMap>
#include <QPointer>
#include <QTextBrowser>

class QAction;
class QComboBox;
class QFontComboBox;
class QTextEdit;
class QTextCharFormat;
class QMenu;
class QPrinter;

#include "mainwindow.h"
#include "dkFormat.h"

class dkEditor : public QMainWindow
{
    Q_OBJECT

public:
    dkEditor(QString * txt, MainWindow *inParent = 0);

//    bool load(const QString &f);

public slots:

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void textBold();
    void textUline();
    void textItalic();
    void clearFormat();
    void currentCharFormatChanged(const QTextCharFormat &format);
    void showContextMenu(const QPoint &pt);
    void clickedLink(QUrl inUrl);

private:
    void setupTextActions();
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void goToGlossary(int nr);

    QAction *actionTextBold;
    QAction *actionTextUline;
    QAction *actionTextItalic;

//    QTextEdit *textEdit;
    QTextBrowser *textEdit;
    QString *introString;
    QWidget *parent;
    dkFormat * format;
};

#endif // TEXTEDITOR_H
