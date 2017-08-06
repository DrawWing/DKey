#ifndef TXTWINDOW_H
#define TXTWINDOW_H

#include <QMainWindow>

class QAction;
class QMenu;
class QTextBrowser;

class TxtWindow: public QMainWindow
{
    Q_OBJECT

public:
    TxtWindow(QWidget *parent = 0);
    TxtWindow(const QString & txt, QWidget *parent = 0);
    void update(const QString & txt);

protected:
    void closeEvent(QCloseEvent *event);

private slots:

private:
    void readSettings();
    void writeSettings();

    QTextBrowser *textEdit;
};

#endif //TXTWINDOW_H
