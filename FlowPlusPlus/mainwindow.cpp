#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_vieweditor.h"
#include "vieweditor.h"
#include <QtGui>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

int tabIndex;
QPlainTextEdit* codeEdit;
QFont textFont;
bool fontChanged = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Flow");
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->splitter_2->setSizes(QList<int>() << 0 << 100);
    ui->splitter->setSizes(QList<int>() << 200 << 10);

    // Adding buttons to menu bar
    QMenuBar *windowBar = new QMenuBar(ui->menubar);
    ui->menubar->addAction(ui->actionRun);
    windowBar->addAction(ui->maximizeButton);
    windowBar->addAction(ui->minimizeButton);
    windowBar->addAction(ui->exitButton);
    ui->menubar->setCornerWidget(windowBar);

    ui->menubar->insertMenu(ui->menuFile->menuAction(), ui->menuFlow);
    ui->menuFlow->setIcon(QIcon(":/images/images/flowIcon.png"));

    // Set up text edit & new tab
    codeEdit = (QPlainTextEdit*) ui->tabWidget->widget(tabIndex);
    tabSetup("Untitled");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::tabSetup(QString fileName)
{
    // Creates new tab and indexes accordingly
    ui->tabWidget->insertTab(ui->tabWidget->count(), new NewTab(), QIcon(QString("")), fileName);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);

    // Adds correct highlighter to the text area
    highlighter = new QSourceHighlite::QSourceHighliter(codeEdit->document());
    syntaxExtensions(fileName);
}

void MainWindow::on_actionNew_File_triggered()
{
    tabSetup("Untitled");
}

void MainWindow::on_actionOpen_File_triggered()
{
    // Gets file
    QString file_name = QFileDialog::getOpenFileName(this, "Open File", " ");

    // Checks if it can be opened
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Can't open file: " + file.errorString());
    }
    else {
        // Gets text from file and puts that into new tab
        QTextStream in(&file);
        QFileInfo fileInfo(file.fileName());

        tabSetup(fileInfo.fileName());
        ui->tabWidget->widget(tabIndex)->setWindowFilePath(file_name);
        codeEdit->setPlainText(in.readAll());

        file.close();
    }
}

void MainWindow::on_actionOpen_Folder_triggered()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    // Make model & set path
    QFileSystemModel *model = new QFileSystemModel;
    model->setRootPath(QDir::currentPath());
    // Output model to the treeView
    ui->treeView->setModel(model);
    ui->treeView->setRootIndex(model->index(dir));
    // Make treeView pop up on screen
    ui->splitter_2->setSizes(QList<int>() << 100 << 500);
}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    // Making model & getting path
    QFileSystemModel *model = new QFileSystemModel;
    QString path = model->fileInfo(index).absoluteFilePath();
    // Opens path of selected item
    QFile file(path);
    file.open(QIODevice::ReadOnly | QFile::Text);

    QFileInfo fileInfo(file.fileName());
    QTextStream in(&file);
    // Setting up the new tab
    tabSetup(fileInfo.fileName());
    ui->tabWidget->widget(tabIndex)->setWindowFilePath(path);
    codeEdit->setPlainText(in.readAll());

    file.close();
}

void MainWindow::on_actionSave_triggered()
{
    QString file_name = ui->tabWidget->widget(tabIndex)->windowFilePath();
    // If no file name, then ask to make one
    if (file_name == "") {
        file_name = QFileDialog::getSaveFileName(this, "Save As", " ");
        ui->tabWidget->widget(tabIndex)->setWindowFilePath(file_name);
    }

    // Open file
    QFile file(file_name);
    file.open(QIODevice::WriteOnly | QFile::Text);

    QTextStream out(&file);

    // Get code from the text area
    QString code = codeEdit->toPlainText();
    out << code;
    // Changes tab title  if necessary
    QFileInfo fileInfo(file.fileName());
    ui->tabWidget->setTabText(tabIndex, fileInfo.fileName());

    file.close();
}

void MainWindow::on_actionSave_As_triggered()
{
    // Save-as pop-up
    QString file_name = QFileDialog::getSaveFileName(this, "Save As", " ");
    ui->tabWidget->widget(tabIndex)->setWindowFilePath(file_name);

    // Open file
    QFile file(file_name);
    file.open(QIODevice::WriteOnly | QFile::Text);

    QTextStream out(&file);

    // Get code from the text area
    QString code = codeEdit->toPlainText();
    out << code;
    // Changes tab title
    QFileInfo fileInfo(file.fileName());
    ui->tabWidget->setTabText(tabIndex, fileInfo.fileName());

    file.close();
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}



void MainWindow::on_tabWidget_currentChanged(int index)
{
    tabIndex = index;

    codeEdit = (QPlainTextEdit*) ui->tabWidget->widget(tabIndex);
    if (fontChanged == true) {codeEdit->document()->setDefaultFont(textFont);}

    highlighter = new QSourceHighlite::QSourceHighliter(codeEdit->document());
    syntaxExtensions(ui->tabWidget->widget(tabIndex)->windowFilePath());
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    QString file_name = ui->tabWidget->widget(index)->windowFilePath();

    // Checks if doc is empty
    if (codeEdit->document()->isEmpty()) {
        ui->tabWidget->removeTab(index);
        return;
    }
    // Checks if file has a name
    if (file_name == "") {
        file_name = QFileDialog::getSaveFileName(this, "Save As", " ");
    }

    // Opens file & saves it
    QFile file(file_name);
    file.open(QIODevice::WriteOnly | QFile::Text);

    QTextStream out(&file);

    QString code = codeEdit->toPlainText();
    out << code;

    file.close();

    ui->tabWidget->removeTab(index);

}

void MainWindow::on_actionFull_Screen_triggered()
{
    if (!isFullScreen()) {MainWindow::showFullScreen();}
    else {MainWindow::showNormal();}
}

void MainWindow::on_actionRun_triggered()
{
    on_actionSave_triggered();

    QFile file = ui->tabWidget->widget(tabIndex)->windowFilePath();

    QFileInfo fileInfo(file.fileName());
    QString filename = (fileInfo.fileName());

    // Gets strictly the name, ex. "main" from main.cpp
    QString baseName = fileInfo.baseName();

    // Directory of file
    QDir dir = QFileInfo(file).absoluteDir();
    // Absolute path of file
    QString absolute = dir.absolutePath();

    std::string stdFileName = filename.toStdString();

    // Extension of file, ex, ".cpp" from main.cpp
    std::string extension = stdFileName.substr(stdFileName.find_last_of(".") + 1);
    std::string command;

    // Checks for extension & runs the correct language
    if (extension == "cpp" || extension == "c")
    {
        QString cd = "cd ";
        QString cmd = cd + '"' + absolute + '"' + " && " + "g++ " + filename + " -o " + baseName + " && " + '"' + absolute + "/" + '"' + baseName;
        command = cmd.toStdString();
    }
    else if (extension == "py")
    {
        QString commandDir = absolute + "/" + filename;
        command = "python " + commandDir.toStdString();
    }
    else if (extension == "js")
    {
        QString commandDir = absolute + "/" + filename;
        command = "node " + commandDir.toStdString();
    }
    else if (extension == "java")
    {
        std::string baseNameStd = baseName.toStdString();
        QString directory = dir.path();
        QString cmd = "cd " + directory + " && " + "javac " + filename + " && " + "java " + baseName;
        command = cmd.toStdString();
    }

    // Gets output from terminal & displays it to the in-app terminal
    char buffer[128];
    QString output = "";

    FILE *in = _popen(command.c_str(), "r");
    if (!in)
        {
            ui->terminal->setPlainText("Run command failed");
        }
    // reads file
    while (!feof(in))
    {
        // use buffer for reading & add to output
        if (fgets(buffer, 128, in) != NULL)
            output += buffer;
    }

    _pclose(in);

    if (output == "")
    {
        ui->terminal->setPlainText(QString::fromStdString(command));
    }
    else
    {
        ui->terminal->setPlainText(output);
        ui->terminal->insertPlainText(QString::fromStdString(command) + "\n");
    }
}

void MainWindow::syntaxExtensions(QString fileName)
{

    std::string stdFileName = fileName.toStdString();
    std::string extension = stdFileName.substr(stdFileName.find_last_of(".") + 1);

    if (extension == "cpp")
    {
        highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeCpp);
    }
    else if (extension == "c")
    {
        highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeC);
    }
    else if (extension == "py")
    {
        highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodePython);
    }
    else if (extension == "js")
    {
        highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeJs);
    }
    else if (extension == "java")
    {
        highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeJava);
    }
    else if (extension == "txt")
    {
        highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeCComment);
    }
    else
    {
        highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeCpp);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    curPos = event->globalPosition().toPoint();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    newPos = QPoint(event->globalPosition().toPoint() - curPos);
    move(x() + newPos.x(), y() + newPos.y());
    curPos = event->globalPosition().toPoint();
}

void MainWindow::on_maximizeButton_triggered()
{
    if (!isFullScreen()) {MainWindow::showFullScreen();}
    else {MainWindow::showNormal();}
}


void MainWindow::on_minimizeButton_triggered()
{
    QMainWindow::showMinimized();
}


void MainWindow::on_exitButton_triggered()
{
    QApplication::quit();
}


void MainWindow::on_actionFont_triggered()
{
    codeEdit = (QPlainTextEdit*) ui->tabWidget->widget(tabIndex);
    // Opens up a dialog box to change font settings
    viewEditor viewEditor;
    viewEditor.setModal(true);
    viewEditor.exec();
}

void viewEditor::on_buttonBox_accepted()
{
    // Changes font based on selected font from box
    QFont font = ui->fontComboBox->currentFont();
    int fontSize = ui->spinBox->value();
    font.setPointSize(fontSize);
    codeEdit->setFont(font);
    textFont = font;
    fontChanged = true;

}
