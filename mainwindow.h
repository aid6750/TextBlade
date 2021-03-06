#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <map>
#include <QTextCursor>
#include "file.h"
#include "mysyntax.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    File file;
    void closePair(QTextCursor&, char ch);
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent *event) override;
    void setColorTheme(const QString& );
    MainWindow(QString& FileName, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_save_triggered();

    void on_setting_triggered();

    void on_textEdit_textChanged();

    void on_open_triggered();

    void on_create_triggered();

    void on_save_as_triggered();

    void on_exit_triggered();
    void on_find_triggered();

    void on_reset_triggered();

    void on_findEdit_returnPressed();

    void on_aboutQt_triggered();

private:
    MySyntax* syntax = nullptr;
    int charCount;
    QString settingFile;
    std::map<QString,QString> settingMap;
};
#endif // MAINWINDOW_H
