#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QFontComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QMenuBar>
#include <QTextCodec>
#include <QTextEdit>
#include <QToolBar>
#include <QTextCursor>
#include <QCloseEvent>

#include "textEditor.h"
#include "dkString.h"

dkEditor::dkEditor(QString *txt, MainWindow *inParent)
    : QMainWindow(inParent)
{
    parent = inParent;
    format = inParent->getFormat();
    introString = txt;
    setWindowTitle(QCoreApplication::applicationName());

    textEdit = new QTextEdit(this);
//    textEdit = new QTextBrowser(this);
    textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(textEdit,SIGNAL(customContextMenuRequested(const QPoint&)),
            this,SLOT(showContextMenu(const QPoint &)));
    connect(textEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this, SLOT(currentCharFormatChanged(QTextCharFormat)));

    //    connect(textEdit, &QTextEdit::currentCharFormatChanged,
//            this, &dkEditor::currentCharFormatChanged);

    //    connect(textEdit, &QTextEdit::cursorPositionChanged,
//            this, &dkEditor::cursorPositionChanged);

//    connect(textEdit, SIGNAL( anchorClicked( QUrl ) ),
//            this, SLOT( clickedLink( QUrl ) ));

    textEdit->setHtml(*txt);
    setCentralWidget(textEdit);

    setToolButtonStyle(Qt::ToolButtonFollowStyle);
    setupTextActions();

//    QFont textFont("Helvetica");
//    textFont.setStyleHint(QFont::SansSerif);
//    textEdit->setFont(textFont);
//    fontChanged(textEdit->font());

    connect(textEdit->document(), &QTextDocument::modificationChanged,
            this, &QWidget::setWindowModified);

    setWindowModified(textEdit->document()->isModified());

    textEdit->setFocus();
}

void dkEditor::setupTextActions()
{
    actionTextBold = new QAction(tr("&Bold"), this);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    actionTextBold->setPriority(QAction::LowPriority);
    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    connect(actionTextBold, SIGNAL(triggered()), this, SLOT(textBold()));
    addAction(actionTextBold);
    actionTextBold->setCheckable(true);

    actionTextItalic = new QAction(tr("&Italic"), this);
    actionTextItalic->setPriority(QAction::LowPriority);
    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    connect(actionTextItalic, SIGNAL(triggered()), this, SLOT(textItalic()));
    addAction(actionTextItalic);
    actionTextItalic->setCheckable(true);

    actionTextUline = new QAction(tr("&Underline"), this);
    actionTextUline->setShortcut(Qt::CTRL + Qt::Key_U);
    actionTextUline->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    actionTextUline->setFont(underline);
    connect(actionTextUline, SIGNAL(triggered()), this, SLOT(textUline()));
    addAction(actionTextUline);
    actionTextUline->setCheckable(true);
}

//bool dkEditor::load(const QString &f)
//{
////    if (!QFile::exists(f))
////        return false;
////    QFile file(f);
////    if (!file.open(QFile::ReadOnly))
////        return false;

//    //    QByteArray data = file.readAll();
//    QByteArray data;
//    data.append(f);
//    QTextCodec *codec = Qt::codecForHtml(data);
//    QString str = codec->toUnicode(data);
//    if (Qt::mightBeRichText(str)) {
//        textEdit->setHtml(str);
//    } else {
//        str = QString::fromLocal8Bit(data);
//        textEdit->setPlainText(str);
//    }

////    setCurrentFileName(f);
//    return true;
//}

void dkEditor::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void dkEditor::textUline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void dkEditor::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void dkEditor::clearFormat()
{
    QTextCharFormat fmt;
    mergeFormatOnWordOrSelection(fmt);
}

void dkEditor::currentCharFormatChanged(const QTextCharFormat &format)
{
    QFont theFont = format.font();
    actionTextItalic->setChecked(theFont.italic());
    actionTextBold->setChecked(theFont.bold());
    actionTextUline->setChecked(theFont.underline());
}

void dkEditor::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void dkEditor::showContextMenu(const QPoint &pt)
{
    QMenu * menu = textEdit->createStandardContextMenu();
    menu->removeAction(menu->actions().at(6)); // remove delete
    menu->addSeparator();
    menu->addAction(actionTextItalic);
    menu->addAction(actionTextBold);
    menu->addAction(actionTextUline);
//    menu->addAction(actionTextClear);
    menu->exec(textEdit->mapToGlobal(pt));
    delete menu;
}

void dkEditor::closeEvent(QCloseEvent *event)
{
    if(isWindowModified())
        parent->setWindowModified(true);

    dkString htmlTxt = textEdit->toHtml();
    htmlTxt = htmlTxt.cleanHtml();
    *introString = htmlTxt;

//    writeSettings();
//    hide();
//    event->ignore();
    event->accept();
}

void dkEditor::clickedLink(QUrl inUrl)
{
    QString fragment =  inUrl.fragment();
    if(!fragment.isEmpty() && fragment[0]=='g')
    {
        QString nrStr = fragment.mid(1);
        bool ok;
        int nr = nrStr.toInt(&ok);
        if(ok)
            goToGlossary(nr-1);
    }
//    else if(fragment[0]=='k')
//    {
//        QString nrStr = fragment.mid(1);
//        bool ok;
//        int nr = nrStr.toInt(&ok);
//        if(ok)
//            goToNumber(nr);
//    }
}

void dkEditor::goToGlossary(int nr)
{
    QString outTxt = format->glossHtml(nr, true);
//    outTxt += "<br />"; // otherwise images in the same line
//    outTxt += format.imagesHtml(outTxt, true);

    // todo: use only pointers to glossary in linkGlossary
//    outTxt = format->linkGlossary(outTxt);

    textEdit->setHtml(outTxt);
    setWindowTitle(tr("Hypertext viewer"));
}
