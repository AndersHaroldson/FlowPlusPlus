#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "newtab.h"
#include "qsourcehighliter.h"
#include <QFileDialog>
#include <QPoint>
#include <QMouseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionNew_File_triggered();

    void on_actionOpen_File_triggered();

    void on_tabWidget_currentChanged(int index);

    void on_actionOpen_Folder_triggered();

    void on_treeView_doubleClicked(const QModelIndex &index);

    void on_actionSave_triggered();

    void on_actionSave_As_triggered();

    void on_actionExit_triggered();

    void on_tabWidget_tabCloseRequested(int index);

    void on_actionFull_Screen_triggered();

    void on_actionRun_triggered();

    void on_maximizeButton_triggered();

    void on_minimizeButton_triggered();

    void on_exitButton_triggered();

    void on_actionFont_triggered();

private:
    Ui::MainWindow *ui;
    NewTab *newTab;
    QSourceHighlite::QSourceHighliter *highlighter;
    void tabSetup(QString fileName);
    void syntaxExtensions(QString fileName);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    QPoint curPos, newPos;
};
#endif // MAINWINDOW_H
