#include "settingwindow.h"
#include "ui_settingwindow.h"
#include <QFont>
#include <QMessageBox>
SettingWindow::SettingWindow(std::map<QString,QString>& settingMap,QWidget *parent) :
    settingMap(settingMap), QDialog(parent),
    ui(new Ui::SettingWindow)
{
    first = true;
    ui->setupUi(this);
    ui->fontSize->setValue(settingMap["font-size"].toInt());
    ui->fontComboBox->setCurrentText(settingMap["font-family"]);
    ui->colorTheme->setCurrentText(settingMap["color-theme"]);
    if((settingMap["syntax-highlighting"]) == "No")
        ui->syntax->setCurrentText("обычный текст");
    else
        ui->syntax->setCurrentText(settingMap["syntax-highlighting"]);
    first = false;
}

SettingWindow::~SettingWindow()
{
    delete ui;
}

void SettingWindow::on_fontSize_valueChanged(int arg1)
{
    settingMap["font-size"] = QString::number(arg1);
}

void SettingWindow::on_fontComboBox_currentTextChanged(const QString &arg1)
{
    settingMap["font-family"] = arg1;
}


void SettingWindow::on_colorTheme_currentTextChanged(const QString &arg1)
{
    settingMap["color-theme"] = arg1;
}

void SettingWindow::on_syntax_currentTextChanged(const QString &arg1)
{
    settingMap["syntax-highlighting"] = arg1;
    if(!first)
        QMessageBox::information(this,"Внимание!!","чтобы изменения вступили в силу необходимо перезапустить приложение");
}
