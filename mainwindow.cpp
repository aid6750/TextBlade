#include "mainwindow.h"
#include <map>
#include "ui_mainwindow.h"
#include "settingwindow.h"
#include <QMessageBox>
#include <QString>
#include <QTextEdit>
#include <QFileDialog>
#include <QFileInfo>
#include <QTextCursor>
#include <mysyntax.h>
#include <QDebug>
MainWindow::MainWindow(QString& fileName , QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textEdit->setTabStopWidth(35);
    ui->findEdit->hide();
    QFile fin;
    settingMap.emplace(std::pair("font-size",""));
    settingMap.emplace(std::pair("font-family",""));
    settingMap.emplace(std::pair("color-theme",""));
    settingMap.emplace(std::pair("syntax-highlighting",""));

    settingFile =  QCoreApplication::applicationDirPath()+"/setting.dat";
    fin.setFileName(settingFile);
    this->setWindowTitle("Text Blade");
    if(!fin.open(QIODevice::ReadOnly))
    {
        fin.open(QIODevice::WriteOnly);
        QTextStream output;
        output.setDevice(&fin);
        settingMap["font-size"] = "15";
        settingMap["color-theme"] = "Dark";
        settingMap["syntax-highlighting"] = "No";
        output << "font-size: " + settingMap["font-size"] + "\n";
        output << "font-family: " + settingMap["font-family"] + "\n";
        output << "syntax-highlighting: " + settingMap["syntax-highlighting"] + "\n";
        fin.close();
    }
    else
    {
        QTextStream input;
        QString temp;
        input.setDevice(&fin);
        input >> temp;
        input >> settingMap["font-size"];
        input >> temp;
        settingMap["font-family"] = input.readLine();
        input >> temp;
        input >> settingMap["color-theme"];
        input >> temp;
        input >> settingMap["syntax-highlighting"];
        input >> temp;
    }
    QFont font;
    font.setFamily(settingMap["font-family"]);
    font.setPixelSize(settingMap["font-size"].toInt());
    font.setBold(ui->textEdit->font().bold());
    ui->textEdit->setFont(font);
    setColorTheme(settingMap["color-theme"]);
    if(settingMap["syntax-highlighting"] == "C++")
        syntax = new MySyntax(ui->textEdit->document());
    fin.close();
    if(!fileName.isNull())
    {
        file.open(fileName);
        ui->textEdit->setText(file.getText());
        file.save(ui->textEdit->toPlainText());
        setWindowTitle("Text Blade - " + file.getFileName());
    }
    setWindowTitle("Text Blade - " + file.getFileName());

}
MainWindow::~MainWindow()
{
    QFile fout;
    fout.setFileName(settingFile);
    fout.open(QIODevice::WriteOnly);
    QTextStream output;
    output.setDevice(&fout);
    output << "font-size: " + settingMap["font-size"] + "\n";
    output << "font-family: " + settingMap["font-family"] + "\n";
    output << "color-theme: " + settingMap["color-theme"] + "\n";
    output << "syntax-highlighting: " + settingMap["syntax-highlighting"] + "\n";
    fout.close();
    delete syntax;
    delete ui;
}

void MainWindow::on_save_triggered()
{
    if(file.isExist())
        file.save(ui->textEdit->toPlainText());
    else
        on_save_as_triggered();
    setWindowTitle("Text Blade - " + file.getFileName());
}

void MainWindow::on_setting_triggered()
{
    SettingWindow settingWindow(settingMap);
    settingWindow.setModal(true);
    settingWindow.exec();
    QFont font;
    font.setFamily(settingMap["font-family"]);
    font.setPixelSize(settingMap["font-size"].toInt());
    font.setBold(ui->textEdit->font().bold());
    ui->textEdit->setFont(font);
    setColorTheme(settingMap["color-theme"]);

}

void MainWindow::closeEvent(QCloseEvent *event)
{
        QMessageBox quit(
        QMessageBox::Question,
        "Text Blade",
        "",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel ,
        this);
        int answer = -200;
        if(!file.isExist() && !ui->textEdit->toPlainText().isEmpty())
        {
            quit.setText("?????????????? ?????????? ?????????");
            quit.setButtonText(QMessageBox::Yes, "??????????????");
            quit.setButtonText(QMessageBox::No, "??????????");
            quit.setButtonText(QMessageBox::Cancel,"????????????");
            answer = quit.exec();
        }
        if(!file.isConserved() && ui->textEdit->toPlainText().isEmpty())
        {
            quit.setText("?????????????????? ?????????????????? ?? ?????????? " +file.getPath() + "?");
            quit.setButtonText(QMessageBox::Yes, "??????????????????");
            quit.setButtonText(QMessageBox::No, "???? ??????????????????????");
            quit.setButtonText(QMessageBox::Cancel,"????????????");
            answer = quit.exec();
        }
        if(answer != -200)
        {
            switch (answer)
            {
                case QMessageBox::No :
                    QApplication::quit();
                    break;
                case QMessageBox::Yes :
                    if(file.isExist())
                        on_save_triggered();
                    else
                        on_save_as_triggered();
                    QApplication::quit();
                    break;
                default :
                    event->ignore();
            }
        }
}

void MainWindow::on_textEdit_textChanged()
{
    QTextCursor dc(ui->textEdit->textCursor());
    QString temp = ui->textEdit->toPlainText();
    if(temp.length() > charCount)
    {
        if(temp[dc.position()-1] == QChar('('))
            closePair(dc,')');
        if(temp[dc.position()-1] == QChar('\"'))
            closePair(dc,'\"');
        if(temp[dc.position()-1] == QChar('{'))
            closePair(dc,'}');
        if(temp[dc.position()-1] == QChar('['))
            closePair(dc,']');
        if(temp[dc.position()-1] == QChar('\''))
            closePair(dc,'\'');
    }
    if(!(ui->textEdit->toPlainText().isEmpty() && !file.isExist()))
        setWindowTitle("Text Blade - " + file.getFileName() + "*");
    file.change();
    int size = temp.split(QRegExp("[\\s]+\\w")).size();
        if(temp[0] == QChar(' ') || temp[0] == QChar('\t') || temp[0] == QChar('\n') || temp.isEmpty() )
            size--;
    QString message = "????????:";
    message += std::to_string(size).data();
    message += "\n????????????????:";
    message += std::to_string(temp.length()).data();
    ui->statusbar->showMessage(message);
    charCount = temp.length();
}

void MainWindow::on_open_triggered()
{
    QString path = QFileDialog::getOpenFileName(0,"?????????? ??????????", "", "");
    file.open(path);
    ui->textEdit->setText(file.getText());
    file.save(ui->textEdit->toPlainText());
    setWindowTitle("Text Blade - " + file.getFileName());
}

void MainWindow::on_create_triggered()
{
    QString path = QFileDialog::getSaveFileName(0, "???????????????? ??????????", "", "");
    file.create(path);
    setWindowTitle("Text Blade - "+ file.getFileName());
}

void MainWindow::on_save_as_triggered()
{
    QString path = QFileDialog::getSaveFileName(0, "???????????????????? ??????????", "", "");
    if(!path.isEmpty())
    {
        file.create(path);
        file.save(ui->textEdit->toPlainText());
        setWindowTitle("Text Blade - " + file.getFileName());
    }
}
void MainWindow::setColorTheme(const QString& colorTheme)
{
    if(colorTheme == "Dark")
    {
        this->setStyleSheet("color:white;background-color:#051B2B;");
        ui->textEdit->setStyleSheet("background-color:#06151a;color:#D6B5F5;");
    }
    if(colorTheme == "Classic")
    {
        this->setStyleSheet("color:black;background-color:white");
        ui->textEdit->setStyleSheet("color:black;background-color:white");
    }
}

void MainWindow::on_exit_triggered()
{
   QCloseEvent event;
   closeEvent(&event);
}

void MainWindow::closePair(QTextCursor& dc ,char ch)
{
    QChar c(ch);
    QString temp = ui->textEdit->toPlainText();
    int pos = dc.position();
    if((temp[pos] == QChar(' ') || temp.length() == pos))
        temp[pos] = c;
    ui->textEdit->setText(temp);
    dc.setPosition(pos);
    ui->textEdit->setTextCursor(dc);
}

void MainWindow::on_find_triggered()
{
    ui->findEdit->show();
}

void MainWindow::on_reset_triggered()
{
    ui->findEdit->hide();
}

void MainWindow::on_findEdit_returnPressed()
{
    ui->findEdit->hide();
    int index = ui->textEdit->toPlainText().indexOf(ui->findEdit->text(),0);
    QTextCursor dc(ui->textEdit->textCursor());
    dc.setPosition(index);
    ui->textEdit->setTextCursor(dc);
}

void MainWindow::on_aboutQt_triggered()
{
    QMessageBox::aboutQt(this,"About Qt");
}
