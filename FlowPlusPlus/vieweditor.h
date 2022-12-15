#ifndef VIEWEDITOR_H
#define VIEWEDITOR_H

#include <QDialog>

namespace Ui {
class viewEditor;
}

class viewEditor : public QDialog
{
    Q_OBJECT

public:
    explicit viewEditor(QWidget *parent = nullptr);
    ~viewEditor();

private slots:

    void on_fontComboBox_currentTextChanged(const QString &fontFamily);

    void on_fontComboBox_currentFontChanged(const QFont &f);

    void on_buttonBox_accepted();

private:
    Ui::viewEditor *ui;
};

#endif // VIEWEDITOR_H
