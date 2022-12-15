#ifndef NewTab_H
#define NewTab_H

#include <QPlainTextEdit>
#include "qsourcehighliter.h"

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

class LineNumberArea;


class NewTab : public QPlainTextEdit
{
    Q_OBJECT

public:
    NewTab(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_plainTextEdit_textChanged();
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;
    QSourceHighlite::QSourceHighliter *highlighter;
};


class LineNumberArea : public QWidget
{
public:
    LineNumberArea(NewTab *editor) : QWidget(editor), NewTab(editor)
    {}

    QSize sizeHint() const override
    {
        return QSize(NewTab->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        NewTab->lineNumberAreaPaintEvent(event);
    }

private:
    NewTab *NewTab;
};


#endif
