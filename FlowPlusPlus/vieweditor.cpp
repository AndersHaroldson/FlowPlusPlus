#include "vieweditor.h"
#include "ui_vieweditor.h"

viewEditor::viewEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::viewEditor)
{
    ui->setupUi(this);
    this->setWindowTitle("Font Editor");
    ui->spinBox->setValue(9);
}

viewEditor::~viewEditor()
{
    delete ui;
}

QString fontFamily;

void viewEditor::on_fontComboBox_currentTextChanged(const QString &fontFamilySelected)
{
    fontFamily = fontFamilySelected;
}


void viewEditor::on_fontComboBox_currentFontChanged(const QFont &font)
{
    ui->fontComboBox->setCurrentFont(font);
}




