#include <QtWidgets>

#include "txtwindow.h"

TxtWindow::TxtWindow(QWidget *parent)
: QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    textEdit = new QTextBrowser;
    setCentralWidget(textEdit);
    readSettings();
}

TxtWindow::TxtWindow(const QString & txt, QWidget *parent)
    : QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    textEdit = new QTextBrowser;
    textEdit->setHtml(txt);
    setCentralWidget(textEdit);
    readSettings();
}

void TxtWindow::update(const QString & txt)
{
    textEdit->setHtml(txt);
    show();
}

void TxtWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    hide();
        event->ignore();
    //    event->accept();
}


void TxtWindow::readSettings()
{
    QString companyName = QGuiApplication::organizationName();
    QString windowName = QGuiApplication::applicationName() + " - Viewer";
    QSettings settings(companyName, windowName);
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void TxtWindow::writeSettings()
{
    QString companyName = QGuiApplication::organizationName();
    QString windowName = QGuiApplication::applicationName() + " - Viewer";
    QSettings settings(companyName, windowName);
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

